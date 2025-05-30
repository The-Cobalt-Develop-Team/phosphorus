//
// Created by Renatus Madrigal on 5/29/2025.
//

#include "phosphorus/phosphorus.h"
#include <chrono>
#include <format>
#include <iostream>
#include <range/v3/view.hpp>

using namespace phosphorus;
using namespace ranges;
using std::cout;
using std::endl;
using std::format;
using std::sin;
using std::stod;
using std::vector;
namespace chrono = std::chrono;

int main(int argc, char *argv[]) {
  static constexpr auto k = 1.0;
  auto force = [](Cartesian2D pos, CommonParticle part) {
    return Cartesian2D::Vector{-pos[0] * k * part.mass(), 0};
  };
  auto field = LambdaField(force);
  auto system = FieldVerletIntegrator(field);

  auto m = 1.0;
  auto omega = std::sqrt(k / m);
  auto v0 = 1.0;
  auto A = v0 * std::sqrt(m / k);

  cout << format("k = {}, m = {}, omega = {}, v0 = {}, A = {}\n", k, m, omega,
                 v0, A);
  cout << format("Vibration equation: x(t) = {} * sin({} * t)\n", A, omega);

  // The path of the particle should be sin wave
  auto initial_position = Cartesian2D{0, 0};
  auto initial_velocity = Vector{v0, 0};
  auto particle = CommonParticle{m, 0};

  auto it = system.pushParticle(particle, initial_position, initial_velocity);

  auto expect_position = [&](double time) {
    return Cartesian2D{A * sin(omega * time), 0};
  };

  auto step = 0.001;
  if (argc > 1) {
    step = stod(argv[1]);
  }
  cout << format("step = {}\n", step);

  constexpr double pi = 3.14159265358979323846;
  auto n = static_cast<size_t>(2 * pi / (omega * step));

  vector<double> time_points;
  vector<double> positions;
  vector<double> expected_positions;
  vector<Cartesian2D> points;

  auto start = chrono::high_resolution_clock::now();

  for (auto i = 0; i <= n; ++i) {
    time_points.emplace_back(i * step);
    positions.emplace_back(it->position[0]);
    points.emplace_back(it->position);
    expected_positions.emplace_back(expect_position(i * step)[0]);
    system.step(step);
  }

  auto end = chrono::high_resolution_clock::now();
  auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

  cout << "The simulation is done. The result is saved in output.txt\n";
  cout << "The simulation took " << duration.count() << " milliseconds.\n";

  // Only generate animation if step is significant
  if (step >= 0.005) {
    AnimateGenerator animator("SpringSystem", 4);
    animator.generate("SpringSystem", points, 30);
    cout << "Animation generated successfully!\n";
  }

  // verlet position - expected position
  auto error = views::zip(positions, expected_positions) |
               views::transform([](auto p) { return p.first - p.second; }) |
               ranges::to<vector<double>>();

  Gnuplot plotter;
  plotter
      .setFigureConfig({
          .xlabel = "time",
          .ylabel = "position",
          .grid = true,
      })
      .plot({
          .x = time_points,
          .y = error,
          .with = Gnuplot::PlotConfig::PlotType::Lines,
          .title = "Error",
          .style = "lt 3 lw 2",
      })
      .savefig("Error");

  cout << "Error plot saved to Error.png\n";

  return 0;
}
