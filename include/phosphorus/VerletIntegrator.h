//
// Created by Renatus Madrigal on 5/23/2025.
//

#ifndef PHOSPHORUS_INCLUDE_PHOSPHORUS_VERLETINTEGRATOR_H
#define PHOSPHORUS_INCLUDE_PHOSPHORUS_VERLETINTEGRATOR_H

#include "phosphorus/Coordinate.h"
#include "phosphorus/Particle.h"
#include <cmath>
#include <iterator>
#include <vector>

namespace phosphorus {

// TODO: Currently the Verlet integrator only supports same particle type
/**
 * @brief Base Verlet integrator for particle simulation.
 * @tparam Impl The implementation type.
 * @tparam Coord The coordinate system used for the simulation.
 >* @tparam ParticleType The type of the particle to be integrated.
 */
template <typename Impl, typename Coord, typename ParticleType>
  requires IsCoordinateVec<Coord> && Massive<ParticleType>
class BaseVerletIntegrator {
  struct Element {
    ParticleType particle;
    Coord position;
    typename Coord::Vector velocity;
    typename Coord::Vector acceleration;
  };

public:
  using TimeType = double;
  using CoordinateVec = Coord;
  using Vector = typename CoordinateVec::Vector;

  using iterator = typename std::vector<Element>::iterator;

  // TODO: Maybe we should implement a iterator for particles
  /**
   * @brief Push a particle into the integrator.
   * @param particle The particle to be added.
   * @param position The initial position of the particle.
   * @param velocity The initial velocity of the particle.
   * @return The iterator to the added particle.
   */
  auto pushParticle(const ParticleType &particle,
                    const CoordinateVec &position = CoordinateVec(),
                    const Vector &velocity = Vector()) {
    elements_.emplace_back(particle, position, velocity, Vector());
    auto it = std::prev(elements_.end());
    elements_.back().acceleration = this->calculateAcceleration(it);
    return it;
  }

  // TODO: First step should be handled separately

  void step(TimeType dt) {
    auto n = this->count();

    // Use static storage to avoid dynamic allocation
    static std::vector<Vector> acc(n);

    // Resize the static storage if needed
    // Because we do not support removing particles, we can assume that
    // the size of the static storage is always greater than or equal to n.
    if (acc.size() < n) {
      acc.resize(n);
    }

    // Calculate acceleration
    auto it = acc.begin();
    for (auto elem_it = elements_.begin(); elem_it != elements_.end();
         ++elem_it, ++it) {
      *it = this->calculateAcceleration(elem_it);
    }

    // Update positions and velocities
    // TODO: Use std::views::zip when C++23 is available
    it = acc.begin();
    for (auto &elem : elements_) {
      elem.position += elem.velocity * dt + 0.5 * elem.acceleration * dt * dt;
      elem.velocity += 0.5 * (elem.acceleration + *it) * dt;
      elem.acceleration = *it;
      ++it;
    }
  }

  auto count() const { return elements_.size(); }
  auto size() const { return elements_.size(); }

  // Iterators for the elements
  auto begin() { return elements_.begin(); }
  auto end() { return elements_.end(); }
  auto begin() const { return elements_.begin(); }
  auto end() const { return elements_.end(); }
  auto front() { return elements_.front(); }
  auto back() { return elements_.back(); }
  auto front() const { return elements_.front(); }
  auto back() const { return elements_.back(); }

  auto data() const { return elements_.data(); }

protected:
  Vector calculateAcceleration(iterator it) const {
    return static_cast<const Impl *>(this)->calculateAccelerationImpl(it);
  }

  std::vector<Element> elements_;
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
  using Base = BaseVerletIntegrator<FieldVerletIntegrator, Coord, ParticleType>;
  friend Base;

public:
  using Base::Base;
  using TimeType = typename Base::TimeType;
  using CoordinateVec = typename Base::CoordinateVec;
  using CartesianVector = typename CoordinateVec::CartesianVector;
  using Vector = typename Base::Vector;
  using iterator = typename Base::iterator;

  FieldVerletIntegrator() = default;
  FieldVerletIntegrator(const FieldVerletIntegrator &) = default;
  FieldVerletIntegrator(FieldVerletIntegrator &&) = default;
  FieldVerletIntegrator &operator=(const FieldVerletIntegrator &) = default;
  FieldVerletIntegrator &operator=(FieldVerletIntegrator &&) = default;

  explicit FieldVerletIntegrator(const Field &force_field)
      : force_field_(force_field) {}

private:
  Vector calculateAccelerationImpl(iterator it) const {
    return force_field_.evaluate(it->position, it->particle) /
           it->particle.mass();
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
  using Base = BaseVerletIntegrator<GravityIntegrator, Coord, ParticleType>;
  friend Base;

public:
  using Base::Base;
  using TimeType = typename Base::TimeType;
  using CoordinateVec = typename Base::CoordinateVec;
  using CartesianVector = typename CoordinateVec::CartesianVector;
  using Vector = typename Base::Vector;
  using iterator = typename Base::iterator;

  static constexpr double kGravityConstant = 6.67430e-11; // m^3 kg^-1 s^-2

private:
  [[nodiscard]] Vector calculateAccelerationImpl(iterator center_it) const {
    // All evaluations are done in Cartesian coordinates to avoid complexity
    auto center = center_it->position.toCartesian();
    auto acc = CartesianVector{};

    for (auto it = this->elements_.begin(); it != this->elements_.end(); ++it) {
      if (it == center_it)
        continue; // Skip self

      auto other_pos = it->position.toCartesian();
      auto distance_vector = other_pos - center;
      auto distance_squared = distance_vector * distance_vector;
      auto normalized_distance = distance_vector / std::sqrt(distance_squared);

      if (distance_squared > 0) {
        // Calculate gravitational force
        auto force_magnitude =
            kGravityConstant * it->particle.mass() / distance_squared;
        acc += normalized_distance * force_magnitude;
      }
    }

    return acc;
  }
};

} // namespace phosphorus

#endif // PHOSPHORUS_INCLUDE_PHOSPHORUS_VERLETINTEGRATOR_H
