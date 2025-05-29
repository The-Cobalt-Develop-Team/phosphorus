//
// Created by Renatus Madrigal on 5/26/2025.
//

#include "phosphorus/Gnuplot.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <range/v3/range.hpp>
#include <range/v3/view.hpp>

using namespace phosphorus;
using namespace ranges;
using std::cout;
using std::endl;
using std::ios;
using std::ofstream;
using std::vector;

int main() {
  static constexpr double pi = 3.14159265358979323846;
  int n = 100; // Number of points
  vector<double> x = views::iota(0, n) |
                     views::transform([n](int i) { return 2 * pi * i / n; }) |
                     ranges::to<vector<double>>();
  vector<double> y = x | views::transform([n](double i) { return sin(i); }) |
                     ranges::to<vector<double>>();
  Gnuplot gnuplot;
  gnuplot.setFigureConfig({.xlabel = "x", .ylabel = "sin(x)"})
      .plot({.x = x, .y = y, .with = Gnuplot::PlotConfig::PlotType::Lines})
      .show();
  return 0;
}
