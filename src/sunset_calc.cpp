#include "sunset_calc.h"

#include <cmath>
#include <cstdio>

#include "constants.h"

using namespace astro;

namespace sunset_calc {

// ============================================================================
// Julian Date and Time Epoch Calculations
// ============================================================================

double SunsetCalculator::getJulianDate(int year, int month, int day) {
  int a = (14 - month) / 12;
  int y = year + 4800 - a;
  int m = month + 12 * a - 3;
  int jd =
      day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
  return jd;
}

double SunsetCalculator::getJ2000(double jd) {
  // convert Julian Date to J2000 epoch, that is, the Julian Date since Jan 1,
  // 2000
  return jd - kJ2000Epoch;
}

double SunsetCalculator::getJulianCentury(double J2000) {
  // convert J2000 date to Julian Ephemeris Century, that is, fraction of a
  // Julian century
  return J2000 / 36525.0;
}

// ============================================================================
// Solar Position Calculations (Geocentric Coordinates)
// ============================================================================

double SunsetCalculator::meanLongitude(double t, Algorithm algo) {
  // Geometric mean longitude of the sun
  // Three implementations for research/comparison purposes

  if (algo == Algorithm::USNO) {
    // Linear approximation from U.S. Naval Observatory
    double L0 = 280.460 + 36000.771 * t;
    while (L0 > 360) L0 -= 360;
    while (L0 < 0) L0 += 360;
    return L0;
  } else if (algo == Algorithm::LASKAR) {
    // High-order polynomial from Laskar (1986)
    double L0 = 280.4664567 + 36000.76982779 * t + 0.03032028 * pow(t, 2) +
                pow(t, 3) / 49931 - pow(t, 4) / 15300 - pow(t, 5) / 2e6;
    while (L0 > 360) L0 -= 360;
    while (L0 < 0) L0 += 360;
    return L0;
  } else {  // Algorithm::NOAA (default)
    // Quadratic fit from NOAA
    double L0 = 280.46646 + t * (36000.76983 + t * 0.0003032);
    while (L0 > 360) L0 -= 360;
    while (L0 < 0) L0 += 360;
    return L0;
  }
}

double SunsetCalculator::meanAnomaly(double t, Algorithm algo) {
  // Mean anomaly of the sun (degrees)

  if (algo == Algorithm::USNO) {
    // Linear approximation
    return 357.528 + 35999.050 * t;
  } else if (algo == Algorithm::LASKAR) {
    // Cubic fit from Reda & Andreas (2008)
    return 357.52772 + 35999.050340 * t - 0.0001603 * pow(t, 2) +
           pow(t, 3) / 300000;
  } else {  // Algorithm::NOAA (default)
    // Quadratic fit
    return 357.52911 + t * (35999.05029 - t * 0.0001536);
  }
}

double SunsetCalculator::equationOfCenter(double t, double M, Algorithm algo) {
  // Sun's equation of center
  // Geocentric apparent ecliptic longitude of the Sun (adjusted for
  // aberration)
  M *= kDeg2Rad;

  if (algo == Algorithm::USNO) {
    // Constant coefficients (simplest)
    return 1.915 * sin(M) + 0.020 * sin(2 * M);
  } else if (algo == Algorithm::LASKAR) {
    // Same as NOAA (both use time-dependent coefficients)
    double C = (1.914602 - (0.004817 * t) - (0.000014 * pow(t, 2))) * sin(M) +
               (0.019993 - (0.000101 * t)) * sin(2 * M) +
               (0.000289 * sin(3 * M));
    return C;
  } else {  // Algorithm::NOAA (default)
    // Time-dependent coefficients from NOAA
    double C = (1.914602 - (0.004817 * t) - (0.000014 * pow(t, 2))) * sin(M) +
               (0.019993 - (0.000101 * t)) * sin(2 * M) +
               (0.000289 * sin(3 * M));
    return C;
  }
}

double SunsetCalculator::longitudeAscendingNode(
    double t, LongitudeAscendingNodeFormulation form) {
  // Longitude of the ascending node of the lunar orbit
  // Used in nutation calculations
  //
  // Two formulations available:
  // - NOAA_LINEAR: Simple approximation, used by NOAA
  // - REDA_ANDREAS_SPA: More precise cubic polynomial from Reda & Andreas
  // (2008)

  switch (form) {
    case LongitudeAscendingNodeFormulation::NOAA_LINEAR:
      // Simple linear form from NOAA
      return (125.04 - 1934.136 * t) * kDeg2Rad;

    case LongitudeAscendingNodeFormulation::REDA_ANDREAS_SPA:
      // Higher precision cubic polynomial
      // From: Reda, I., & Andreas, A. (2008). Solar position algorithm for
      // solar radiation applications. NREL Technical Report NREL/TP-560-34302.
      // This is Eq. 19 (X4) from the SPA algorithm
      return (125.04452 - 1934.136261 * t + 0.0020708 * pow(t, 2) +
              pow(t, 3) / 450000.0) *
             kDeg2Rad;

    default:
      // Default to higher precision
      return (125.04452 - 1934.136261 * t + 0.0020708 * pow(t, 2) +
              pow(t, 3) / 450000.0) *
             kDeg2Rad;
  }
}

double SunsetCalculator::nutationInLongitude(
    double Omega, double JCE, double X1,
    LongitudeAscendingNodeFormulation form) {
  // Nutation in longitude (degrees)
  // This accounts for small periodic variations in Earth's axis
  // JCE is Julian Ephemeris Century
  // form parameter is passed for consistency but not used in simple nutation
  // calculation
  Omega *= kDeg2Rad;

  double psi = -17.20 / 3600.0 * sin(Omega);
  psi += -1.32 / 3600.0 * sin(2 * X1);
  psi += -0.23 / 3600.0 * sin(2 * Omega);
  psi += 0.21 / 3600.0 * sin(2 * Omega);

  return psi;
}

double SunsetCalculator::eccentricity(double t) {
  // eccentricity of Earth's orbit
  // input is Julian century
  // Meeus pg. 163, Eq. 25.4

  return 0.016708634 - t * (0.000042037 + t * 0.0000001267);
}

double SunsetCalculator::radiusVector(double e, double nu) {
  // Distance from sun to earth (Astronomical Units)
  return (1.000001018 * (1 - e * e)) / (1 + e * cos(nu * kDeg2Rad));  // NOAA
}

double SunsetCalculator::obliquityOfEcliptic(double T, Algorithm algo) {
  // Obliquity of the ecliptic (angle of Earth's axial tilt)
  // This is the angle between Earth's equatorial plane and orbital plane

  // Reference obliquity at J2000 epoch
  double epsilon0 = 23.0 + (26.0 + 21.448 / 60.0) / 60.0;  // 23.439291°

  // Convert Julian centuries to 10,000 Julian years for Laskar
  double U = T / 100.0;

  if (algo == Algorithm::USNO) {
    // Linear fit used by USNO
    double t0 = epsilon0 * 3600.0;  // in arcseconds
    double epsilon = t0 - 4680.93 * U;
    return epsilon / 3600.0;  // convert back to degrees
  } else if (algo == Algorithm::LASKAR) {
    // Tenth-degree polynomial from Laskar (1986)
    double t0 = epsilon0 * 3600.0;
    double epsilon = t0 - 1.55 * pow(U, 2) + 1999.25 * pow(U, 3) -
                     51.38 * pow(U, 4) - 249.67 * pow(U, 5) -
                     39.05 * pow(U, 6) + 7.12 * pow(U, 7) + 27.87 * pow(U, 8) +
                     5.79 * pow(U, 9) + 2.45 * pow(U, 10);
    return epsilon / 3600.0;
  } else {  // Algorithm::NOAA (default)
    // Cubic fit used by NOAA
    double t0 = epsilon0 * 3600.0;
    double epsilon = t0 - 4681.5 * U - 5.9 * pow(U, 2) + 1813 * pow(U, 3);
    return epsilon / 3600.0;
  }
}

// ============================================================================
// Time and Angle Calculations
// ============================================================================

double SunsetCalculator::equationOfTime(double M, double RA, double DPsi,
                                        double epsilon, double L) {
  // Equation of Time: correction to convert solar time to mean time
  // Using Smart (1956) formula for higher accuracy
  // W.M. Smart, Text-Book on Spherical Astronomy, Cambridge University Press,
  // 1956, p. 149

  double e = eccentricity((getJ2000(getJulianDate(2000, 1, 1))) / 36525.0);
  double y = pow(tan(epsilon * kDeg2Rad / 2.0), 2);

  // Convert to radians
  double L_rad = L * kDeg2Rad;
  double M_rad = M * kDeg2Rad;

  double E_rad = y * sin(2 * L_rad) - 2 * e * sin(M_rad) +
                 4 * e * y * sin(M_rad) * cos(2 * L_rad) -
                 0.5 * y * y * sin(4 * L_rad) - 1.25 * e * e * sin(2 * M_rad);

  // Convert from radians to hours
  return E_rad * kRad2Deg / 15.0;
}

double SunsetCalculator::getZenith(double e, double nu,
                                   double altitude_meters) {
  // Solar zenith angle at standard sunset/sunrise
  // Includes solar radius, atmospheric refraction, and altitude correction

  // True longitude of the Sun (not used in simplified version)
  // double sun_lon = e + nu;
  // double lambda = sun_lon;  // Simplified, could add nutation correction

  // Standard elevation angle for sunset/sunrise
  // = -(solar_radius + atmospheric_refraction)
  // ≈ -0.833 degrees
  double apparentSunsetElevation = -0.833;

  // Altitude correction (observer at elevation sees sun for longer)
  // Formula from MATLAB sunrise.m: -2.076*sqrt(alt)/60 degrees
  // where alt is altitude in meters
  if (altitude_meters > 0) {
    double altitudeCorrection = -2.076 * sqrt(altitude_meters) / 60.0;
    apparentSunsetElevation += altitudeCorrection;
  }

  // Convert elevation to zenith angle
  return 90.0 - apparentSunsetElevation;
}

double SunsetCalculator::hourAngle(double h0, double phi, double delta) {
  // Calculate hour angle for the sun at a given zenith angle
  // h0: zenith angle (degrees)
  // phi: observer latitude (degrees)
  // delta: solar declination (degrees)

  h0 *= kDeg2Rad;
  phi *= kDeg2Rad;
  delta *= kDeg2Rad;

  double cos_H0 = (cos(h0) - sin(phi) * sin(delta)) / (cos(phi) * cos(delta));

  // Check if sun is above horizon
  if (cos_H0 > 1.0) return 0.0;     // Sun never rises
  if (cos_H0 < -1.0) return 180.0;  // Sun never sets

  double H0 = acos(cos_H0);
  return H0 * kRad2Deg;
}

double SunsetCalculator::getSolarNoon(double longitude, int timezone) {
  // Solar noon is when the sun reaches its highest point
  // J_noon = J_transit - (longitude + E_Omega) / 360
  // Simplified version: solar noon in local time

  // Julian day at J2000 epoch
  // Approximate formula for solar noon
  double J_noon = 0.0009 - (longitude / 360.0);

  // Convert from solar time to local time
  double solar_noon_hours = 12.0 - (J_noon * 24.0);

  // Adjust for timezone
  solar_noon_hours += timezone;

  // Normalize to 0-24
  while (solar_noon_hours >= 24) solar_noon_hours -= 24;
  while (solar_noon_hours < 0) solar_noon_hours += 24;

  return solar_noon_hours;
}

// ============================================================================
// Main Sunset/Sunrise Calculation
// ============================================================================

double SunsetCalculator::getSunset(int year, int month, int day,
                                   double latitude, double longitude,
                                   int timezone, double altitude_meters,
                                   double* out_solarNoon, double* out_delta) {
  // Validate input parameters
  if (!validateInputs(year, month, day, latitude, longitude, timezone)) {
    // Return invalid time (24.0) to indicate calculation failure
    if (out_solarNoon) *out_solarNoon = -1.0;
    if (out_delta) *out_delta = -999.0;
    return 24.0;
  }

  // Step 1: Convert calendar date to Julian Date
  double jd = getJulianDate(year, month, day);

  // Step 2: Convert to J2000 epoch (days since Jan 1, 2000 noon)
  double J2000 = getJ2000(jd);

  // Step 3: Convert to Julian Centuries (centuries from J2000)
  double t = getJulianCentury(J2000);

  // Step 4: Calculate mean elements of the Sun's orbit
  double L = meanLongitude(t);  // Mean longitude
  double M = meanAnomaly(t);    // Mean anomaly

  // Step 5: Calculate equation of center and true longitude
  double C = equationOfCenter(t, M);
  double sun_lon = L + C;  // True longitude

  // Step 6: Apparent longitude (with nutation)
  double Omega = longitudeAscendingNode(t);
  double nutation = nutationInLongitude(Omega, t, L);
  double lambda = sun_lon + nutation;

  // Step 7: Calculate Earth-Sun distance
  double e = eccentricity(t);
  double nu = M + C;  // True anomaly
  (void)radiusVector(
      e, nu);  // Calculated for completeness in simplified algorithm

  // Step 8: Apparent solar diameter and zenith angle
  double zenith =
      getZenith(nu, C, altitude_meters);  // Includes altitude correction

  // Step 9: Calculate solar declination
  double epsilon = obliquityOfEcliptic(t);
  double delta =
      asin(sin(epsilon * kDeg2Rad) * sin(lambda * kDeg2Rad)) * kRad2Deg;

  // Step 10: Calculate hour angle for sunset
  double HA_deg = hourAngle(zenith, latitude, delta);
  double HA = HA_deg / 15.0;  // Convert degrees to hours

  // Step 11: Calculate solar noon in local time
  // Use Smart (1956) equation of time with current eccentricity
  double y = pow(tan(epsilon * kDeg2Rad / 2.0), 2);
  double L_rad = L * kDeg2Rad;
  double M_rad = M * kDeg2Rad;
  double E_rad = y * sin(2 * L_rad) - 2 * e * sin(M_rad) +
                 4 * e * y * sin(M_rad) * cos(2 * L_rad) -
                 0.5 * y * y * sin(4 * L_rad) - 1.25 * e * e * sin(2 * M_rad);
  double eot = E_rad * kRad2Deg / 15.0;  // Convert to hours

  double solar_noon_utc = 12.0 - (longitude / 15.0) - eot;

  // Adjust for timezone
  double solar_noon = solar_noon_utc + timezone;

  // Normalize to 0-24 hour range
  while (solar_noon >= 24.0) solar_noon -= 24.0;
  while (solar_noon < 0.0) solar_noon += 24.0;

  // Step 12: Calculate sunset time
  double sunset_time = solar_noon + HA;

  // Normalize to 0-24 hour range
  while (sunset_time >= 24.0) sunset_time -= 24.0;
  while (sunset_time < 0.0) sunset_time += 24.0;

  // Store output parameters if requested
  if (out_solarNoon) *out_solarNoon = solar_noon;
  if (out_delta) *out_delta = delta;

  return sunset_time;
}

double SunsetCalculator::getSunrise(int year, int month, int day,
                                    double latitude, double longitude,
                                    int timezone, double altitude_meters,
                                    double* out_solarNoon, double* out_delta) {
  // Validate input parameters
  if (!validateInputs(year, month, day, latitude, longitude, timezone)) {
    if (out_solarNoon) *out_solarNoon = -1.0;
    if (out_delta) *out_delta = -999.0;
    return -1.0;
  }

  // Get solar noon and declination from sunset calculation
  double solar_noon, delta;
  getSunset(year, month, day, latitude, longitude, timezone, altitude_meters,
            &solar_noon, &delta);

  // Safety check: if getSunset returned invalid time, propagate error
  if (solar_noon < 0.0 || solar_noon >= 24.0) {
    if (out_solarNoon) *out_solarNoon = -1.0;
    if (out_delta) *out_delta = -999.0;
    return -1.0;
  }

  // For sunrise, we need to recalculate with the same method
  // Simplified version: sunrise is symmetric around solar noon
  double jd = getJulianDate(year, month, day);
  double J2000 = getJ2000(jd);
  double t = getJulianCentury(J2000);

  double L = meanLongitude(t);
  double M = meanAnomaly(t);
  double C = equationOfCenter(t, M);
  double sun_lon = L + C;

  double Omega = longitudeAscendingNode(t);
  double nutation = nutationInLongitude(Omega, t, L);
  double lambda = sun_lon + nutation;

  double epsilon = obliquityOfEcliptic(t);
  double calculated_delta =
      asin(sin(epsilon * kDeg2Rad) * sin(lambda * kDeg2Rad)) * kRad2Deg;

  double zenith =
      getZenith(M + C, C, altitude_meters);  // Includes altitude correction
  double HA_deg = hourAngle(zenith, latitude, calculated_delta);
  double HA = HA_deg / 15.0;

  double sunrise_time = solar_noon - HA;

  while (sunrise_time >= 24.0) sunrise_time -= 24.0;
  while (sunrise_time < 0.0) sunrise_time += 24.0;

  if (out_solarNoon) *out_solarNoon = solar_noon;
  if (out_delta) *out_delta = calculated_delta;

  return sunrise_time;
}

// ============================================================================
// Utility Functions
// ============================================================================

void SunsetCalculator::decimalHoursToHMS(double hours, int& outHours,
                                         int& outMinutes, int& outSeconds) {
  outHours = static_cast<int>(hours);
  double remaining = (hours - outHours) * 60.0;
  outMinutes = static_cast<int>(remaining);
  outSeconds = static_cast<int>((remaining - outMinutes) * 60.0);
}

void SunsetCalculator::decimalHoursToString(double hours, char* buffer,
                                            int bufferSize, bool includeMinutes,
                                            bool includeSeconds) {
  if (!buffer || bufferSize < 6) return;

  int h, m, s;
  decimalHoursToHMS(hours, h, m, s);

  if (includeSeconds) {
    snprintf(buffer, bufferSize, "%02d:%02d:%02d", h, m, s);
  } else if (includeMinutes) {
    snprintf(buffer, bufferSize, "%02d:%02d", h, m);
  } else {
    snprintf(buffer, bufferSize, "%02d", h);
  }
}

bool SunsetCalculator::validateInputs(int year, int month, int day,
                                      double latitude, double longitude,
                                      int timezone) {
  // Validate year (reasonable range for astronomical calculations)
  if (year < 1900 || year > 2100) return false;

  // Validate month (1-12)
  if (month < 1 || month > 12) return false;

  // Validate day (1-31; note: doesn't check month-specific limits)
  if (day < 1 || day > 31) return false;

  // Validate latitude (-90 to 90 degrees)
  if (latitude < -90.0 || latitude > 90.0) return false;

  // Validate longitude (-180 to 180 degrees)
  if (longitude < -180.0 || longitude > 180.0) return false;

  // Validate timezone (-12 to +14 hours covers all practical timezones)
  // Note: This assumes integer timezone; fractional offsets like +5:30 would
  // need the timezone parameter to be a double instead
  if (timezone < -12 || timezone > 14) return false;

  return true;
}

}  // namespace sunset_calc
