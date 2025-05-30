//
// Created by Renatus Madrigal on 5/28/2025.
//

#ifndef PHOSPHORUS_INCLUDE_PHOSPHORUS_ANIMATE_H
#define PHOSPHORUS_INCLUDE_PHOSPHORUS_ANIMATE_H

#include "phosphorus/Coordinate.h"
#include <boost/asio.hpp>
#include <opencv2/videoio.hpp>
#include <random>
#include <span>

namespace phosphorus {

class AnimateGenerator {
public:
  AnimateGenerator() = default;
  AnimateGenerator(std::string name, size_t num_threads)
      : name_(std::move(name)), pool_(num_threads) {}

  AnimateGenerator(const AnimateGenerator &) = delete;
  AnimateGenerator &operator=(const AnimateGenerator &) = delete;
  AnimateGenerator(AnimateGenerator &&) = delete;
  AnimateGenerator &operator=(AnimateGenerator &&) = delete;

  // Generate the animation.
  void generate(const std::string &filename, std::span<Cartesian2D> points,
                double time);

  void pushPoints(std::span<Cartesian2D> points) {
    point_list_.emplace_back(points);
  }

  void generate(const std::string &filename, double time);

private:
  void setup();
  void blockWorkflow();
  void generateDatafile(std::span<Cartesian2D> points);
  void generateKeyframeBlock(int start, int end);
  void mergeBlock(const std::vector<cv::Mat> &keyframes, int start,
                  int end) const;
  void handleBlock(int start, int end);
  void cleanup();

  static auto getUniqueId() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis{};
    return std::to_string(dis(gen));
  }

  static cv::Mat interpolateFrames(const cv::Mat &prev, const cv::Mat &next,
                                   double t);

  static constexpr double kFPS = 30.0;         // Frames per second
  static constexpr unsigned int kWidth = 800;  // Width of the video
  static constexpr unsigned int kHeight = 600; // Height of the video

  double delay_ = 0.1; // Default delay between frames
  double time_ = 60;   // Default total time for the animation
  std::string name_ = "Animate";
  boost::asio::thread_pool pool_{4};
  std::string current_temp_;
  std::unique_ptr<cv::VideoWriter> writer_;
  std::vector<std::span<Cartesian2D>> point_list_;
  int interpolation_steps_ = 0; // Default interpolation steps
  double min_x = 0, max_x = 0, min_y = 0, max_y = 0, min_z = 0, max_z = 0;
};

} // namespace phosphorus

#endif // PHOSPHORUS_INCLUDE_PHOSPHORUS_ANIMATE_H
