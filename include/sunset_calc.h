/**
 * Sunset Time Calculation Library
 *
 * Core astronomical calculations for determining sunset and sunrise times.
 * Designed to be portable across platforms (desktop C++ and Arduino).
 *
 * Usage:
 *   SunsetCalculator calc;
 *   double sunsetTime = calc.getSunset(2026, 1, 10, 40.5, -74.0, -5);
 *   // Returns sunset time in hours (e.g., 17.25 = 5:15 PM)
 *
 * References:
 *   - James Still's solar coordinates:
 * https://squarewidget.com/solar-coordinates/
 *   - USNO: https://aa.usno.navy.mil/faq/sun_approx
 *   - Meeus, Jean (1991). Astronomical Algorithms.
 */

#ifndef SUNSET_CALC_H
#define SUNSET_CALC_H

#include <cmath>

#include "config_location.h"

namespace sunset_calc {

/**
 * Algorithm selection for multi-variant calculations.
 * NOAA: Most common, good balance of accuracy and simplicity
 * USNO: Linear approximation from U.S. Naval Observatory
 * LASKAR: High-order polynomial from Laskar (1986), highest accuracy
 */
enum class Algorithm { NOAA, USNO, LASKAR };

/**
 * Formulation selection for longitude of ascending node.
 * Different authoritative sources use different equations.
 * NOAA_LINEAR: Simple linear form from NOAA
 *   125.04 - 1934.136 * t
 * REDA_ANDREAS_SPA: Cubic polynomial from Reda & Andreas (2008)
 *   "Solar position algorithm for solar radiation applications"
 *   NREL Technical Report NREL/TP-560-34302
 *   National Renewable Energy Laboratory, Solar Radiation Research Laboratory
 */
enum class LongitudeAscendingNodeFormulation { NOAA_LINEAR, REDA_ANDREAS_SPA };

/**
 * Main class for sunset calculations.
 * All calculations are done internally; results are returned as simple numeric
 * values suitable for embedded systems.
 */
class SunsetCalculator {
 public:
  /**
   * Calculate sunset time for a given date and location.
   *
   * @param year           - Calendar year (e.g., 2026)
   * @param month          - Month (1-12)
   * @param day            - Day of month (1-31)
   * @param latitude       - Geographic latitude in degrees (-90 to 90)
   * @param longitude      - Geographic longitude in degrees (-180 to 180)
   * @param timezone       - UTC offset in hours (e.g., -5 for EST)
   * @param altitude_meters - (optional) Observer altitude above sea level in
   * meters (default: 0)
   * @param out_solarNoon  - (optional) Pointer to store calculated solar noon
   * @param out_delta      - (optional) Pointer to store solar declination
   *
   * @return Sunset time as decimal hours (e.g., 17.25 = 5:15 PM local time)
   * @note Higher altitude extends sunset time (geometric horizon is lower)
   */
  double getSunset(
      int year, int month, int day, double latitude, double longitude,
      int timezone,
      double altitude_meters = config::location::kDefaultObserverAltitude,
      double* out_solarNoon = nullptr, double* out_delta = nullptr);

  /**
   * Calculate sunrise time for a given date and location.
   *
   * @param year           - Calendar year (e.g., 2026)
   * @param month          - Month (1-12)
   * @param day            - Day of month (1-31)
   * @param latitude       - Geographic latitude in degrees (-90 to 90)
   * @param longitude      - Geographic longitude in degrees (-180 to 180)
   * @param timezone       - UTC offset in hours (e.g., -5 for EST)
   * @param altitude_meters - (optional) Observer altitude above sea level in
   * meters (default: 0)
   * @param out_solarNoon  - (optional) Pointer to store calculated solar noon
   * @param out_delta      - (optional) Pointer to store solar declination
   *
   * @return Sunrise time as decimal hours (e.g., 6.5 = 6:30 AM local time)
   * @note Higher altitude extends sunrise time (sun appears earlier from
   * elevated locations)
   */
  double getSunrise(
      int year, int month, int day, double latitude, double longitude,
      int timezone,
      double altitude_meters = config::location::kDefaultObserverAltitude,
      double* out_solarNoon = nullptr, double* out_delta = nullptr);

