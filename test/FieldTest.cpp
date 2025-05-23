//
// Created by Renatus Madrigal on 5/23/2025.
//

#include "phosphorus/Field.h"
#include "phosphorus/Vector.h"
#include <gtest/gtest.h>

using namespace phosphorus;

TEST(FieldTest, LambdaField) {
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

TEST(FieldTest, FieldAdd) {
  auto force_function1 = [](const Cartesian3D &point,
                            const CommonParticle &particle) {
    return point.toCartesian() * particle.mass();
  };
  auto force_function2 = [](const Cartesian3D &point,
                            const CommonParticle &particle) {
    return point.toCartesian() * particle.charge();
  };
  auto field1 = LambdaField(force_function1);
  auto field2 = LambdaField(force_function2);

  auto composite_field = field1 + field2;

  CommonParticle particle{1.0, 2.0};
  Cartesian3D position{3.0, 4.0, 5.0};
  auto composite_force = composite_field.force(position, particle);
  auto expected_force =
      field1.force(position, particle) + field2.force(position, particle);
  EXPECT_EQ(composite_force, expected_force);

  auto negative_field = -field1;
  composite_force = negative_field.force(position, particle);
  expected_force = -field1.force(position, particle);
  EXPECT_EQ(composite_force, expected_force);
}
