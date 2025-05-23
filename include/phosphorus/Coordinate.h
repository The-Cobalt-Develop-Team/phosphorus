//
// Created by Renatus Madrigal on 5/20/2025.
//

#ifndef PHOSPHORUS_INCLUDE_PHOSPHORUS_COORDINATE_H
#define PHOSPHORUS_INCLUDE_PHOSPHORUS_COORDINATE_H

#include "phosphorus/Vector.h"
#include <concepts>
#include <type_traits>

namespace phosphorus {

// FIXME: Currently the Coordinate and Vector are mixed up.
// We should set a clear separation between them.

template <typename Coord>
concept IsCoordinate = requires(Coord c) {
  typename Coord::Scalar;
  typename Coord::CartesianVector;
  typename Coord::Vector;
  { c.toCartesian() } -> std::convertible_to<typename Coord::CartesianVector>;
  {
    Coord::fromCartesian(std::declval<typename Coord::CartesianVector>())
  } -> std::convertible_to<Coord>;
};

/**
 * Coordinate is a template base class for representing a coordinate system.
 * We use CRTP to allow derived classes to implement specific coordinate
 * systems, so that we can implement a static polymorphic interface.
 * @tparam Impl The implementation type.
 * @tparam kDimension The number of dimensions in the coordinate system.
 */
template <typename Impl, size_t kDimension> class BaseCoordinate {
public:
  using Scalar = double;
  using CartesianVector = EuclidVector<kDimension, Scalar>;
  using Vector = EuclidVector<kDimension, Scalar>;

  BaseCoordinate() = default;
  BaseCoordinate(const BaseCoordinate &) = default;
  BaseCoordinate(BaseCoordinate &&) = default;
  BaseCoordinate &operator=(const BaseCoordinate &) = default;
  BaseCoordinate &operator=(BaseCoordinate &&) = default;

  BaseCoordinate(std::initializer_list<Scalar> list) {
    static_cast<Impl &>(*this) = BaseCoordinate::fromCartesian(list);
  }

  [[nodiscard]] CartesianVector toCartesian() const {
    return static_cast<const Impl *>(this)->toCartesian();
  }

  static auto fromCartesian(const CartesianVector &cartesian) {
    return Impl::fromCartesian(cartesian);
  }

  static constexpr size_t dimension() { return kDimension; }
};

// TODO: Implement a template for Cartesian coordinates

/**
 * 2D Cartesian coordinate system.
 */
class Cartesian2D : public BaseCoordinate<Cartesian2D, 2> {
public:
  [[nodiscard]] CartesianVector toCartesian() const { return {x_, y_}; }

  static auto fromCartesian(const CartesianVector &cartesian) {
    Cartesian2D coord;
    coord.x_ = cartesian[0];
    coord.y_ = cartesian[1];
    return coord;
  }

private:
  Scalar x_ = 0, y_ = 0;
};

static_assert(IsCoordinate<Cartesian2D>, "Cartesian2D is not a coordinate");

/**
 * 3D Cartesian coordinate system.
 */
class Cartesian3D : public BaseCoordinate<Cartesian3D, 3> {
public:
  using BaseCoordinate::BaseCoordinate;

  [[nodiscard]] CartesianVector toCartesian() const { return {x_, y_, z_}; }

  static auto fromCartesian(const CartesianVector &cartesian) {
    Cartesian3D coord;
    coord.x_ = cartesian[0];
    coord.y_ = cartesian[1];
    coord.z_ = cartesian[2];
    return coord;
  }

  bool operator==(const Cartesian3D &rhs) const {
    return x_ == rhs.x_ && y_ == rhs.y_ && z_ == rhs.z_;
  }

  auto operator*(Scalar scalar) const {
    return Cartesian3D{x_ * scalar, y_ * scalar, z_ * scalar};
  }

  friend auto operator*(Scalar scalar, const Cartesian3D &coord) {
    return coord * scalar;
  }

private:
  Scalar x_ = 0, y_ = 0, z_ = 0;
};

static_assert(IsCoordinate<Cartesian3D>, "Cartesian3D is not a coordinate");

/**
 * Polar coordinate system.
 */
class Polar : public BaseCoordinate<Polar, 2> {};

/**
 * Spherical coordinate system.
 */
class Spherical : public BaseCoordinate<Spherical, 3> {};

/**
 * Minkowski coordinate system.
 * This is a 4D coordinate system used in special relativity.
 */
class Minkowski : public BaseCoordinate<Minkowski, 4> {};

} // namespace phosphorus

#endif // PHOSPHORUS_INCLUDE_PHOSPHORUS_COORDINATE_H