  /**
   * Convert decimal hours to hours, minutes, seconds.
   *
   * @param hours      - Decimal hours (e.g., 17.25)
   * @param outHours   - Pointer to store hour component
   * @param outMinutes - Pointer to store minute component
   * @param outSeconds - Pointer to store second component
   */
  static void decimalHoursToHMS(double hours, int& outHours, int& outMinutes,
                                int& outSeconds);

  /**
   * Convert decimal hours to a time string.
   * Requires sprintf; for Arduino, consider decimalHoursToHMS() instead.
   *
   * @param hours           - Decimal hours
   * @param includeMinutes  - Include minutes in output
   * @param includeSeconds  - Include seconds in output
   * @param buffer          - Output buffer (must be at least 12 chars for full
   *                          format)
   * @param bufferSize      - Size of output buffer
   */
  static void decimalHoursToString(double hours, char* buffer, int bufferSize,
                                   bool includeMinutes = true,
                                   bool includeSeconds = true);

  /**
   * Validate input parameters for sunset/sunrise calculation.
   * Returns true if all inputs are within valid ranges.
   *
   * @param year      - Calendar year (valid: 1900-2100)
   * @param month     - Month (valid: 1-12)
   * @param day       - Day of month (valid: 1-31)
   * @param latitude  - Geographic latitude (valid: -90 to 90)
   * @param longitude - Geographic longitude (valid: -180 to 180)
   * @param timezone  - UTC offset (valid: -12 to 14)
   *
   * @return True if all parameters are valid, false otherwise
   */
  static bool validateInputs(int year, int month, int day, double latitude,
                             double longitude, int timezone);

  /**
   * Educational helper functions - public to support demonstrations.
   * These are used by main/sunset.cpp to show calculation steps.
   * Arduino users can call these if studying the algorithm, but the
   * recommended API is getSunset() and getSunrise().
   */

  // Date/time conversion helpers
  double getJulianDate(int year, int month, int day);
  double getJ2000(double jd);
  double getJulianCentury(double J2000);

  // Orbital mechanics helpers
  double eccentricity(double t);
  double hourAngle(double h0, double phi, double delta);

  // Algorithm selection helpers - used by pedagogical sunset.cpp
  double meanLongitude(double t, Algorithm algo = Algorithm::NOAA);
  double meanAnomaly(double t, Algorithm algo = Algorithm::NOAA);
  double equationOfCenter(double t, double M, Algorithm algo = Algorithm::NOAA);
  double obliquityOfEcliptic(double T, Algorithm algo = Algorithm::NOAA);

  /**
   * Calculate longitude of the ascending node of the Moon's mean orbit.
   * Used for nutation and aberration corrections.
   *
   * Two formulations available:
   * - NOAA_LINEAR: Simple linear form (lower precision, faster)
   * - REDA_ANDREAS_SPA: Cubic polynomial (higher precision, production
   * recommended)
   *
   * References:
   * - NOAA: https://www.esrl.noaa.gov/gmd/grad/solcalc/
   * - Reda, I., & Andreas, A. (2008). Solar position algorithm for solar
   * radiation applications. NREL Technical Report NREL/TP-560-34302. National
   * Renewable Energy Laboratory.
   *
   * @param t Julian centuries since J2000.0
   * @param form Formulation to use (defaults to REDA_ANDREAS_SPA for higher
   * precision)
   * @return Longitude in radians
   */
  double longitudeAscendingNode(
      double t, LongitudeAscendingNodeFormulation form =
                    LongitudeAscendingNodeFormulation::REDA_ANDREAS_SPA);

 private:
  // Astronomical constants
  static constexpr double kJ2000Epoch = 2451545.0;
  static constexpr double kMinutesPerHour = 60.0;

  // Helper calculation functions (internal use only)
  double nutationInLongitude(
      double Omega, double JCE, double X1,
      LongitudeAscendingNodeFormulation form =
          LongitudeAscendingNodeFormulation::REDA_ANDREAS_SPA);
  double radiusVector(double e, double nu);
  double equationOfTime(double M, double RA, double DPsi, double epsilon,
                        double L);
  double getZenith(
      double e, double nu,
      double altitude_meters = config::location::kDefaultObserverAltitude);
  double getSolarNoon(double longitude, int timezone);

  // Internal calculation state (for chaining operations)
  bool use_NOAA_algorithm = true;
};

}  // namespace sunset_calc

#endif  // SUNSET_CALC_H
