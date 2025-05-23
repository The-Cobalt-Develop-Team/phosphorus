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
 * The template base class for a force field.
 * @tparam Impl The implementation of the force field.
 */
template <typename Impl, typename Coord = typename Impl::CoordinateType>
class BaseField {
public:
  using Coordinate = Coord;
  using Vector = typename Coordinate::Vector;

  template <typename ParticleType>
  Vector force(const Coordinate &pos, const ParticleType &particle) const {
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

template <typename Coord, typename ParticleType>
  // requires IsCoordinate<Coord>
class LambdaField : public BaseField<LambdaField<Coord, ParticleType>, Coord> {
public:
  using CoordinateType = Coord;
  using Vector = typename CoordinateType::Vector;

  template <typename Func>
    requires ForceFunction<Func, CoordinateType, ParticleType>
  explicit LambdaField(Func &&force) : force_(std::forward<Func>(force)) {}

  CoordinateType force(const CoordinateType &coord,
               const ParticleType &particle) const {
    return force_(coord, particle);
  }

private:
  std::function<CoordinateType(const CoordinateType &, const ParticleType &)> force_;
};

template <typename Func>
LambdaField(Func) -> LambdaField<
    std::remove_cv_t<typename function_traits<Func>::first_argument_type>,
    std::remove_cv_t<typename function_traits<Func>::second_argument_type>>;

} // namespace phosphorus

#endif // PHOSPHORUS_INCLUDE_PHOSPHORUS_FIELD_H
