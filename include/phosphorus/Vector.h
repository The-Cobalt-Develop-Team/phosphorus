//
// Created by Renatus Madrigal on 5/22/2025.
//

#ifndef PHOSPHORUS_INCLUDE_PHOSPHORUS_CARTESIANVECTOR_H
#define PHOSPHORUS_INCLUDE_PHOSPHORUS_CARTESIANVECTOR_H

#include <cassert>
#include <initializer_list>
#include <ostream>

namespace phosphorus {

/**
 * Vector class representing a Cartesian vector in a given dimension.
 * @tparam kDimension Dimension of the vector.
 * @tparam T The type of the vector components.
 */
template <size_t kDimension, typename T = double> class Vector {
public:
  using Scalar = T;
  using value_type = T;

  Vector() = default;
  Vector(const Vector &) = default;
  Vector(Vector &&) = default;
  Vector(std::initializer_list<Scalar> list) {
    assert(list.size() == kDimension);
    size_t i = 0;
    for (const auto &value : list) {
      components_[i++] = value;
    }
  }
  Vector &operator=(const Vector &) = default;
  Vector &operator=(Vector &&) = default;
  Vector &operator=(std::initializer_list<Scalar> list) {
    assert(list.size() == kDimension);
    size_t i = 0;
    for (const auto &value : list) {
      components_[i++] = value;
    }
    return *this;
  }

  bool operator==(const Vector &rhs) const {
    for (size_t i = 0; i < kDimension; ++i) {
      if (components_[i] != rhs.components_[i]) {
        return false;
      }
    }
    return true;
  }

  Vector &operator+=(const Vector &rhs) {
    for (size_t i = 0; i < kDimension; ++i) {
      components_[i] += rhs.components_[i];
    }
    return *this;
  }

  Vector &operator-=(const Vector &rhs) {
    for (size_t i = 0; i < kDimension; ++i) {
      components_[i] -= rhs.components_[i];
    }
    return *this;
  }

  Vector &operator*=(Scalar scalar) {
    for (size_t i = 0; i < kDimension; ++i) {
      components_[i] *= scalar;
    }
    return *this;
  }

  Vector operator-() const {
    Vector result;
    for (size_t i = 0; i < kDimension; ++i) {
      result.components_[i] = -components_[i];
    }
    return result;
  }

  Vector operator+(const Vector &rhs) const {
    Vector result = *this;
    result += rhs;
    return result;
  }

  Vector operator-(const Vector &rhs) const {
    Vector result = *this;
    result -= rhs;
    return result;
  }

  friend Vector operator*(const Vector &lhs, Scalar scalar) {
    Vector result = lhs;
    result *= scalar;
    return result;
  }

  friend Vector operator*(Scalar scalar, const Vector &rhs) {
    return rhs * scalar;
  }

  friend Vector operator/(const Vector &lhs, Scalar scalar) {
    return (1.0 / scalar) * lhs;
  }

  // TODO: This is just for convenience. We need to fix some issues with it.
  friend Scalar operator*(const Vector &lhs, const Vector &rhs) {
    Scalar result = 0;
    for (size_t i = 0; i < kDimension; ++i) {
      result += lhs.components_[i] * rhs.components_[i];
    }
    return result;
  }

  Scalar norm() const {
    return std::sqrt(*this * *this);
  }

  Scalar operator[](size_t index) const {
    assert(index < kDimension);
    return components_[index];
  }

  Scalar &operator[](size_t index) {
    assert(index < kDimension);
    return components_[index];
  }

  friend std::ostream &operator<<(std::ostream &os, const Vector &rhs) {
    os << "(";
    for (size_t i = 0; i < kDimension; ++i) {
      os << rhs.components_[i];
      if (i < kDimension - 1) {
        os << ", ";
      }
    }
    os << ")";
    return os;
  }

  [[nodiscard]] size_t size() const { return kDimension; }

protected:
  Scalar components_[kDimension];
};

template <typename... T>
Vector(T...)
    -> Vector<sizeof...(T),
              std::conditional_t<std::conjunction_v<std::is_same<T, int>...>,
                                 double, std::common_type_t<T...>>>;

template <size_t kDimension, typename T = double>
class EuclideanVector : public Vector<kDimension, T> {
public:
  using Vector<kDimension, T>::Vector;

  explicit EuclideanVector(const Vector<kDimension, T> &vector)
      : Vector<kDimension, T>(vector) {}

  using Scalar = T;

  friend EuclideanVector operator+(const EuclideanVector &lhs,
                                   const EuclideanVector &rhs) {
    EuclideanVector result = lhs;
    result += rhs;
    return result;
  }

  friend EuclideanVector operator-(const EuclideanVector &lhs,
                                   const EuclideanVector &rhs) {
    EuclideanVector result = lhs;
    result -= rhs;
    return result;
  }

  friend EuclideanVector operator*(const EuclideanVector &lhs, Scalar scalar) {
    EuclideanVector result = lhs;
    result *= scalar;
    return result;
  }

  friend EuclideanVector operator*(Scalar scalar, const EuclideanVector &rhs) {
    return rhs * scalar;
  }

  friend EuclideanVector operator/(const EuclideanVector &lhs, Scalar scalar) {
    return (1.0 / scalar) * lhs;
  }

  Scalar operator*(const EuclideanVector &rhs) const {
    // TODO: More general dot product
    Scalar result = 0;
    for (size_t i = 0; i < kDimension; ++i) {
      result += this->components_[i] * rhs.components_[i];
    }
    return result;
  }

  Scalar norm() const {
    Scalar result = 0;
    for (size_t i = 0; i < kDimension; ++i) {
      result += this->components_[i] * this->components_[i];
    }
    return std::sqrt(result);
  }
};

} // namespace phosphorus

#endif // PHOSPHORUS_INCLUDE_PHOSPHORUS_CARTESIANVECTOR_H
