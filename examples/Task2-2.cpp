//
// Created by Renatus Madrigal on 5/29/2025.
//

#include "phosphorus/phosphorus.h"
#include <chrono>
#include <iostream>
#include <range/v3/all.hpp>

#define SCI_CONST static constexpr double

using namespace phosphorus;
using namespace ranges;
using std::cout;
using std::endl;
using std::format;
using std::vector;
namespace chrono = std::chrono;

int main() {
  // We use all SI units for consistency
  SCI_CONST M = 1.989e30;    // Mass of the Sun in kg
  SCI_CONST AU = 1.496e11;   // Astronomical unit in meters
  SCI_CONST G = 6.67430e-11; // Gravitational constant in m^3 kg^-1 s^-2
  SCI_CONST v0 = 29780;      // Initial velocity in m/s
  SCI_CONST DAY = 86400;     // Seconds in a day

  // The gravity field is implemented in Field.h
  Cartesian2DGravityField field({0, 0}, M);
  Cartesian2D initial_position{AU, 0};
  Cartesian2D::Vector initial_velocity{0, v0};

  // We can ignore the mass of Earth for this simulation
  CommonParticle earth(1.0, 0);

  using IntegratorType =
      FieldVerletIntegrator<Cartesian2DGravityField, // The field type
                            Cartesian2D,             // The coordinate system
                            CommonParticle>;         // The particle type

  IntegratorType system(field);

  auto earth_it =
      system.pushParticle(earth, initial_position, initial_velocity);

  auto step = DAY / 4; // 1/4 of a day per step
  auto max_n = 100000; // Maximum number of steps to avoid overflow
  auto n = 0;

  vector<Cartesian2D> result;
  vector<double> energy;

  auto start = chrono::high_resolution_clock::now();

  static constexpr double EPS = 1e-3; // Tolerance for convergence
  auto calc_deviation = [](const Cartesian2D &pos, const Cartesian2D &initial) {
    return std::abs((pos.toCartesian() - initial.toCartesian()).norm() /
                    initial.toCartesian().norm());
  };

  do {
    result.push_back(earth_it->position);
    auto potential_energy = -G * M * earth_it->particle.mass() /
                            earth_it->position.toCartesian().norm();
    auto kinetic_energy = 0.5 * earth_it->particle.mass() * earth_it->velocity *
                          earth_it->velocity;
    energy.emplace_back(potential_energy + kinetic_energy);
    system.step(step);
    ++n;
  } while (calc_deviation(earth_it->position, initial_position) > EPS &&
           n < max_n);

  std::cout << format("Converged after {} steps\n", n);

  auto end = chrono::high_resolution_clock::now();
  auto duration =
      chrono::duration_cast<chrono::milliseconds>(end - start).count();

  cout << format("Simulation completed in {} ms\n", duration);

  // AnimateGenerator animator("Earth", 4);
  // animator.generate("Earth orbit", result, 60);
  // cout << "Animation generated: Earth orbit\n";

  vector<double> x = result |
                     views::transform([](const auto &p) { return p[0]; }) |
                     to<std::vector<double>>();
  vector<double> y = result |
                     views::transform([](const auto &p) { return p[1]; }) |
                     to<std::vector<double>>();

  vector<double> times = views::iota(0, n) |
                         views::transform([step](int i) { return i * step; }) |
                         to<std::vector<double>>();

  Gnuplot plotter;
  plotter
      .setFigureConfig({
          .xrange = {-1.5 * AU, 1.5 * AU},
          .yrange = {-1.5 * AU, 1.5 * AU},
          .xlabel = "X (m)",
          .ylabel = "Y (m)",
          .grid = true,
      })
      .plot({
          .x = x,
          .y = y,
          .with = Gnuplot::PlotConfig::PlotType::Lines,
          .style = "lt 1 lw 2 notitle",
      })
      .savefig("Earth");

  // Calculate the energy deviation
  auto mean_energy = ranges::accumulate(energy, 0.0) / energy.size();
  auto energy_deviation =
      energy | views::transform([mean_energy](double e) {
        return (e - mean_energy) * 100 / mean_energy; // percent
      }) |
      to<std::vector<double>>();

  Gnuplot plotter2;
  plotter2.setFigureConfig({.grid = true})
      .plot({
          .x = times,
          .y = energy_deviation,
          .with = Gnuplot::PlotConfig::PlotType::Lines,
          .title = "Energy Deviation (%)",
          .style = "lt 2 lw 2",
      })
      .savefig("Energy");

  auto max_abs_deviation =
      ranges::max(energy_deviation |
                  views::transform([](double e) { return std::abs(e); }));

  cout << format("Maximum absolute energy deviation: {:.6f}%\n",
                 max_abs_deviation);

  return 0;
}
