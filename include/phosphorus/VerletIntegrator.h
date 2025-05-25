//
// Created by Renatus Madrigal on 5/23/2025.
//

#ifndef PHOSPHORUS_INCLUDE_PHOSPHORUS_VERLETINTEGRATOR_H
#define PHOSPHORUS_INCLUDE_PHOSPHORUS_VERLETINTEGRATOR_H

#include "phosphorus/Coordinate.h"
#include "phosphorus/Field.h"
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

  // The vector<>::iterator may be invalidated when the vector is resized.
  // So we need to use a custom iterator to avoid this problem.
  class iterator {
  private:
    using vector_type = std::vector<Element>;
    using vector_iter = typename vector_type::iterator;

  public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = Element;
    using difference_type = std::ptrdiff_t;
    using pointer = Element *;
    using reference = Element &;

    iterator(BaseVerletIntegrator *container, size_t index)
        : container_(container), index_(index) {}

    iterator(BaseVerletIntegrator *container, vector_iter iter)
        : container_(container),
          index_(std::distance(container->elements_.begin(), iter)) {}

    iterator(BaseVerletIntegrator *container, pointer ptr)
        : container_(container),
          index_(std::distance(container->elements_.data(), ptr)) {}

    reference operator*() const { return container_->elements_[index_]; }

    pointer operator->() const { return &(container_->elements_[index_]); }

    iterator &operator++() {
      index_++;
      return *this;
    }
    iterator operator++(int) {
      auto tmp = *this;
      ++(*this);
      return tmp;
    }
    iterator &operator--() {
      index_--;
      return *this;
    }
    iterator operator--(int) {
      auto tmp = *this;
      --(*this);
      return tmp;
    }
    iterator &operator+=(difference_type n) {
      index_ += n;
      return *this;
    }
    iterator &operator-=(difference_type n) {
      index_ -= n;
      return *this;
    }
    iterator operator+(difference_type n) const {
      return iterator(container_, index_ + n);
    }
    iterator operator-(difference_type n) const {
      return iterator(container_, index_ - n);
    }
    difference_type operator-(const iterator &other) const {
      return index_ - other.index_;
    }

    bool operator==(const iterator &other) const {
      return index_ == other.index_;
    }
    bool operator!=(const iterator &other) const { return !(*this == other); }
    bool operator<(const iterator &other) const {
      return index_ < other.index_;
    }

  private:
    BaseVerletIntegrator *container_;
    size_t index_;
  };

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
    return std::prev(this->end());
  }

  // TODO: First step should be handled separately

  void step(TimeType dt) {
    auto n = this->count();

    // Use static storage to avoid dynamic allocation
    static std::vector<Vector> acc(n);
    static bool first_step = true;

    // Resize the static storage if needed
    // Because we do not support removing particles, we can assume that
    // the size of the static storage is always greater than or equal to n.
    if (acc.size() < n) {
      acc.resize(n);
    }

    if (first_step) {
      for (auto &elem : elements_) {
        elem.acceleration = this->calculateAcceleration(&elem);
      }
    }

    // Update positions and velocities
    // TODO: Use std::views::zip when C++23 is available
    for (auto &elem : elements_) {
      elem.position += elem.velocity * dt + 0.5 * elem.acceleration * dt * dt;
      auto prev_acc = elem.acceleration;
      elem.acceleration = this->calculateAcceleration(&elem);
      elem.velocity += 0.5 * (prev_acc + elem.acceleration) * dt;
    }

    first_step = false;
  }

  auto count() const { return elements_.size(); }
  auto size() const { return elements_.size(); }

  // Access the elements
  auto operator[](size_t index) { return elements_[index]; }
  auto operator[](size_t index) const { return elements_[index]; }
  auto at(size_t index) { return elements_.at(index); }
  auto at(size_t index) const { return elements_.at(index); }
  auto front() { return elements_.front(); }
  auto back() { return elements_.back(); }
  auto front() const { return elements_.front(); }
  auto back() const { return elements_.back(); }

  // Iterators for the elements
  auto begin() { return iterator(this, elements_.begin()); }
  auto end() { return iterator(this, elements_.end()); }
  auto begin() const { return iterator(this, elements_.begin()); }
  auto end() const { return iterator(this, elements_.end()); }

  auto data() const { return elements_.data(); }

protected:
  Vector calculateAcceleration(iterator it) const {
    return static_cast<const Impl *>(this)->calculateAccelerationImpl(it);
  }

  Vector calculateAcceleration(Element *elem) const {
    return this->calculateAcceleration(
        iterator(const_cast<BaseVerletIntegrator *>(this), elem));
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

// A deducing guide for LambdaField for convenience
template <typename Coord, typename ParticleType>
FieldVerletIntegrator(LambdaField<Coord, ParticleType>)
    -> FieldVerletIntegrator<LambdaField<Coord, ParticleType>, Coord,
                             ParticleType>;

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
