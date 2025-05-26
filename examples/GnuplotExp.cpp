//
// Created by Renatus Madrigal on 5/26/2025.
//

#include "phosphorus/Gnuplot.h"
#include <iostream>

using namespace phosphorus;

int main() {
  Gnuplot plot;
  try {
    plot.execute("plot sin(x)");
    plot.execute("plot cos(x)");
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
