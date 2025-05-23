//
// Created by Renatus Madrigal on 5/20/2025.
//

#ifndef PHOSPHORUS_INCLUDE_PHOSPHORUS_PARTICLE_H
#define PHOSPHORUS_INCLUDE_PHOSPHORUS_PARTICLE_H

#include <concepts>
#include <type_traits>

namespace phosphorus {

/**
 * Massive is a concept that represents a particle type.
 * It must have a mass() method that returns a double.
 * The mass() method must return a double.
 */
template <typename ParticleType>
concept Massive = requires(ParticleType p) {
  { p.mass() } -> std::convertible_to<double>;
};

/**
 * Charged is a concept that represents a particle type.
 * It must have a charge() method that returns a double.
 * The charge() method must return a double.
 */
template <typename ParticleType>
concept Charged = requires(ParticleType p) {
  { p.charge() } -> std::convertible_to<double>;
};

class CommonParticle {
public:
  CommonParticle() = default;
  CommonParticle(const double mass, const double charge)
      : mass_(mass), charge_(charge) {}

  [[nodiscard]] double mass() const { return mass_; }
  [[nodiscard]] double charge() const { return charge_; }
  double &mass() { return mass_; }
  double &charge() { return charge_; }

  // The velocity should be implemented in the Coordinate class.

private:
  double mass_ = 0;
  double charge_ = 0;
};

} // namespace phosphorus

#endif // PHOSPHORUS_INCLUDE_PHOSPHORUS_PARTICLE_H
