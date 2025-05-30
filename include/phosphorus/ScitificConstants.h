//
// Created by Renatus Madrigal on 5/30/2025.
//

#ifndef PHOSPHORUS_INCLUDE_PHOSPHORUS_SCITIFICCONSTANTS_H
#define PHOSPHORUS_INCLUDE_PHOSPHORUS_SCITIFICCONSTANTS_H

namespace phosphorus {

namespace Constants {

// Some physical and astronomical constants

constexpr double G = 6.67430e-11; ///> Gravitational constant in m^3 kg^-1 s^-2
constexpr double M = 1.989e30;    ///> Mass of the Sun in kg
constexpr double AU = 1.496e11;   ///> Astronomical unit in meters
constexpr double DAY = 86400;     ///> One day in seconds
constexpr double YEAR = 365.25 * DAY; ///> One year in seconds

} // namespace Constants

inline double operator""_au(const long double x) {
  return x * Constants::AU;
} // Converts a value in astronomical units to meters

} // namespace phosphorus

#endif // PHOSPHORUS_INCLUDE_PHOSPHORUS_SCITIFICCONSTANTS_H
