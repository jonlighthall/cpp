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
  // Convert a Gregorian calendar date to Julian Day Number (integer).
  // Algorithm: proleptic Gregorian calendar conversion.
  // Source: Richards, E. G. (2013). "Calendars." In Explanatory Supplement
  //   to the Astronomical Almanac, 3rd ed., Eq. 15.11.
  // Also described in: Meeus (1991), Astronomical Algorithms, Ch. 7.
  // Note: Returns noon JD (integer). For sub-day precision, add fractional
  //   day offset separately.
  int a = (14 - month) / 12;
  int y = year + 4800 - a;
  int m = month + 12 * a - 3;
  int jd =
      day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
  return jd;
}

double SunsetCalculator::getJ2000(double jd) {
  // Convert Julian Date to days since J2000.0 epoch (2000 Jan 1.5 TT).
  // kJ2000Epoch = 2451545.0 (Julian Date of J2000.0)
  return jd - kJ2000Epoch;
}

double SunsetCalculator::getJulianCentury(double J2000) {
  // Convert days since J2000.0 to Julian centuries (T).
  // One Julian century = 36525 days (exactly 365.25 × 100).
  return J2000 / 36525.0;
}

// ============================================================================
// Solar Position Calculations (Geocentric Coordinates)
// ============================================================================

double SunsetCalculator::meanLongitude(double t, Algorithm algo) {
  // Geometric mean longitude of the Sun (degrees), referred to the mean
  // equinox of the date. Input t is Julian centuries from J2000.0.

  if (algo == Algorithm::USNO) {
    // Source: USNO, "Approximate Solar Coordinates."
    //   https://aa.usno.navy.mil/faq/sun_approx
    double L0 = 280.460 + 36000.771 * t;
    while (L0 > 360) L0 -= 360;
    while (L0 < 0) L0 += 360;
    return L0;
  } else if (algo == Algorithm::EXTENDED) {
    // Quintic polynomial from VSOP87 planetary theory
    //   (Bretagnon, P. & Francou, G., 1988).
    // Reproduced in: Meeus (1991), Astronomical Algorithms, Eq. 28.2 (p. 183).
    // Also used by the NOAA solar calculator for the equation of time.
    double L0 = 280.4664567 + 36000.76982779 * t + 0.03032028 * pow(t, 2) +
                pow(t, 3) / 49931 - pow(t, 4) / 15300 - pow(t, 5) / 2e6;
    while (L0 > 360) L0 -= 360;
    while (L0 < 0) L0 += 360;
    return L0;
  } else {  // Algorithm::NOAA (default)
    // Source: Meeus (1991), Astronomical Algorithms, Eq. 25.2 (p. 163).
    // Also used by the NOAA solar calculator spreadsheet.
    double L0 = 280.46646 + t * (36000.76983 + t * 0.0003032);
    while (L0 > 360) L0 -= 360;
    while (L0 < 0) L0 += 360;
    return L0;
  }
}

double SunsetCalculator::meanAnomaly(double t, Algorithm algo) {
  // Mean anomaly of the Sun (degrees). Measures angle from perihelion
  // assuming uniform orbital motion. Input t is Julian centuries from J2000.0.

  if (algo == Algorithm::USNO) {
    // Source: USNO, "Approximate Solar Coordinates."
    //   https://aa.usno.navy.mil/faq/sun_approx
    return 357.528 + 35999.050 * t;
  } else if (algo == Algorithm::EXTENDED) {
    // Source: Reda, I., & Andreas, A. (2008). "Solar position algorithm for
    //   solar radiation applications." NREL/TP-560-34302, Eq. 3.3.2.
    // Coefficients originally from Simon, J. L. et al. (1994).
    //   "Numerical expressions for precession formulae and mean elements
    //   for the Moon and the planets." A&A, 282, 663-683.
    return 357.52772 + 35999.050340 * t - 0.0001603 * pow(t, 2) +
           pow(t, 3) / 300000;
  } else {  // Algorithm::NOAA (default)
    // Source: Meeus (1991), Astronomical Algorithms, Eq. 25.3 (p. 163).
    return 357.52911 + t * (35999.05029 - t * 0.0001536);
  }
}

