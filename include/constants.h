/**
 * Mathematical and physical constants for astronomical calculations.
 *
 * These values are fundamental constants that should not be modified.
 * Naming follows Google C++ Style Guide: kConstantName for constants.
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cmath>

//TODO: this namespace could be refined or broken up into more informative
//groups. the constants like pi and deg2rad don't need to be in the astro
//namespace (more like math or trig). does c++ have an established definition of
//pi, etc? Similarly, the time constants have no business in "astro". The sun
//angle values seem like derived quantities and should be grouped with the
//zenith values. is it inapropriate to have the zenith values defined interms of
//+/- anges vs 90? kSunset should definitely refer to standard sunset elevation.

namespace astro {

// Mathematical constants
constexpr double kPi = atan(1) * 4;

// Angle conversion factors
constexpr double kDeg2Rad = kPi / 180.0;
constexpr double kRad2Deg = 180.0 / kPi;

// Time conversion factors
constexpr double kHoursPerDegree = 1.0 / 15.0;  // 15 degrees per hour
constexpr double kDegreesPerHour = 15.0;
constexpr double kMinutesPerHour = 60.0;
constexpr double kSecondsPerHour = 3600.0;

// Astronomical constants
constexpr double kAuMeters = 149597870700.0;  // Astronomical unit in meters
constexpr double kSolarRadiusMeters = 696342e3;  // Solar radius in meters

// Standard atmospheric refraction at horizon (degrees)
constexpr double kAtmosphericRefraction = 0.5667;

// Standard solar angular radius at 1 AU (degrees)
constexpr double kSolarAngularRadius = 0.26667;

// Standard elevation for sunset/sunrise calculations (degrees)
// This is -(solar_radius + atmospheric_refraction), rounded
constexpr double kStandardSunsetElevation = -0.833;

// Zenith angles for twilight events (degrees)
namespace zenith {
constexpr double kGoldenHourStart = 84.0;       // Sun 6° above horizon
constexpr double kSunset = 90.833;              // Standard sunset with refraction
constexpr double kGoldenHourEnd = 94.0;         // Sun 4° below horizon
constexpr double kCivilTwilight = 96.0;         // Sun 6° below horizon
constexpr double kNauticalTwilight = 102.0;     // Sun 12° below horizon
constexpr double kAstronomicalTwilight = 108.0; // Sun 18° below horizon
}  // namespace zenith

// Julian date constants
constexpr double kJ2000Epoch = 2451545.0;  // Julian date of J2000.0 epoch
constexpr double kJulianCenturyDays = 36525.0;

}  // namespace astro

#endif  // CONSTANTS_H
