//
// Created by Renatus Madrigal on 5/23/2025.
//

#include "phosphorus/Field.h"
#include "phosphorus/Vector.h"
#include <gtest/gtest.h>

TEST(FieldTest, LambdaField) {
  using namespace phosphorus;

  // Define a simple coordinate type
  using SimpleCoordinate = Cartesian3D;

  // Define a simple particle type
  struct SimpleParticle : public CommonParticle {};

  // Create a lambda function for the force
  auto force_function = [](const SimpleCoordinate &point,
                           const SimpleParticle &particle) {
    return point.toCartesian() * particle.mass();
  };

  // Create a LambdaField instance
  LambdaField field(force_function);

  // Create a coordinate and a particle
  SimpleCoordinate coord{};
  coord = {1.0, 2.0, 3.0};
  SimpleParticle particle;
  particle.mass() = 5.0;

  // Calculate the force
  auto force = field.force(coord, particle);

  // Check the result
  auto expected_force = coord.toCartesian() * particle.mass();
  EXPECT_EQ(force, expected_force);

  auto force_coefficient = 2.718;

  auto force_function_with_coefficient =
      [force_coefficient](const SimpleCoordinate &point,
                          const SimpleParticle &particle) {
        return force_coefficient * point.toCartesian() * particle.mass();
      };

  LambdaField field_with_coefficient(force_function_with_coefficient);

  auto force2 = field_with_coefficient.force(coord, particle);
  auto expected_force2 =
      force_coefficient * coord.toCartesian() * particle.mass();
  EXPECT_EQ(force2, expected_force2);
}