double SunsetCalculator::equationOfCenter(double t, double M, Algorithm algo) {
  // Sun's equation of center C (degrees).
  // C = true_anomaly - mean_anomaly. Added to mean longitude to get the
  // Sun's true geocentric longitude: L_true = L0 + C.
  // Input M is mean anomaly in degrees; t is Julian centuries from J2000.0.
  M *= kDeg2Rad;

  if (algo == Algorithm::USNO) {
    // Source: USNO, "Approximate Solar Coordinates."
    //   https://aa.usno.navy.mil/faq/sun_approx
    // Two-harmonic form with constant coefficients.
    return 1.915 * sin(M) + 0.020 * sin(2 * M);
  } else {  // Algorithm::NOAA and EXTENDED use the same time-dependent form
    // Source: Meeus (1991), Astronomical Algorithms, p. 164.
    // Three-harmonic form with time-dependent coefficients.
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
    default:
      // Higher precision cubic polynomial (default)
      // From: Reda, I., & Andreas, A. (2008). Solar position algorithm for
      // solar radiation applications. NREL Technical Report NREL/TP-560-34302.
      // This is Eq. 19 (X4) from the SPA algorithm
      return (125.04452 - 1934.136261 * t + 0.0020708 * pow(t, 2) +
              pow(t, 3) / 450000.0) *
             kDeg2Rad;
  }
}

double SunsetCalculator::nutationInLongitude(
    double Omega, double JCE, double X1,
    LongitudeAscendingNodeFormulation form) {
  // Nutation in longitude ΔΨ (degrees).
  // Nutation is the small, short-period wobble of Earth's rotation axis
  // superimposed on the slow 26,000-year precession, caused by the
  // gravitational pull of the Moon and Sun on Earth's equatorial bulge.
  // The largest component has a period of 18.6 years (Moon's nodal period).
  //
  // This is a simplified 4-term approximation of the IAU 1980 nutation
  // series. The full series has 63 terms — not a 63rd-degree polynomial,
  // but a 63-term trigonometric sum where each term is:
  //   ΔΨ_i = (A_i + B_i·T) × sin(Σ n_ij·F_j)
  // with 5 fundamental arguments F_j (Moon's node, Sun's mean anomaly,
  // Moon's mean anomaly, Moon's mean longitude, Moon-Sun elongation).
  // The modern IAU 2000A model uses 1365 terms.
  //
  // Our 4 terms capture ~97% of the total effect (~17" out of ~18.5").
  // The remaining 59 IAU 1980 terms collectively contribute <1 arcsecond.
  // For sunrise/sunset timing, this simplification introduces ~±30 seconds
  // of error. At or near the precision ceiling for a simplified model.
  //
  // Source: Meeus (1991), Astronomical Algorithms, Table 22.A.
  // Simplified 4-term form from the Astronomical Almanac (reduced accuracy).
  //
  // Parameters:
  //   Omega - longitude of ascending node of Moon's orbit (radians,
  //           as returned by longitudeAscendingNode())
  //   JCE   - Julian Ephemeris Century (used to compute Moon's mean longitude)
  //   X1    - Sun's mean longitude L₀ (degrees)
  //   form  - passed for API consistency (unused)
  //
  // Coefficients are in arcseconds, divided by 3600 to convert to degrees.
  // Note: Omega arrives in radians from longitudeAscendingNode().
  // X1 (Sun's mean longitude) arrives in degrees and must be converted.

  // Convert Sun's mean longitude to radians
  double X1_rad = X1 * kDeg2Rad;

  // Moon's mean longitude, simplified from Meeus (1991), Eq. 47.1 (p. 338).
  // Needed for nutation term 3: sin(2·L_moon).
  double L_moon_rad = (218.3165 + 481267.8813 * JCE) * kDeg2Rad;

  // IAU 1980 nutation series, 4 largest terms (Table 22.A rows 1-4):
  //   Row 1: argument Ω             → sin(Ω)
  //   Row 2: argument -2D+2F+2Ω     → simplified to sin(2L₀)
  //   Row 3: argument 2F+2Ω         → simplified to sin(2L_moon)
  //   Row 4: argument 2Ω            → sin(2Ω)
  double psi = -17.20 / 3600.0 * sin(Omega);    // Term 1: -17.20" sin(Ω)
  psi += -1.32 / 3600.0 * sin(2 * X1_rad);      // Term 2: -1.32" sin(2L₀)
  psi += -0.23 / 3600.0 * sin(2 * L_moon_rad);  // Term 3: -0.23" sin(2L_m)
  psi += 0.21 / 3600.0 * sin(2 * Omega);        // Term 4: +0.21" sin(2Ω)

  return psi;
}

