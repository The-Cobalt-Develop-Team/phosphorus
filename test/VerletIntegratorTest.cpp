//
// Created by Renatus Madrigal on 5/24/2025.
//

#include "phosphorus/VerletIntegrator.h"
#include "TestHelper.h"
#include "phosphorus/Field.h"
#include "phosphorus/Particle.h"
#include <gtest/gtest.h>

using namespace phosphorus;

static constexpr auto eps = 1e-6;

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

TEST(VerletIntegratorTest, MultipleParticles) {
  EmptySystem system;
  auto initial_position = Cartesian3D{0, 0, 0};
  auto initial_velocity = Vector{1, 0, 0};
  auto it1 = system.pushParticle(CommonParticle{1.0, 1.0}, initial_position,
                                 initial_velocity);
  auto it2 = system.pushParticle(CommonParticle{2.0, 2.0}, initial_position,
                                 -initial_velocity);

  EXPECT_EQ(it1->position, initial_position);
  EXPECT_EQ(it2->position, initial_position);

  constexpr auto n = 10;

  for (auto i = 1; i <= n; ++i) {
    system.step(1.0);
    EXPECT_EQ(it1->position, initial_position + initial_velocity * i);
    EXPECT_EQ(it2->position, initial_position - initial_velocity * i);
  }
}

TEST(VerletIntegratorTest, ParticleInField) {
  static constexpr auto g = 9.8;
  auto force = [](Cartesian3D, CommonParticle particle) {
    return Cartesian3D::Vector{0, 0, particle.mass() * g};
  };
  auto gravity_field = LambdaField(force);
  auto system = FieldVerletIntegrator(gravity_field);

  auto initial_position = Cartesian3D{0, 0, 0};
  auto it = system.pushParticle(CommonParticle{1.0, 1.0}, initial_position,
                                Cartesian3D::Vector{0, 0, 0});
  auto acc = Cartesian3D::Vector{0, 0, g};

  EXPECT_VEC_NEAR(it->position, initial_position, eps);

  static constexpr auto n = 10;

  for (auto i = 1; i <= n; ++i) {
    system.step(1.0);
    EXPECT_VEC_NEAR(it->position, initial_position + 0.5 * acc * i * i, eps);
    EXPECT_VEC_NEAR(it->velocity, acc * i, eps);
  }
}

TEST(VerletIntegratorTest, NonUniformField) {
  static constexpr auto k = 1.0;
  auto force = [](Cartesian3D pos, CommonParticle part) {
    return Cartesian3D::Vector{-pos[0] * k * part.mass(), 0, 0};
  };
  auto field = LambdaField(force);
  auto system = FieldVerletIntegrator(field);

  auto m = 1.0;
  auto omega = std::sqrt(k / m);
  auto v0 = 1.0;
  auto A = v0 * std::sqrt(m / k);

  // The path of the particle should be sin wave
  auto initial_position = Cartesian3D{0, 0, 0};
  auto initial_velocity = Vector{v0, 0, 0};
  auto particle = CommonParticle{m, 0};

  auto it = system.pushParticle(particle, initial_position, initial_velocity);

  auto expect_position = [&](double time) {
    return Cartesian3D{A * std::sin(omega * time), 0, 0};
  };

  constexpr auto n = 1000;
  for (auto i = 0; i <= n; ++i) {
    EXPECT_VEC_NEAR(it->position, expect_position(i / 1000.0), eps)
        << "Where i == " << i;
    system.step(0.001);
  }
}
