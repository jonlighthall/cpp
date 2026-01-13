/**
 * Mathematical and physical constants for astronomical calculations.
 *
 * These values are fundamental constants that should not be modified.
 * Naming follows Google C++ Style Guide: kConstantName for constants.
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cmath>

// DESIGN NOTE: This namespace contains both mathematical and astronomical
// constants. While it could be split (math::kPi, astro::kAuMeters, etc.),
// the current organization keeps all domain constants together for sunset
// calculations. Mathematical constants (kPi, kDeg2Rad) are here because they
// are primarily used in astronomical calculations. Consider refactoring into
// separate namespaces (math::, astro::zenith::) if constants grow
// significantly.

namespace astro {

// Mathematical constants
constexpr double kPi = atan(1) * 4;

// Angle conversion factors
constexpr double kDeg2Rad = kPi / 180.0;
constexpr double kRad2Deg = 1.0 / kDeg2Rad;

// Time conversion factors
constexpr double kDegreesPerHour = 15.0;
constexpr double kMinutesPerHour = 60.0;

// Astronomical constants
constexpr double kAuMeters = 149597870700.0;  // Astronomical unit in meters

// measured solar radius from Mercury transits (2011 study)
// https://arxiv.org/abs/1203.4898
// 696,342 ± 65km
constexpr double kSolarRadiusMeters = 696342e3;  // Solar radius in meters

// Standard atmospheric refraction at horizon (degrees)
constexpr double kAtmosphericRefraction = 0.5667;

// Standard solar angular radius at 1 AU (degrees)
// Calculated from physical size: atan(radius/distance) converted to degrees
constexpr double kSolarAngularRadius =
    atan(kSolarRadiusMeters / kAuMeters) * kRad2Deg;  // ≈ 0.26667 degrees

// Standard elevation for sunset/sunrise calculations (degrees)
// This is -(solar_radius + atmospheric_refraction)
constexpr double kStandardSunsetElevation =
    -(kSolarAngularRadius + kAtmosphericRefraction);  // ≈ -0.83337 degrees

// Zenith angles for twilight events (degrees)
namespace zenith {
constexpr double kGoldenHourStart = 84.0;  // Sun 6° above horizon
constexpr double kSunset =
    90.0 - kStandardSunsetElevation;         // Standard sunset with refraction
constexpr double kGoldenHourEnd = 94.0;      // Sun 4° below horizon
constexpr double kCivilTwilight = 96.0;      // Sun 6° below horizon
constexpr double kNauticalTwilight = 102.0;  // Sun 12° below horizon
constexpr double kAstronomicalTwilight = 108.0;  // Sun 18° below horizon
}  // namespace zenith

// Julian date constants
constexpr double kJ2000Epoch = 2451545.0;       // Julian date of J2000.0 epoch
constexpr double kJulianCenturyDays = 36525.0;  // exactly 365.25 * 100

}  // namespace astro

#endif  // CONSTANTS_H
