//
// Created by Renatus Madrigal on 5/26/2025.
//

#include "phosphorus/Gnuplot.h"
#include <iostream>

using namespace phosphorus;
using namespace std;

int main() {
  try {
    Gnuplot plot;
    plot.execute("plot sin(x)");
    int x;
    cin >> x; // Wait for user input to keep the plot open
    plot.execute("plot cos(x)");
    cin >> x; // Wait for user input to keep the plot open
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
