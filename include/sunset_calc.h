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

namespace sunset_calc {

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
   * @param year      - Calendar year (e.g., 2026)
   * @param month     - Month (1-12)
   * @param day       - Day of month (1-31)
   * @param latitude  - Geographic latitude in degrees (-90 to 90)
   * @param longitude - Geographic longitude in degrees (-180 to 180)
   * @param timezone  - UTC offset in hours (e.g., -5 for EST)
   * @param out_solarNoon - (optional) Pointer to store calculated solar noon
   * @param out_delta     - (optional) Pointer to store solar declination
   *
   * @return Sunset time as decimal hours (e.g., 17.25 = 5:15 PM local time)
   */
  double getSunset(int year, int month, int day, double latitude,
                   double longitude, int timezone,
                   double* out_solarNoon = nullptr,
                   double* out_delta = nullptr);

  /**
   * Calculate sunrise time for a given date and location.
   *
   * @param year      - Calendar year (e.g., 2026)
   * @param month     - Month (1-12)
   * @param day       - Day of month (1-31)
   * @param latitude  - Geographic latitude in degrees (-90 to 90)
   * @param longitude - Geographic longitude in degrees (-180 to 180)
   * @param timezone  - UTC offset in hours (e.g., -5 for EST)
   * @param out_solarNoon - (optional) Pointer to store calculated solar noon
   * @param out_delta     - (optional) Pointer to store solar declination
   *
   * @return Sunrise time as decimal hours (e.g., 6.5 = 6:30 AM local time)
   */
  double getSunrise(int year, int month, int day, double latitude,
                    double longitude, int timezone,
                    double* out_solarNoon = nullptr,
                    double* out_delta = nullptr);

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

 private:
  // Astronomical constants
  static constexpr double kJ2000Epoch = 2451545.0;
  static constexpr double kMinutesPerHour = 60.0;

  // Helper calculation functions (all private, internal use only)
  double getJulianDate(int year, int month, int day);
  double getJ2000(double jd);
  double getJulianCentury(double J2000);
  double meanLongitude(double t);
  double meanAnomaly(double t);
  double equationOfCenter(double t, double M);
  double longitudeAscendingNode(double t);
  double nutationInLongitude(double Omega, double JCE, double X1);
  double eccentricity(double t);
  double radiusVector(double e, double nu);
  double obliquityOfEcliptic(double T);
  double equationOfTime(double M, double RA, double DPsi, double epsilon,
                        double L);
  double getZenith(double e, double nu);
  double hourAngle(double h0, double phi, double delta);
  double getSolarNoon(double longitude, int timezone);

  // Internal calculation state (for chaining operations)
  bool use_NOAA_algorithm = true;
};

}  // namespace sunset_calc

#endif  // SUNSET_CALC_H
