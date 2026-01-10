#ifndef SOLAR_UTILS_H
#define SOLAR_UTILS_H

// Shared solar utility functions and constants for twilight/sunset programs.
// Keep simple and educational per repo guidance.

#include "constants.h"  // for astro::kDeg2Rad, kRad2Deg and standard elevation

namespace solar_utils {
// Golden hour convention (photography-oriented):
// Start when sun altitude is +6° above horizon (angle relative to horizon =
// -6°) End when sun altitude is −4° (angle relative to horizon = +4°)
constexpr double kGoldenStartAngle = -6.0;  // degrees (above horizon)
constexpr double kGoldenEndAngle = 4.0;     // degrees (below horizon)

// Convert sun angle (degrees relative to horizon) to zenith angle.
// Convention: sunAngle < 0 => degrees ABOVE horizon; sunAngle > 0 => BELOW.
// Altitude a = -sunAngle; Zenith z = 90 - a = 90 + sunAngle.
inline double sunAngleToZenith(double sunAngle) { return 90.0 + sunAngle; }

// Calculate hour angle for a given zenith angle and latitude/declination.
// Returns degrees; caller can convert to hours by dividing by 15.
inline double calcHourAngle(double zenithAngle, double latitude, double delta) {
  double h0 = zenithAngle * astro::kDeg2Rad;
  double phi = latitude * astro::kDeg2Rad;
  double d = delta * astro::kDeg2Rad;

  double cosH = (cos(h0) - sin(phi) * sin(d)) / (cos(phi) * cos(d));
  if (cosH < -1.0 || cosH > 1.0) {
    return -1.0;  // Event doesn't occur
  }
  return acos(cosH) * astro::kRad2Deg;
}

}  // namespace solar_utils

#endif  // SOLAR_UTILS_H