double SunsetCalculator::eccentricity(double t) {
  // Eccentricity of Earth's orbit (dimensionless).
  // Input t is Julian centuries from J2000.0.
  // Source: Meeus (1991), Astronomical Algorithms, Eq. 25.4 (p. 163).
  return 0.016708634 - t * (0.000042037 + t * 0.0000001267);
}

double SunsetCalculator::radiusVector(double e, double nu) {
  // Earth-Sun distance in Astronomical Units (AU).
  // Standard conic section formula: R = a(1-e²)/(1 + e·cos(ν)).
  // Source: Meeus (1991), Astronomical Algorithms, Eq. 25.9 (p. 164).
  // Coefficient 1.000001018 is the semi-major axis in AU.
  return (1.000001018 * (1 - e * e)) / (1 + e * cos(nu * kDeg2Rad));
}

double SunsetCalculator::obliquityOfEcliptic(double T, Algorithm algo) {
  // Obliquity of the ecliptic ε (degrees): angle of Earth's axial tilt.
  // This is the angle between Earth's equatorial plane and orbital plane.
  // Input T is Julian centuries from J2000.0.

  // Reference obliquity at J2000.0 epoch: 23°26'21.448"
  // Source: IAU 1976 value, Lieske (1979). Also Meeus (1991), Eq. 22.2.
  double epsilon0 = 23.0 + (26.0 + 21.448 / 60.0) / 60.0;  // 23.439291°

  // Convert Julian centuries to Julian 10,000-year units for Laskar polynomial
  double U = T / 100.0;

  if (algo == Algorithm::USNO) {
    // First-order truncation of the Laskar (1986) polynomial.
    // Coefficient -4680.93"/U is from Laskar; see Meeus (1991) Eq. 22.3.
    // NOTE: labeled USNO in the Algorithm enum but the coefficient is from
    // Laskar (1986), not the USNO approximation page.
    double t0 = epsilon0 * 3600.0;  // in arcseconds
    double epsilon = t0 - 4680.93 * U;
    return epsilon / 3600.0;  // convert back to degrees
  } else if (algo == Algorithm::EXTENDED) {
    // Full tenth-degree polynomial.
    // Source: Laskar, J. (1986). "Secular terms of classical planetary
    //   theories using the results of general theory." Astronomy and
    //   Astrophysics, 157, 59-70.
    // Reproduced in: Meeus (1991), Astronomical Algorithms, Eq. 22.3 (p. 135).
    // Coefficients are in arcseconds; U is in Julian 10,000-year units.
    double t0 = epsilon0 * 3600.0;
    double epsilon = t0 - 4680.93 * U - 1.55 * pow(U, 2) + 1999.25 * pow(U, 3) -
                     51.38 * pow(U, 4) - 249.67 * pow(U, 5) -
                     39.05 * pow(U, 6) + 7.12 * pow(U, 7) + 27.87 * pow(U, 8) +
                     5.79 * pow(U, 9) + 2.45 * pow(U, 10);
    return epsilon / 3600.0;
  } else {  // Algorithm::NOAA (default)
    // Source: Lieske, J. H. (1979). "Precession matrix based on IAU (1976)
    //   system of astronomical constants." Astronomy and Astrophysics, 73, 282.
    // Reproduced in NOAA solar calculator spreadsheet.
    // Coefficients converted from arcsec/century to arcsec/U (U = T/100).
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
  // Equation of Time (hours): difference between apparent and mean solar time.
  // This direct formula approximates EoT from orbital elements.
  // Source: Meeus (1991), Astronomical Algorithms, Ch. 28 (p. 183-185).
  // Also used by the NOAA solar calculator spreadsheet.
  // NOTE: previously attributed to Smart (1956) Text-Book on Spherical
  // Astronomy, p. 149. Smart derives EoT conceptually (RA - mean sun) but
  // does not give this direct closed-form expression. The formula below
  // is from Meeus / the NOAA spreadsheet.

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
  // Source: Meeus (1991), Astronomical Algorithms, Ch. 15 (p. 101-103).
  double apparentSunsetElevation = -0.833;

  // Altitude correction: elevated observer sees a lower geometric horizon.
  // Dip angle d ≈ 1.76' × √(h_meters) from geometric derivation;
  // refraction-adjusted coefficient is 2.076'/√m.
  // Source: Meeus (1991), Eq. 15.1 (p. 102); also Hohenkerk & Sinclair
  //   (1985) in the Nautical Almanac.
  if (altitude_meters > 0) {
    double altitudeCorrection = -2.076 * sqrt(altitude_meters) / 60.0;
    apparentSunsetElevation += altitudeCorrection;
  }

  // Convert elevation to zenith angle
  return 90.0 - apparentSunsetElevation;
}

double SunsetCalculator::hourAngle(double h0, double phi, double delta) {
  // Hour angle H₀ at which the Sun reaches zenith angle h₀ (degrees).
  // Standard spherical trigonometry: cos(H₀) = (cos(h₀) - sin(φ)sin(δ)) /
  //   (cos(φ)cos(δ)).
  // Source: Meeus (1991), Astronomical Algorithms, Eq. 15.1 (p. 102).
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
  // PRECISION NOTE: Annual aberration not applied here. The apparent position
  // of the Sun is shifted by ~20.5" due to Earth's orbital velocity and the
  // finite speed of light (κ = v_earth/c × 206265" ≈ 20.496").
  // Adding this would be: lambda -= 20.49552 / 3600.0;
  // Effect on rise/set: ~15 seconds. Source: Meeus (1991), Eq. 23.2 (p. 151).

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
  // Equation of time from Meeus (1991) Ch. 28 / NOAA spreadsheet
  // PRECISION NOTE: This is a single-pass calculation. Orbital elements were
  // computed for noon UT, but the actual transit time differs from noon.
  // Iterating (recompute elements at estimated transit → re-derive transit)
  // converges in 2–3 passes and improves accuracy by ~10–15 seconds.
  // See: Reda & Andreas (2008), Section 3.6.
  double y = pow(tan(epsilon * kDeg2Rad / 2.0), 2);
  double L_rad = L * kDeg2Rad;
  double M_rad = M * kDeg2Rad;
  double E_rad = y * sin(2 * L_rad) - 2 * e * sin(M_rad) +
                 4 * e * y * sin(M_rad) * cos(2 * L_rad) -
                 0.5 * y * y * sin(4 * L_rad) - 1.25 * e * e * sin(2 * M_rad);
  double eot = E_rad * kRad2Deg / 15.0;  // Convert to hours

  // PRECISION NOTE: All polynomial formulas compute positions in Terrestrial
  // Time (TT), a uniform atomic-clock timescale. Sunrise/sunset times are
  // experienced in Universal Time (UT1), which tracks Earth's actual rotation.
  // Currently ΔT = TT − UT1 ≈ 69 seconds. Not corrected here; effect on
  // rise/set: ~69 seconds for current dates, grows for historical/future.
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

  // Get solar noon, declination, and sunset from common calculation
  double solar_noon, delta;
  double sunset = getSunset(year, month, day, latitude, longitude, timezone,
                            altitude_meters, &solar_noon, &delta);

  // Safety check: if getSunset returned invalid time, propagate error
  if (solar_noon < 0.0 || solar_noon >= 24.0) {
    if (out_solarNoon) *out_solarNoon = -1.0;
    if (out_delta) *out_delta = -999.0;
    return -1.0;
  }

  // Sunrise is symmetric around solar noon: sunrise = noon - HA, sunset =
  // noon + HA. Instead of recalculating, derive HA from the sunset result.
  double HA = sunset - solar_noon;
  double sunrise_time = solar_noon - HA;

  while (sunrise_time >= 24.0) sunrise_time -= 24.0;
  while (sunrise_time < 0.0) sunrise_time += 24.0;

  if (out_solarNoon) *out_solarNoon = solar_noon;
  if (out_delta) *out_delta = delta;

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
