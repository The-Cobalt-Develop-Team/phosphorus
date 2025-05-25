//
// Created by Renatus Madrigal on 5/25/2025.
//

#include "phosphorus/phosphorus.h"
#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#include <print>

using namespace phosphorus;
using std::cout;
using std::ofstream;
using std::println;

int main(int argc, char *argv[]) {
  static constexpr auto k = 1.0;
  auto force = [](Cartesian3D pos, CommonParticle part) {
    return Cartesian3D::Vector{-pos[0] * k * part.mass(), 0, 0};
  };
  auto field = LambdaField(force);
  auto system = FieldVerletIntegrator(field);

  auto m = 1.0;
  auto omega = std::sqrt(k / m);
  auto v0 = 1.0;
  auto A = v0 * std::sqrt(m / k);

  println(cout, "k = {}, m = {}, omega = {}, v0 = {}, A = {}", k, m, omega, v0,
          A);
  println(cout, "Vibration equation: x(t) = {} * sin({} * t)", A, omega);

  // The path of the particle should be sin wave
  auto initial_position = Cartesian3D{0, 0, 0};
  auto initial_velocity = Vector{v0, 0, 0};
  auto particle = CommonParticle{m, 0};

  auto it = system.pushParticle(particle, initial_position, initial_velocity);

  auto expect_position = [&](double time) {
    return Cartesian3D{A * std::sin(omega * time), 0, 0};
  };

  auto step = 0.0001;
  if (argc > 1) {
    step = std::stod(argv[1]);
  }
  println(cout, "step = {}", step);

  ofstream file("output.txt");

  auto start = std::chrono::high_resolution_clock::now();
  constexpr double pi = 3.14159265358979323846;
  auto n = static_cast<size_t>(2 * pi / (omega * step));
  for (auto i = 0; i <= n; ++i) {
    println(file, "{:.5f} {:.9f} {:.9f}", i * step, it->position[0],
            expect_position(i * step)[0]);
    system.step(step);
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  cout << "The simulation is done. The result is saved in output.txt\n";
  cout << "The simulation took " << duration.count() << " milliseconds.\n";

  return 0;
}
