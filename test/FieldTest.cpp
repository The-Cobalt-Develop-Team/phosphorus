//
// Created by Renatus Madrigal on 5/23/2025.
//

#include "phosphorus/Field.h"
#include "phosphorus/Vector.h"
#include <gtest/gtest.h>
#include <iostream>

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
  auto force = field.evaluate(coord, particle);

  // Check the result
  auto expected_force = Vector<3>{1.0, 2.0, 3.0} * particle.mass();
  EXPECT_EQ(force, expected_force);

  auto force_coefficient = 2.718;

  auto force_function_with_coefficient =
      [force_coefficient](const SimpleCoordinate &point,
                          const SimpleParticle &particle) {
        return force_coefficient * point.toCartesian() * particle.mass();
      };

  LambdaField field_with_coefficient(force_function_with_coefficient);

  auto force2 = field_with_coefficient.evaluate(coord, particle);
  auto expected_force2 =
      force_coefficient * coord.toCartesian() * particle.mass();
  EXPECT_EQ(force2, expected_force2);
}

TEST(FieldTest, FieldOperations) {
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
  auto composite_force = composite_field.evaluate(position, particle);
  auto expected_force =
      field1.evaluate(position, particle) + field2.evaluate(position, particle);
  EXPECT_EQ(composite_force, expected_force);

  auto negative_field = -field1;
  composite_force = negative_field.evaluate(position, particle);
  expected_force = -field1.evaluate(position, particle);
  EXPECT_EQ(composite_force, expected_force);
}

TEST(FieldTest, FieldOperationWithModification) {
  auto force_function = [](const Cartesian3D &point,
                           const CommonParticle &particle) {
    return point.toCartesian() * particle.mass();
  };
  auto field = LambdaField(force_function);

  struct ModifiableField : public BaseField<ModifiableField, Cartesian3D> {
    using CoordinateVecType = Cartesian3D;
    using Vector = typename CoordinateVecType::Vector;

    explicit ModifiableField(double scalar) : scalar_(scalar) {}

    void setScalar(double scalar) { scalar_ = scalar; }
    [[nodiscard]] double scalar() const { return scalar_; }

    [[nodiscard]] Vector evaluate(const CoordinateVecType &coord,
                                  const CommonParticle &particle) const {
      return coord.toCartesian() * particle.mass() * scalar_;
    }

  private:
    double scalar_ = 0;
  };

  auto modifiable_field = ModifiableField(2.0);
  auto composite_field = field + modifiable_field;

  CommonParticle particle{1.0, 2.0};
  Cartesian3D position{3.0, 4.0, 5.0};
  auto modified_force = composite_field.evaluate(position, particle);
  auto expected_force =
      position.toCartesian() * particle.mass() +
      position.toCartesian() * particle.mass() * modifiable_field.scalar();
  EXPECT_EQ(modified_force, expected_force);

  modifiable_field.setScalar(4.0);
  auto modified_force2 = composite_field.evaluate(position, particle);
  auto expected_force2 =
      position.toCartesian() * particle.mass() +
      position.toCartesian() * particle.mass() * modifiable_field.scalar();
  EXPECT_EQ(modified_force2, expected_force2);
  EXPECT_NE(modified_force, modified_force2);
}
