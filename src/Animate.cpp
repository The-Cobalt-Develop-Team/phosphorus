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
using std::format;

namespace phosphorus {

void AnimateGenerator::generate(const std::string &filename,
                                std::span<Cartesian2D> points, double time) {
  try {
    name_ = filename;
    time_ = time;
    setup();
    generateKeyframes(points);
    mergeKeyframes(points.size());
    cleanup();
  } catch (std::exception &e) {
    std::cerr << "Error during setup: " << e.what() << '\n';
  }
}

void AnimateGenerator::setup() {
  if (!current_temp_.empty())
    cleanup();
  current_temp_ = "temp_" + getUniqueId();
  fs::create_directory(current_temp_);
}

void AnimateGenerator::generateKeyframes(std::span<Cartesian2D> points) const {
  std::vector<double> x =
      points | ranges::views::transform([](const auto &p) { return p[0]; }) |
      ranges::to<std::vector<double>>();
  std::vector<double> y =
      points | ranges::views::transform([](const auto &p) { return p[1]; }) |
      ranges::to<std::vector<double>>();

  // set the scaler
  static constexpr double kScaleFactor = 1.1;
  auto max_x = *ranges::max_element(x) * kScaleFactor;
  auto max_y = *ranges::max_element(y) * kScaleFactor;
  auto min_x = *ranges::min_element(x) * kScaleFactor;
  auto min_y = *ranges::min_element(y) * kScaleFactor;

  auto temp_data = fs::path(current_temp_) / (name_ + ".dat");
  auto temp_name = temp_data.string();
  Gnuplot::generateDataBlock(temp_data.string(), x, y);

  // std::ofstream temp("temp.txt");

#pragma omp parallel for
  for (int i = 0; i < points.size(); i++) {
    Gnuplot plot;
    plot.execute(format("set terminal pngcairo enhanced size 800,600\n"))
        .execute(format("set output '{}/{}_{}.png'\n", current_temp_, name_, i))
        .setFigureConfig({
            .xrange = {min_x, max_x},
            .yrange = {min_y, max_y},
            .xlabel = "X",
            .ylabel = "Y",
            .grid = true,
        })
        .plot({
            .index = 0,
            .every = {1, i + 1},
            .with = Gnuplot::PlotConfig::PlotType::Lines,
            .style = "lt 1 lw 2 notitle",
        })
        .plot({
            .index = 0,
            .every = {i + 1, i + 1},
            .with = Gnuplot::PlotConfig::PlotType::Points,
            .style = "pt 5 ps 1 lc rgb 'red' notitle",
        });
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

cv::Mat interpolateFrames(const cv::Mat &prev, const cv::Mat &next, double t) {
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

void mergeBlock(const std::vector<cv::Mat> &keyframes, int start, int end,
                int interpolation_steps, cv::VideoWriter &writer) {
  using namespace cv;
  using namespace ranges;
  using std::format;
  using std::vector;

  // Static output_frames to avoid reallocation
  static vector<Mat> output_frames;

  auto total = (end - start) * (interpolation_steps + 1);
  output_frames.resize(total);

  std::cout << format(
      "Processing keyframes from {} to {} with {} interpolation steps\n", start,
      end - 1, interpolation_steps);
  std::cout << format("Total frames to generate: {}\n", total);

#pragma omp parallel for
  for (int i = 0; i < end - start; ++i) {
    auto base_idx = i * (interpolation_steps + 1);
    output_frames[base_idx] = keyframes[i + start];

    for (int j = 1; j <= interpolation_steps; ++j) {
      auto t = static_cast<double>(j) / (interpolation_steps + 1);
      output_frames[base_idx + j] =
          interpolateFrames(keyframes[i + start], keyframes[i + start + 1], t);
    }
    std::cout << format("Interpolated frames between keyframes {} and {}\n",
                        i + start, i + start + 1);
  }

  for (auto &&[idx, frame] : views::enumerate(output_frames)) {
    if (frame.empty()) {
      if (idx % (interpolation_steps + 1) == 0) {
        std::cerr << format("Warning: Keyframe {} is empty, skipping.\n", idx);
      } else {
        std::cerr << format(
            "Warning: Interpolated frame {} is empty, skipping.\n", idx);
      }
      continue;
    }
    writer.write(frame);
  }
}

void AnimateGenerator::mergeKeyframes(int n) const {
  using namespace cv;
  using namespace ranges;
  using std::format;
  using std::vector;
  vector<Mat> keyframes =
      views::iota(0, n) | views::transform([this](int i) {
        return fs::path(current_temp_) / format("{}_{}.png", name_, i);
      }) |
      views::transform(
          [this](const auto &filename) { return imread(filename.string()); }) |
      ranges::to<std::vector<Mat>>();

  auto size = keyframes[0].size();
  VideoWriter writer(format("{}.mp4", name_),
                     VideoWriter::fourcc('m', 'p', '4', 'v'), 30, size);

  auto interpolation_steps =
      std::min(static_cast<int>(std::ceil((time_ * 30.0 - n) / (n - 1))), 1);

  static constexpr int kBlockFactor = 50;
  auto block_count = (n + kBlockFactor - 1) / kBlockFactor;
  auto step = (n + block_count - 1) / block_count; // Calculate step size

  for (int i = 0; i < block_count; ++i) {
    int start = i * step;
    int end = std::min(start + step, n - 1);
    if (start < end) {
      std::cout << format("Processing block {}: keyframes {} to {}\n", i, start,
                          end - 1);
      mergeBlock(keyframes, start, end, interpolation_steps, writer);
    }
  }

  // Write the last keyframe
  writer.write(keyframes.back());

  writer.release();
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
