//
// Created by Renatus Madrigal on 5/28/2025.
//

#ifndef PHOSPHORUS_INCLUDE_PHOSPHORUS_ANIMATE_H
#define PHOSPHORUS_INCLUDE_PHOSPHORUS_ANIMATE_H

#include "phosphorus/Coordinate.h"
#include <boost/asio.hpp>
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

private:
  void setup();
  void generateKeyframes(std::span<Cartesian2D>) const;
  void mergeKeyframes(int n) const;
  void cleanup();

  static auto getUniqueId() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis{};
    return std::to_string(dis(gen));
  }

  double delay_ = 0.1; // Default delay between frames
  double time_ = 60; // Default total time for the animation
  std::string name_ = "Animate";
  boost::asio::thread_pool pool_{4};
  std::string current_temp_;
};

} // namespace phosphorus

#endif // PHOSPHORUS_INCLUDE_PHOSPHORUS_ANIMATE_H
