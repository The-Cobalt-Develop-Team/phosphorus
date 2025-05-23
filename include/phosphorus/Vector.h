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
template <size_t kDimension, typename T = double> class EuclidVector {
public:
  EuclidVector() = default;
  EuclidVector(const EuclidVector &) = default;
  EuclidVector(EuclidVector &&) = default;
  EuclidVector(std::initializer_list<T> list) {
    assert(list.size() == kDimension);
    size_t i = 0;
    for (const auto &value : list) {
      components_[i++] = value;
    }
  }
  EuclidVector &operator=(const EuclidVector &) = default;
  EuclidVector &operator=(EuclidVector &&) = default;
  EuclidVector &operator=(std::initializer_list<T> list) {
    assert(list.size() == kDimension);
    size_t i = 0;
    for (const auto &value : list) {
      components_[i++] = value;
    }
    return *this;
  }

  bool operator==(const EuclidVector &rhs) const {
    for (size_t i = 0; i < kDimension; ++i) {
      if (components_[i] != rhs.components_[i]) {
        return false;
      }
    }
    return true;
  }

  EuclidVector &operator+=(const EuclidVector &rhs) {
    for (size_t i = 0; i < kDimension; ++i) {
      components_[i] += rhs.components_[i];
    }
    return *this;
  }

  EuclidVector &operator-=(const EuclidVector &rhs) {
    for (size_t i = 0; i < kDimension; ++i) {
      components_[i] -= rhs.components_[i];
    }
    return *this;
  }

  EuclidVector &operator*=(T scalar) {
    for (size_t i = 0; i < kDimension; ++i) {
      components_[i] *= scalar;
    }
    return *this;
  }

  EuclidVector operator-() const {
    EuclidVector result;
    for (size_t i = 0; i < kDimension; ++i) {
      result.components_[i] = -components_[i];
    }
    return result;
  }

  EuclidVector operator+(const EuclidVector &rhs) const {
    EuclidVector result = *this;
    result += rhs;
    return result;
  }

  EuclidVector operator-(const EuclidVector &rhs) const {
    EuclidVector result = *this;
    result -= rhs;
    return result;
  }

  EuclidVector operator*(T scalar) const {
    auto result = *this;
    result *= scalar;
    return result;
  }

  friend EuclidVector operator*(T scalar, const EuclidVector &rhs) {
    return rhs * scalar;
  }

  // TODO: Split the implementation of CartesianVector with common vector
  T operator*(const EuclidVector &rhs) const {
    // TODO: More general dot product
    // Currently we assume the dot product is under Euclidean geometry
    // This could be changed to a more general case
    T result = 0;
    for (size_t i = 0; i < kDimension; ++i) {
      result += components_[i] * rhs.components_[i];
    }
    return result;
  }

  T operator[](size_t index) const {
    assert(index < kDimension);
    return components_[index];
  }

  T &operator[](size_t index) {
    assert(index < kDimension);
    return components_[index];
  }

  friend std::ostream &operator<<(std::ostream &os, const EuclidVector &rhs) {
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

private:
  T components_[kDimension];
};

} // namespace phosphorus

#endif // PHOSPHORUS_INCLUDE_PHOSPHORUS_CARTESIANVECTOR_H
