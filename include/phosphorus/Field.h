//
// Created by Renatus Madrigal on 5/22/2025.
//

#ifndef PHOSPHORUS_INCLUDE_PHOSPHORUS_FIELD_H
#define PHOSPHORUS_INCLUDE_PHOSPHORUS_FIELD_H

#include "phosphorus/Coordinate.h"
#include "phosphorus/Particle.h"
#include "phosphorus/TypeTraits.h"
#include <concepts>
#include <functional>
#include <type_traits>

namespace phosphorus {

/**
 * @brief The template base class for a force field.
 * @tparam Impl The implementation of the force field.
 */
template <typename Impl, typename Coord = typename Impl::CoordinateType>
class BaseField {
public:
  using CoordinateVec = Coord;
  using Vector = typename CoordinateVec::Vector;

  template <typename ParticleType>
  Vector force(const CoordinateVec &pos, const ParticleType &particle) const {
    return static_cast<const Impl *>(this)->force(pos, particle);
  }
};

template <typename Func, typename CoordType, typename ParticleType>
concept ForceFunction = requires {
  std::invocable<Func, const CoordType &, const ParticleType &>;
  std::convertible_to<
      std::invoke_result_t<Func, const CoordType &, const ParticleType &>,
      typename CoordType::Vector>;
};

/**
 * @brief A force field that is defined by a lambda function.
 * @tparam Coord The coordinate system contains this field
 * @tparam ParticleType The type of the particle
 */
template <typename Coord, typename ParticleType>
  requires IsCoordinateVec<Coord>
class LambdaField : public BaseField<LambdaField<Coord, ParticleType>, Coord> {
public:
  using CoordinateVecType = Coord;
  using Vector = typename CoordinateVecType::Vector;

  template <typename Func>
    requires ForceFunction<Func, CoordinateVecType, ParticleType>
  explicit LambdaField(Func &&force) : force_(std::forward<Func>(force)) {}

  Vector force(const CoordinateVecType &coord,
               const ParticleType &particle) const {
    return force_(coord, particle);
  }

private:
  std::function<Vector(const CoordinateVecType &, const ParticleType &)> force_;
};

template <typename Func>
LambdaField(Func) -> LambdaField<
    std::remove_cv_t<typename function_traits<Func>::first_argument_type>,
    std::remove_cv_t<typename function_traits<Func>::second_argument_type>>;

} // namespace phosphorus

#endif // PHOSPHORUS_INCLUDE_PHOSPHORUS_FIELD_H
