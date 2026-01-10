/**
 * Arduino Example: Sunset Time Calculator
 *
 * This example shows how to use the sunset_calc library in an Arduino project.
 * It demonstrates calculating sunset time and converting to hours/minutes.
 *
 * Usage on Arduino:
 *   1. Copy sunset_calc.h and sunset_calc.cpp to your project
 *   2. Copy constants.h (needed for kDeg2Rad and kRad2Deg)
 *   3. Include "sunset_calc.h" in your sketch
 *   4. Create a SunsetCalculator instance
 *   5. Call getSunset() with your date, location, and timezone
 */

#include "sunset_calc.h"

using namespace sunset_calc;

// Example for the Arduino Wemos D1 Mini with 7-segment displays
// Location: New York area
// Time source: NTP (synced via WiFi)

class SunsetDisplay {
 private:
  SunsetCalculator calc;
  double latitude = 40.7128;    // New York latitude
  double longitude = -74.0060;  // New York longitude
  int timezone_offset = -5;     // EST (UTC-5)

 public:
  /**
   * Calculate sunset time for today
   * @param year - Current year
   * @param month - Current month (1-12)
   * @param day - Current day (1-31)
   * @return Sunset time as decimal hours
   */
  double getSunsetTimeToday(int year, int month, int day) {
    return calc.getSunset(year, month, day, latitude, longitude,
                          timezone_offset);
  }

  /**
   * Calculate time remaining until sunset
   * @param year - Current year
   * @param month - Current month (1-12)
   * @param day - Current day (1-31)
   * @param currentHour - Current hour (0-23)
   * @param currentMinute - Current minute (0-59)
   * @return Time remaining in decimal hours
   */
  double getTimeUntilSunset(int year, int month, int day, int currentHour,
                            int currentMinute) {
    double sunset = getSunsetTimeToday(year, month, day);
    double currentTime = currentHour + currentMinute / 60.0;
    return sunset - currentTime;
  }

  /**
   * Format sunset time for display
   * @param sunsetHours - Sunset time (decimal hours)
   * @param outHour - Pointer to store hour (0-23)
   * @param outMinute - Pointer to store minute (0-59)
   */
  void formatSunsetForDisplay(double sunsetHours, int& outHour,
                              int& outMinute) {
    int h, m, s;
    SunsetCalculator::decimalHoursToHMS(sunsetHours, h, m, s);
    outHour = h;
    outMinute = m;
  }

  /**
   * Get sunrise time (same method used by sunset calculation)
   * @param year - Current year
   * @param month - Current month (1-12)
   * @param day - Current day (1-31)
   * @return Sunrise time as decimal hours
   */
  double getSunriseTimeToday(int year, int month, int day) {
    return calc.getSunrise(year, month, day, latitude, longitude,
                           timezone_offset);
  }

  /**
   * Check if it's currently golden hour
   * Golden hour: 1 hour before and after sunset
   */
  bool isGoldenHour(int year, int month, int day, int currentHour,
                    int currentMinute) {
    double sunset = getSunsetTimeToday(year, month, day);
    double currentTime = currentHour + currentMinute / 60.0;

    // Golden hour is from sunset-1 to sunset+1
    return (currentTime > (sunset - 1.0)) && (currentTime < (sunset + 1.0));
  }
};

// ============================================================================
// Desktop Example (for testing before deploying to Arduino)
// ============================================================================

#ifdef DESKTOP_TEST

#include <ctime>
#include <iomanip>
#include <iostream>

int main() {
  SunsetDisplay display;

  // Get current date/time
  std::time_t now = std::time(nullptr);
  std::tm* ltm = std::localtime(&now);

  int year = ltm->tm_year + 1900;
  int month = ltm->tm_mon + 1;
  int day = ltm->tm_mday;
  int hour = ltm->tm_hour;
  int minute = ltm->tm_min;

  // Calculate sunset
  double sunset = display.getSunsetTimeToday(year, month, day);
  double sunrise = display.getSunriseTimeToday(year, month, day);
  double timeUntilSunset =
      display.getTimeUntilSunset(year, month, day, hour, minute);

  // Format for display
  int sunsetHour, sunsetMinute;
  display.formatSunsetForDisplay(sunset, sunsetHour, sunsetMinute);

  // Output
  std::cout << "=== Sunset Display Example ===" << std::endl;
  std::cout << "Date: " << year << "-" << std::setfill('0') << std::setw(2)
            << month << "-" << std::setw(2) << day << std::endl;
  std::cout << "Current time: " << std::setw(2) << hour << ":" << std::setw(2)
            << minute << std::endl;
  std::cout << std::endl;
  std::cout << "Sunrise: ";
  int rh, rm, rs;
  SunsetCalculator::decimalHoursToHMS(sunrise, rh, rm, rs);
  std::cout << std::setw(2) << rh << ":" << std::setw(2) << rm << std::endl;

  std::cout << "Sunset:  " << std::setw(2) << sunsetHour << ":" << std::setw(2)
            << sunsetMinute << std::endl;
  std::cout << "Time until sunset: " << std::fixed << std::setprecision(2)
            << timeUntilSunset << " hours" << std::endl;

  std::cout << std::endl;
  std::cout << "Golden hour now: "
            << (display.isGoldenHour(year, month, day, hour, minute) ? "YES"
                                                                     : "NO")
            << std::endl;

  return 0;
}

#endif  // DESKTOP_TEST
