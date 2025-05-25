//
// Created by Renatus Madrigal on 5/25/2025.
//

#ifndef PHOSPHORUS_TEST_TESTHELPER_H
#define PHOSPHORUS_TEST_TESTHELPER_H

#include "phosphorus/phosphorus.h"
#include <gtest/gtest.h>

// A test helper for vector comparing
template <typename Container>
::testing::AssertionResult
ContainersNear(const char *expr1, const char *expr2, const char *abs_error_expr,
               const Container &c1, const Container &c2,
               typename Container::value_type abs_error) {
  using ValueType = typename Container::value_type;
  static_assert(std::is_floating_point_v<ValueType>,
                "ContainersNear requires floating-point element types.");

  if (c1.size() != c2.size()) {
    return ::testing::AssertionFailure()
           << "Size mismatch: " << expr1 << " (size " << c1.size() << ") vs "
           << expr2 << " (size " << c2.size() << ")";
  }

  for (size_t i = 0; i < c1.size(); ++i) {
    auto diff = std::abs(c1[i] - c2[i]);
    if (diff > abs_error) {
      return ::testing::AssertionFailure()
             << "Containers differ at index " << i << ":\n"
             << expr1 << "[" << i << "] = " << c1[i] << "\n"
             << expr2 << "[" << i << "] = " << c2[i] << "\n"
             << "Difference = " << diff << " > " << abs_error_expr << " ("
             << abs_error << ")";
    }
  }
  return ::testing::AssertionSuccess();
}

#define EXPECT_VEC_NEAR(c1, c2, abs_error)                                     \
  EXPECT_PRED_FORMAT3(ContainersNear<decltype(c1)>, c1, c2, abs_error)
#define ASSERT_VEC_NEAR(c1, c2, abs_error)                                     \
  ASSERT_PRED_FORMAT3(ContainersNear<decltype(c1)>, c1, c2, abs_error)

#endif // PHOSPHORUS_TEST_TESTHELPER_H
