//
// Created by Renatus Madrigal on 5/24/2025.
//

#include "phosphorus/VerletIntegrator.h"
#include "phosphorus/Field.h"
#include "phosphorus/Particle.h"
#include <gtest/gtest.h>

using namespace phosphorus;

class EmptySystem
    : public BaseVerletIntegrator<EmptySystem, Cartesian3D, CommonParticle> {
  using Base = BaseVerletIntegrator<EmptySystem, Cartesian3D, CommonParticle>;
  friend Base;

public:
  using Base::Base;

private:
  [[nodiscard]] Vector calculateAccelerationImpl(auto) const {
    return Vector{0, 0, 0};
  }
};

TEST(VerletIntegratorTest, Step) {
  EmptySystem system;
  auto initial_position = Cartesian3D{0, 0, 0};
  auto initial_velocity = Vector{1, 0, 0};
  auto it = system.pushParticle(CommonParticle{1.0, 1.0}, initial_position,
                                initial_velocity);

  EXPECT_EQ(it->position, initial_position);

  constexpr auto n = 10;

  for (auto i = 1; i <= n; ++i) {
    system.step(1.0);
    EXPECT_EQ(it->position, initial_position + initial_velocity * i);
  }
}