//
// Created by Renatus Madrigal on 5/29/2025.
//

#include "phosphorus/phosphorus.h"
#include <format>
#include <iostream>
#include <range/v3/all.hpp>

#define SCI_CONST static constexpr double

using namespace phosphorus;
using namespace ranges;
using std::cout;
using std::format;
using std::string;
using std::vector;

struct Config {
  struct ParticleWithPosition {
    CommonParticle particle;
    Cartesian2D position;
    Cartesian2D::Vector velocity;
  };
  vector<ParticleWithPosition> particles;
  double step;
  size_t n;
};

Config config1 = {
    .particles =
        {
            Config::ParticleWithPosition{
                .particle = {1.989e30, 0},
                .position = {-1.496e11, 0},
                .velocity = {0, -29788.0},
            },
            Config::ParticleWithPosition{
                .particle = {1.989e30, 0},
                .position = {1.496e11, 0},
                .velocity = {0, 0},
            },
            Config::ParticleWithPosition{
                .particle = {1.989e20, 0},
                .position = {0, 0},
                .velocity = {0, 0},
            },
        },
    .step = 86400,
    .n = 1000,
};

Config config2 = {
    .particles =
        {
            Config::ParticleWithPosition{
                .particle = {5.972e30, 0},
                .position = {-1.496e11, 0},
                .velocity = {0, -29785.6783137522},
            },
            Config::ParticleWithPosition{
                .particle = {5.972e30, 0},
                .position = {1.496e11, 0},
                .velocity = {0, 29785.6783137522},
            },
            Config::ParticleWithPosition{
                .particle = {5.972e20, 0},
                .position = {0, 1.496e11},
                .velocity = {0, 0},
            },
        },
    .step = 86400,
    .n = 730,
};

int main() {
  using IntegratorType = GravityIntegrator<Cartesian2D, CommonParticle>;
  using Iter = IntegratorType::iterator;
  GravityIntegrator<Cartesian2D, CommonParticle> system;

  const Config &config = config1; // Change to config2 for the second test
  auto step = config.step;
  auto n = config.n;

  vector<Iter> part_its;
  for (auto &&particle : config.particles) {
    Iter it = system.pushParticle(particle.particle, particle.position,
                                  particle.velocity);
    part_its.push_back(it);
  }

  vector<vector<Cartesian2D>> result(config.particles.size());

  for (auto i = 0; i < n; ++i) {
    for (auto &&[it, result_it] : views::zip(part_its, result)) {
      result_it.push_back(it->position);
    }
    system.step(step);
  }

  AnimateGenerator animator("ThreeBodySystem", 4);
  for (auto &&result_it : result) {
    animator.pushPoints(result_it);
  }
  try {
    animator.generate("ThreeBodySystem", 60);
  } catch (const std::exception &e) {
    std::cerr << "Error generating animation: " << e.what() << '\n';
  }

  Gnuplot plot;
  plot.setFigureConfig({
      .xlabel = "X (m)",
      .ylabel = "Y (m)",
      .grid = true,
  });

  vector<vector<double>> x_data(config.particles.size());
  vector<vector<double>> y_data(config.particles.size());

  for (auto &&[i, result_it] : views::enumerate(result)) {
    x_data[i] =
        result_it |
        views::transform([](const auto &p) { return p[0] / Constants::AU; }) |
        to<std::vector<double>>();
    y_data[i] =
        result_it |
        views::transform([](const auto &p) { return p[1] / Constants::AU; }) |
        to<std::vector<double>>();

    plot.plot({
        .x = x_data[i],
        .y = y_data[i],
        .with = Gnuplot::PlotConfig::PlotType::Lines,
        .title = format("Particle {} (AU)", i + 1),
    });
  }

  plot.savefig("Result");

  return 0;
}
