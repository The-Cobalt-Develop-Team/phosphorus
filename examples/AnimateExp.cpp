//
// Created by Renatus Madrigal on 5/29/2025.
//

#include "phosphorus/phosphorus.h"
#include <iostream>
#include <range/v3/range.hpp>
#include <range/v3/view.hpp>
#include <vector>

using namespace phosphorus;
using namespace ranges;
using std::cout;
using std::endl;
using std::vector;

int main() {
  // Create a simple animation of a sine wave
  static constexpr double pi = 3.14159265358979323846;
  int n = 500; // Number of points
  vector<double> x = views::iota(0, n) |
                     views::transform([n](int i) { return 2 * pi * i / n; }) |
                     ranges::to<vector<double>>();
  vector<double> y = x | views::transform([](double i) { return sin(i); }) |
                     ranges::to<vector<double>>();

  vector<Cartesian2D> points =
      views::zip(x, y) |
      views::transform([](auto p) { return Cartesian2D{p.first, p.second}; }) |
      ranges::to<vector<Cartesian2D>>();

  AnimateGenerator animator("SineWave", 4);
  animator.generate("sine_wave_animation", points, 60);

  cout << "Animation generated successfully!" << endl;
  return 0;
}
