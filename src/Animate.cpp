//
// Created by Renatus Madrigal on 5/28/2025.
//

#include "phosphorus/Animate.h"
#include "phosphorus/Gnuplot.h"
#include "range/v3/algorithm/max_element.hpp"
#include "range/v3/algorithm/min_element.hpp"
#include <boost/filesystem.hpp>
#include <format>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <range/v3/range.hpp>
#include <range/v3/view.hpp>

namespace fs = boost::filesystem;
using std::cout;
using std::format;
using std::string;
using std::vector;
using namespace ranges;

namespace phosphorus {

void AnimateGenerator::generate(const std::string &filename,
                                std::span<Cartesian2D> points, double time) {
  try {
    name_ = filename;
    time_ = time;
    point_list_ = {points};
    setup();
    generateDatafile(points);
    blockWorkflow();
    cleanup();
  } catch (std::exception &e) {
    std::cerr << "Error during setup: " << e.what() << '\n';
  }
}

void AnimateGenerator::generate(const std::string &filename, double time) {
  if (point_list_.empty()) {
    throw std::runtime_error("No points to generate animation");
  }
  try {
    name_ = filename;
    time_ = time;
    setup();
    for (const auto &points : point_list_) {
      generateDatafile(points);
    }
    blockWorkflow();
    cleanup();
  } catch (std::exception &e) {
    std::cerr << format("Error during generation: {}", e.what());
  }
}

void AnimateGenerator::setup() {
  if (!current_temp_.empty())
    cleanup();
  current_temp_ = "temp_" + getUniqueId();
  fs::create_directory(current_temp_);
}

void AnimateGenerator::generateDatafile(std::span<Cartesian2D> points) {
  auto temp_file = fs::path(current_temp_) / (name_ + ".dat");
  auto temp_name = temp_file.string();
  vector<double> x = points |
                     views::transform([](const auto &p) { return p[0]; }) |
                     to<std::vector<double>>();
  vector<double> y = points |
                     views::transform([](const auto &p) { return p[1]; }) |
                     to<std::vector<double>>();

  static constexpr double kScaleFactor = 1.1; // Scale factor for coordinates
  min_x = min(*ranges::min_element(x) * kScaleFactor, min_x);
  max_x = max(*ranges::max_element(x) * kScaleFactor, max_x);
  min_y = min(*ranges::min_element(y) * kScaleFactor, min_y);
  max_y = max(*ranges::max_element(y) * kScaleFactor, max_y);

  Gnuplot::generateDataBlock(temp_name, x, y);
}

void AnimateGenerator::blockWorkflow() {
  static constexpr int kBlockFactor = 50;
  auto n = point_list_[0].size(); // Assuming all spans have the same size
  auto block_count = (n + kBlockFactor - 1) / kBlockFactor;
  auto step = (n + block_count - 1) / block_count; // Calculate step size

  writer_ = std::make_unique<cv::VideoWriter>(
      format("{}.mp4", name_), cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
      kFPS, cv::Size(kWidth, kHeight)); // Assuming a fixed size for simplicity

  // TODO: Fix the interpolation algorithm.
  // interpolation_steps_ = static_cast<int>((kFPS * time_ - n) / (n - 1));

  for (auto i = 0; i < block_count; ++i) {
    auto start = i * step;
    auto end = std::min(start + step, n - 1);
    if (start < end) {
      std::cout << format("Processing block {}: keyframes {} to {}\n", i, start,
                          end - 1);
      handleBlock(start, end);
    }
  }

  writer_->release();
}

cv::Mat AnimateGenerator::interpolateFrames(const cv::Mat &prev,
                                            const cv::Mat &next, double t) {
  using namespace cv;
  Mat prev_gray, next_gray;
  cvtColor(prev, prev_gray, COLOR_BGR2GRAY);
  cvtColor(next, next_gray, COLOR_BGR2GRAY);

  Mat flow_forward, flow_backward;
  calcOpticalFlowFarneback(prev_gray, next_gray, flow_forward, //
                           0.5, 3, 15, 3, 5, 1.2, 0);
  calcOpticalFlowFarneback(next_gray, prev_gray, flow_backward, //
                           0.5, 3, 15, 3, 5, 1.2, 0);

  Mat map_forward(prev.size(), CV_32FC2);
  Mat map_backward(prev.size(), CV_32FC2);

#pragma omp parallel for collapse(2)
  for (int y = 0; y < prev.rows; ++y) {
    for (int x = 0; x < prev.cols; ++x) {
      Vec2f fw = flow_forward.at<Vec2f>(y, x);
      map_forward.at<Vec2f>(y, x) = Vec2f(x + fw[0] * t, y + fw[1] * t);

      Vec2f bw = flow_backward.at<Vec2f>(y, x);
      map_backward.at<Vec2f>(y, x) =
          Vec2f(x + bw[0] * (1 - t), y + bw[1] * (1 - t));
    }
  }

  Mat warped_prev, warped_next;
  remap(prev, warped_prev, map_forward, Mat(), INTER_LINEAR);
  remap(next, warped_next, map_backward, Mat(), INTER_LINEAR);

  Mat blended;
  addWeighted(warped_prev, 1 - t, warped_next, t, 0, blended);
  return blended;
}

void AnimateGenerator::generateKeyframeBlock(int start, int end) {
  auto temp_data = fs::path(current_temp_) / (name_ + ".dat");
  auto temp_name = temp_data.string();

#pragma omp parallel for
  for (int i = start; i < end; i++) {
    Gnuplot plot;
    plot.execute(format("set terminal pngcairo enhanced size 800,600\n"))
        .execute(format("set output '{}/{}_{}.png'\n", current_temp_, name_, i))
        .setFigureConfig({
            .width = kWidth,
            .height = kHeight,
            .xrange = {min_x, max_x},
            .yrange = {min_y, max_y},
            .xlabel = "X",
            .ylabel = "Y",
            .grid = true,
        });
    for (int idx = 0; idx < point_list_.size(); ++idx) {
      plot.plot({
                    .index = idx,
                    .every = {1, i + 1},
                    .with = Gnuplot::PlotConfig::PlotType::Lines,
                    .style = "lw 2 notitle",
                })
          .plot({
              .index = idx,
              .every = {i + 1, i + 1},
              .with = Gnuplot::PlotConfig::PlotType::Points,
              .style = "pt 5 ps 1 lc rgb 'red' notitle",
          });
    }
    auto command = plot.generatePlotCommand(temp_name);
    plot.execute(command);
    // temp << command;
    plot.execute("exit\n");
    auto res = plot.wait();
    if (res != 0) {
      std::cerr << format("Error during generating keyframes: {}\n", res);
    } else {
      std::cout << format("Generated keyframe {}: {}_{}.png\n", i,
                          current_temp_, i);
    }
  }
}

void AnimateGenerator::mergeBlock(const std::vector<cv::Mat> &keyframes,
                                  int start, int end) const {
  using namespace cv;
  using namespace ranges;
  using std::format;
  using std::vector;

  // Static output_frames to avoid reallocation
  static vector<Mat> output_frames;

  auto total = (end - start) * (interpolation_steps_ + 1);
  output_frames.resize(total);

  std::cout << format(
      "Processing keyframes from {} to {} with {} interpolation steps\n", start,
      end - 1, interpolation_steps_);
  std::cout << format("Total frames to generate: {}\n", total);

#pragma omp parallel for
  for (int i = 0; i < end - start; ++i) {
    auto base_idx = i * (interpolation_steps_ + 1);
    output_frames[base_idx] = keyframes[i];

    for (int j = 1; j <= interpolation_steps_; ++j) {
      auto t = static_cast<double>(j) / (interpolation_steps_ + 1);
      output_frames[base_idx + j] =
          interpolateFrames(keyframes[i], keyframes[i + 1], t);
    }
    std::cout << format("Interpolated frames between keyframes {} and {}\n",
                        i + start, i + start + 1);
  }

  for (auto &&[idx, frame] : views::enumerate(output_frames)) {
    if (frame.empty()) {
      if (idx % (interpolation_steps_ + 1) == 0) {
        std::cerr << format("Warning: Keyframe {} is empty, skipping.\n",
                            idx + start);
      } else {
        std::cerr << format(
            "Warning: Interpolated frame {} is empty, skipping.\n", idx);
      }
      continue;
    }
    writer_->write(frame);
  }
}

void AnimateGenerator::handleBlock(int start, int end) {
  using cv::Mat;

  generateKeyframeBlock(start, end + 1);
  cout << format("Keyframes generated from {} to {}\n", start, end - 1);

  vector<string> filenames =
      views::iota(start, end + 1) | views::transform([this](int i) {
        return format("{}/{}_{}.png", current_temp_, name_, i);
      }) |
      to<vector<string>>();

  vector<Mat> keyframes = filenames |
                          views::transform([](const string &filename) {
                            return cv::imread(filename);
                          }) |
                          to<std::vector<Mat>>();

  cout << format("Loaded {} keyframes from temporary files\n",
                 keyframes.size());

  mergeBlock(keyframes, start, end);
  cout << format("Merged keyframes from {} to {}\n", start, end - 1);

  for (auto &&filename : filenames) {
    try {
      fs::remove(filename);
      std::cout << format("Removed temporary file: {}\n", filename);
    } catch (const fs::filesystem_error &e) {
      std::cerr << format("Error removing file {}: {}\n", filename, e.what());
    }
  }
}

void AnimateGenerator::cleanup() {
  if (!current_temp_.empty()) {
    try {
      fs::remove_all(current_temp_);
      current_temp_ = "";
    } catch (const fs::filesystem_error &e) {
      // Log the error or handle it as needed
      std::cerr << "Error cleaning up temporary files: " << e.what() << '\n';
    }
  }
}

} // namespace phosphorus
