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

double SunsetCalculator::getJ2000(double jd) { return jd - kJ2000Epoch; }

double SunsetCalculator::getJulianCentury(double J2000) {
  return J2000 / 36525.0;
}

// ============================================================================
// Solar Position Calculations (Geocentric Coordinates)
// ============================================================================

double SunsetCalculator::meanLongitude(double t) {
  // Geometric mean longitude of the sun
  double L0 = 280.46646 + t * (36000.76983 + t * 0.0003032);
  // Normalize to 0-360
  while (L0 > 360) L0 -= 360;
  while (L0 < 0) L0 += 360;
  return L0;
}

double SunsetCalculator::meanAnomaly(double t) {
  // Mean anomaly of the sun (degrees)
  return 357.52911 + t * (35999.05029 - t * 0.0001536);
}

double SunsetCalculator::equationOfCenter(double t, double M) {
  // Sun's equation of center
  // Geocentric apparent ecliptic longitude of the Sun (adjusted for
  // aberration)
  M *= kDeg2Rad;

  double C_2 = (1.914602 - (0.004817 * t) - (0.000014 * pow(t, 2))) * sin(M) +
               (0.019993 - (0.000101 * t)) * sin(2 * M) +
               (0.000289 * sin(3 * M));

  return C_2;
}

double SunsetCalculator::longitudeAscendingNode(double t) {
  // Longitude of the ascending node of the lunar orbit
  // Used in nutation calculations
  return 125.04 - 1934.136 * t;
}

double SunsetCalculator::nutationInLongitude(double Omega, double JCE,
                                             double X1) {
  // Nutation in longitude (degrees)
  // This accounts for small periodic variations in Earth's axis
  // JCE is Julian Ephemeris Century
  Omega *= kDeg2Rad;

  double psi = -17.20 / 3600.0 * sin(Omega);
  psi += -1.32 / 3600.0 * sin(2 * X1);
  psi += -0.23 / 3600.0 * sin(2 * Omega);
  psi += 0.21 / 3600.0 * sin(2 * Omega);

  return psi;
}

double SunsetCalculator::eccentricity(double t) {
  // Eccentricity of Earth's orbit
  return 0.016708634 - t * (0.000042037 + t * 0.0000001267);
}

double SunsetCalculator::radiusVector(double e, double nu) {
  // Distance from sun to earth (Astronomical Units)
  return (1.000001018 * (1 - e * e)) / (1 + e * cos(nu * kDeg2Rad));
}

double SunsetCalculator::obliquityOfEcliptic(double T) {
  // Obliquity of the ecliptic (angle of Earth's axial tilt)
  // This is the angle between Earth's equatorial plane and orbital plane

  // Mean obliquity
  double seconds = 21.448 - T * (46.8150 + T * (0.00059 - T * (0.001813)));
  double epsilon0 = 23.0 + (26.0 + seconds / 60.0) / 60.0;

  // Calculate nutation components
  double Omega = longitudeAscendingNode(T);
  double X1 = meanLongitude(T);

  // Nutation in obliquity
  double epsilon_c = 9.452 / 3600.0 * cos(Omega * kDeg2Rad);
  epsilon_c += 0.016 / 3600.0 * cos(2 * X1 * kDeg2Rad);

  return epsilon0 + epsilon_c;
}

// ============================================================================
// Time and Angle Calculations
// ============================================================================

double SunsetCalculator::equationOfTime(double M, double RA, double DPsi,
                                        double epsilon, double L) {
  // Equation of Time: correction to convert solar time to mean time
  // This accounts for the elliptical orbit and axial tilt

  M *= kDeg2Rad;

  // Coefficients from NOAA algorithm
  double eot = 229.18 * (0.000075 + 0.001868 * cos(M) - 0.032077 * sin(M) -
                         0.014615 * cos(2 * M) - 0.040849 * sin(2 * M));

  return eot;
}

double SunsetCalculator::getZenith(double e, double nu) {
  // Solar zenith angle at standard sunset/sunrise
  // Includes solar radius and atmospheric refraction

  // True longitude of the Sun
  double sun_lon = e + nu;

  // Apparent longitude of sun
  double lambda = sun_lon;  // Simplified, could add nutation correction

  // Geocentric declination of the Sun
  double delta =
      asin(sin(obliquityOfEcliptic(0) * kDeg2Rad) * sin(lambda * kDeg2Rad)) *
      kRad2Deg;

  // Standard elevation angle for sunset/sunrise
  // = -(solar_radius + atmospheric_refraction)
  // ≈ -0.833 degrees
  double apparentSunsetElevation = -0.833;

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
                                   int timezone, double* out_solarNoon,
                                   double* out_delta) {
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
  double zenith = 90.0 + 0.833;  // Standard sunset zenith angle

  // Step 9: Calculate solar declination
  double epsilon = obliquityOfEcliptic(t);
  double delta =
      asin(sin(epsilon * kDeg2Rad) * sin(lambda * kDeg2Rad)) * kRad2Deg;

  // Step 10: Calculate hour angle for sunset
  double HA_deg = hourAngle(zenith, latitude, delta);
  double HA = HA_deg / 15.0;  // Convert degrees to hours

  // Step 11: Calculate solar noon in local time
  double eot = equationOfTime(M, 0, nutation, epsilon, L);  // Equation of Time
  double solar_noon_utc = 12.0 - (longitude / 15.0) - (eot / 60.0);

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
                                    int timezone, double* out_solarNoon,
                                    double* out_delta) {
  // Validate input parameters
  if (!validateInputs(year, month, day, latitude, longitude, timezone)) {
    if (out_solarNoon) *out_solarNoon = -1.0;
    if (out_delta) *out_delta = -999.0;
    return -1.0;
  }

  // Get solar noon and declination from sunset calculation
  double solar_noon, delta;
  getSunset(year, month, day, latitude, longitude, timezone, &solar_noon,
            &delta);

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

  double zenith = 90.0 + 0.833;
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
