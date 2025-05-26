//
// Created by Renatus Madrigal on 5/22/2025.
//

#ifndef PHOSPHORUS_INCLUDE_PHOSPHORUS_FIELD_H
#define PHOSPHORUS_INCLUDE_PHOSPHORUS_FIELD_H

#include "phosphorus/Coordinate.h"
#include "phosphorus/Particle.h"
#include "phosphorus/SignalSlot.h"
#include "phosphorus/TypeTraits.h"
#include <concepts>
#include <functional>
#include <type_traits>

namespace phosphorus {

template <typename Field>
concept IsField = requires {
  typename Field::CoordinateVec;
  typename Field::Vector;
  // Currently the force method cannot be checked because it is a
  // template method with constraints.
};

/**
 * @brief The template base class for a force field.
 * @tparam Impl The implementation of the force field.
 */
template <typename Impl, typename Coord = typename Impl::CoordinateVecType>
class BaseField {
public:
  using CoordinateVec = Coord;
  using Vector = typename CoordinateVec::Vector;

  template <typename ParticleType>
  Vector evaluate(const CoordinateVec &pos,
                  const ParticleType &particle) const {
    return static_cast<const Impl *>(this)->evaluate(pos, particle);
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
  using Particle = ParticleType;
  using CoordinateVecType = Coord;
  using Vector = typename CoordinateVecType::Vector;

  template <typename Func>
    requires ForceFunction<Func, CoordinateVecType, ParticleType>
  explicit LambdaField(Func &&force) : field_func_(std::forward<Func>(force)) {}

  Vector evaluate(const CoordinateVecType &coord,
                  const ParticleType &particle) const {
    return field_func_(coord, particle);
  }

private:
  std::function<Vector(const CoordinateVecType &, const ParticleType &)>
      field_func_;
};

template <typename Func>
LambdaField(Func) -> LambdaField<
    std::remove_cv_t<typename function_traits<Func>::first_argument_type>,
    std::remove_cv_t<typename function_traits<Func>::second_argument_type>>;

// TODO: We may need a more general composite field that can be modified.
// Currently, the type of the field is fixed, and we cannot change it.
// Thus, we must redefine the field type every time we want to change it.
// This leads to a lot of memory allocation and deallocation.
/**
 * @brief A composite field that is the sum of two fields.
 * @tparam LHS The LHS Field
 * @tparam RHS The RHS Field
 */
template <typename LHS, typename RHS>
  requires std::same_as<typename LHS::CoordinateVec,
                        typename RHS::CoordinateVec>
class CompositeField
    : public BaseField<CompositeField<LHS, RHS>, typename LHS::CoordinateVec> {
public:
  using CoordinateVecType = typename LHS::CoordinateVec;
  using Vector = typename CoordinateVecType::Vector;

  CompositeField(const LHS &lhs, const RHS &rhs) : lhs_(lhs), rhs_(rhs) {}

  template <typename ParticleType>
  Vector evaluate(const CoordinateVecType &coord,
                  const ParticleType &particle) const {
    return lhs_.evaluate(coord, particle) + rhs_.evaluate(coord, particle);
  }

private:
  const LHS &lhs_;
  const RHS &rhs_;
};

template <typename Field>
class NegativeField
    : public BaseField<NegativeField<Field>, typename Field::CoordinateVec> {
public:
  using CoordinateVecType = typename Field::CoordinateVec;
  using Vector = typename CoordinateVecType::Vector;

  explicit NegativeField(const Field &field) : field_(field) {}

  template <typename ParticleType>
  Vector evaluate(const CoordinateVecType &coord,
                  const ParticleType &particle) const {
    return -field_.evaluate(coord, particle);
  }

private:
  const Field &field_;
};

template <typename LHS, typename RHS>
  requires IsField<LHS> && IsField<RHS>
auto operator+(const LHS &lhs, const RHS &rhs) -> CompositeField<LHS, RHS> {
  return CompositeField<LHS, RHS>(lhs, rhs);
}

template <typename Operand>
  requires IsField<Operand>
auto operator-(const Operand &operand) -> NegativeField<Operand> {
  return NegativeField<Operand>(operand);
}

template <typename LHS, typename RHS>
  requires IsField<LHS> && IsField<RHS>
auto operator-(const LHS &lhs, const RHS &rhs)
    -> CompositeField<LHS, NegativeField<RHS>> {
  return lhs + (-rhs);
}

/**
 * @brief A gravity field in Cartesian coordinates.
 */
class CartesianGravityField : public BaseField<CartesianGravityField, Cartesian3D> {
public:
  using CoordinateVecType = Cartesian3D;
  using Vector = CoordinateVecType::Vector;
  using Scalar = CoordinateVecType::Scalar;

  CartesianGravityField() = default;
  CartesianGravityField(const Cartesian3D &center, Scalar mass)
      : center_(center), mass_(mass) {}

  template <typename ParticleType>
    requires Massive<ParticleType>
  Vector evaluate(const CoordinateVecType &coord,
                  const ParticleType &particle) const {
    auto r = coord.toCartesian() - center_.toCartesian();
    auto distance = r.norm();
    auto elem_r = r / distance;
    auto force = -mass_ * particle.mass() / (distance * distance);
    return force * elem_r;
  }

private:
  Cartesian3D center_ = {0, 0, 0};
  Scalar mass_ = 1.0;
};

static_assert(IsField<CartesianGravityField>,
              "CartesianGravityField is not a field");

} // namespace phosphorus

#endif // PHOSPHORUS_INCLUDE_PHOSPHORUS_FIELD_H
