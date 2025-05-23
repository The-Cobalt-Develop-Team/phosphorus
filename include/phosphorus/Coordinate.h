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
concept IsCoordinateVec = requires(Coord c) {
  typename Coord::Scalar;
  typename Coord::CartesianVector;
  typename Coord::Vector;
  { c.toCartesian() } -> std::convertible_to<typename Coord::CartesianVector>;
  {
    Coord::fromCartesian(std::declval<typename Coord::CartesianVector>())
  } -> std::convertible_to<Coord>;
};

template <typename Coord>
  requires IsCoordinateVec<Coord>
typename Coord::Scalar distance(const Coord &lhs, const Coord &rhs) {
  return (lhs.toCartesian() - rhs.toCartesian()).norm();
}

/**
 * @brief The base class for a coordinate system.
 * @details Coordinate is a template base class for representing a coordinate
 * system. We use CRTP to allow derived classes to implement specific coordinate
 * systems, so that we can implement a static polymorphic interface.
 * A CoordinateVec contains the position of a point in a given coordinate.
 *
 * @tparam Impl The implementation type.
 * @tparam kDimension The number of dimensions in the coordinate system.
 */
template <typename Impl, size_t kDimension> class BaseCoordinateVec {
public:
  using Scalar = double; ///< The type of the coordinate components.

  /**
   * @brief The type of the corresponding Cartesian vector.
   * @details This is the return type of the toCartesian() method. We define
   * this alias to emphasize that the Cartesian vector is a different type from
   * the vector type in the coordinate system. In detail, assume that in a 2D
   * linear space with a Cartesian coordinate system and a polar coordinate
   * system. \f$e_x\f$ and \f$e_y\f$ are the standard basis vectors of the
   * Cartesian coordinate system. \f$r\f$ and \f$\theta\f$ are the polar
   * coordinates. Then a point P can be represented as: \f$ P = x e_x + y e_y =
   * r \cos(\theta) e_x + r \sin(\theta)\f$. Then the Cartesian vector is
   * \f$(x, y)\f$.
   */
  using CartesianVector = Vector<kDimension, Scalar>;

  /**
   * @brief The type of vector in the coordinate system.
   * @details It is for the vector field in the coordinate system. For example,
   * in a 2D polar coordinate system, the vector field is represented as \f$
   * F(r, \theta) = F_r e_r + F_\theta e_\theta \f$. Then the vector is
   * \f$(F_r, F_\theta) \f$.
   */
  using Vector = Vector<kDimension, Scalar>;

  BaseCoordinateVec() = default;
  BaseCoordinateVec(const BaseCoordinateVec &) = default;
  BaseCoordinateVec(BaseCoordinateVec &&) = default;
  BaseCoordinateVec &operator=(const BaseCoordinateVec &) = default;
  BaseCoordinateVec &operator=(BaseCoordinateVec &&) = default;

  BaseCoordinateVec(std::initializer_list<Scalar> list) {
    static_cast<Impl &>(*this) = BaseCoordinateVec::fromCartesian(list);
  }

  /**
   * @brief Convert the coordinate to the corresponding Cartesian vector.
   * @return The vector in the corresponding Cartesian coordinate system.
   */
  [[nodiscard]] CartesianVector toCartesian() const {
    return static_cast<const Impl *>(this)->toCartesianImpl();
  }

  /**
   * @brief Convert the Cartesian vector to the coordinate.
   * @param cartesian The vector in corresponding Cartesian coordinate system
   * @return The coordinate in the coordinate system.
   */
  static auto fromCartesian(const CartesianVector &cartesian) {
    return Impl::fromCartesianImpl(cartesian);
  }

  static constexpr size_t dimension() { return kDimension; }
};

// TODO: Implement a template for Cartesian coordinates

/**
 * @brief 2D Cartesian coordinate system.
 */
class Cartesian2D : public BaseCoordinateVec<Cartesian2D, 2> {
public:
  friend BaseCoordinateVec;
  using BaseCoordinateVec::BaseCoordinateVec;

private:
  [[nodiscard]] CartesianVector toCartesianImpl() const { return {x_, y_}; }

  static auto fromCartesianImpl(const CartesianVector &cartesian) {
    Cartesian2D coord;
    coord.x_ = cartesian[0];
    coord.y_ = cartesian[1];
    return coord;
  }

  Scalar x_ = 0, y_ = 0;
};

static_assert(IsCoordinateVec<Cartesian2D>, "Cartesian2D is not a coordinate");

/**
 * @brief 3D Cartesian coordinate system.
 */
class Cartesian3D : public BaseCoordinateVec<Cartesian3D, 3> {
public:
  friend BaseCoordinateVec;
  using BaseCoordinateVec::BaseCoordinateVec;
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
  [[nodiscard]] CartesianVector toCartesianImpl() const { return {x_, y_, z_}; }

  static auto fromCartesianImpl(const CartesianVector &cartesian) {
    Cartesian3D coord;
    coord.x_ = cartesian[0];
    coord.y_ = cartesian[1];
    coord.z_ = cartesian[2];
    return coord;
  }

  Scalar x_ = 0, y_ = 0, z_ = 0;
};

static_assert(IsCoordinateVec<Cartesian3D>, "Cartesian3D is not a coordinate");

/**
 * Polar coordinate system.
 */
class Polar : public BaseCoordinateVec<Polar, 2> {};

/**
 * Spherical coordinate system.
 */
class Spherical : public BaseCoordinateVec<Spherical, 3> {};

/**
 * Minkowski coordinate system.
 * This is a 4D coordinate system used in special relativity.
 */
class Minkowski : public BaseCoordinateVec<Minkowski, 4> {};

} // namespace phosphorus

#endif // PHOSPHORUS_INCLUDE_PHOSPHORUS_COORDINATE_H
