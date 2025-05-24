//
// Created by Renatus Madrigal on 5/23/2025.
//

#ifndef PHOSPHORUS_INCLUDE_PHOSPHORUS_VERLETINTEGRATOR_H
#define PHOSPHORUS_INCLUDE_PHOSPHORUS_VERLETINTEGRATOR_H

#include "phosphorus/Particle.h"
#include <vector>

namespace phosphorus {

// TODO: Currently the Verlet integrator only supports same particle type
/**
 * @brief Base Verlet integrator for particle simulation.
 * @tparam Impl The implementation type.
 * @tparam Coord The coordinate system used for the simulation.
 * @tparam ParticleType The type of the particle to be integrated.
 */
template <typename Impl, typename Coord, typename ParticleType>
  requires IsCoordinateVec<Coord> && Massive<ParticleType>
class BaseVerletIntegrator {
public:
  using TimeType = double;
  using CoordinateVec = Coord;
  using Vector = typename CoordinateVec::Vector;

  // TODO: First step should be handled separately

  void step(TimeType dt) {
    auto n = particles_.size();

    // Use static storage to avoid dynamic allocation
    static std::vector<Vector> acceleration(n);

    // Calculate acceleration
    for (size_t i = 0; i < n; ++i) {
      acceleration[i] = calculateAcceleration(i);
    }

    // Update positions and velocities
    for (size_t i = 0; i < n; ++i) {
      positions_[i] += velocities_[i] * dt + 0.5 * accelerations_[i] * dt * dt;
      velocities_[i] += 0.5 * (accelerations_[i] + acceleration[i]) * dt;
      accelerations_[i] = acceleration[i];
    }
  }

  const auto &particles() const { return particles_; }
  const auto &positions() const { return positions_; }
  const auto &velocities() const { return velocities_; }
  const auto &accelerations() const { return accelerations_; }

protected:
  Vector calculateAcceleration(size_t idx) const {
    return static_cast<Impl *>(this)->calculateAccelerationImpl(idx);
  }

  std::vector<ParticleType> particles_;
  std::vector<CoordinateVec> positions_;
  std::vector<Vector> velocities_;
  std::vector<Vector> accelerations_;
};

/**
 * @brief Verlet integrator for particle simulation.
 * @tparam Field The field used for the simulation.
 * @tparam Coord The coordinate system used for the simulation.
 * @tparam ParticleType The type of the particle to be integrated.
 */
template <typename Field, typename Coord, typename ParticleType>
class FieldVerletIntegrator
    : public BaseVerletIntegrator<
          FieldVerletIntegrator<Field, Coord, ParticleType>, Coord,
          ParticleType> {
  friend BaseVerletIntegrator;
  using Base = BaseVerletIntegrator<FieldVerletIntegrator, Coord, ParticleType>;

public:
  using Base::Base;
  using TimeType = typename Base::TimeType;
  using CoordinateVec = typename Base::CoordinateVec;
  using CartesianVector = typename CoordinateVec::CartesianVector;
  using Vector = typename Base::Vector;

  FieldVerletIntegrator() = default;
  FieldVerletIntegrator(const FieldVerletIntegrator &) = default;
  FieldVerletIntegrator(FieldVerletIntegrator &&) = default;
  FieldVerletIntegrator &operator=(const FieldVerletIntegrator &) = default;
  FieldVerletIntegrator &operator=(FieldVerletIntegrator &&) = default;

  explicit FieldVerletIntegrator(const Field &force_field)
      : force_field_(force_field) {}

private:
  Vector calculateAccelerationImpl(size_t idx) const {
    return force_field_.evaluate(this->positions_[idx], this->particles_[idx]) /
           this->particles_[idx].mass();
  }

  Field force_field_;
};

/**
 * @brief Verlet integrator for particle simulation with gravity.
 * @tparam Coord The Coordinate system used for the simulation.
 * @tparam ParticleType The Particle type to be integrated.
 */
template <typename Coord, typename ParticleType>
class GravityIntegrator
    : public BaseVerletIntegrator<GravityIntegrator<Coord, ParticleType>, Coord,
                                  ParticleType> {
  friend BaseVerletIntegrator;
  using Base = BaseVerletIntegrator<GravityIntegrator, Coord, ParticleType>;

public:
  using Base::Base;
  using TimeType = typename Base::TimeType;
  using CoordinateVec = typename Base::CoordinateVec;
  using CartesianVector = typename CoordinateVec::CartesianVector;
  using Vector = typename Base::Vector;

  static constexpr double kGravityConstant = 6.67430e-11; // m^3 kg^-1 s^-2

private:
  [[nodiscard]] Vector calculateAccelerationImpl(size_t idx) const {
    // All evaluations are done in Cartesian coordinates to avoid complexity
    auto center = this->positions_[idx].toCartesian();
    auto acc = CartesianVector{};

    for (size_t i = 0; i < this->particles_.size(); ++i) {
      if (i == idx)
        continue; // Skip self

      auto other_pos = this->positions_[i].toCartesian();
      auto distance_vector = other_pos - center;
      auto distance_squared = distance_vector * distance_vector;
      auto normalized_distance = distance_vector / std::sqrt(distance_squared);

      if (distance_squared > 0) {
        // Calculate gravitational force
        auto force_magnitude =
            kGravityConstant * this->particles_[i].mass() / distance_squared;
        acc += normalized_distance * force_magnitude;
      }
    }

    return acc;
  }
};

} // namespace phosphorus

#endif // PHOSPHORUS_INCLUDE_PHOSPHORUS_VERLETINTEGRATOR_H
