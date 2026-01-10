/**
 * Twilight - Solar event table
 *
 * Displays a table of solar events with times and relative countdowns
 */

#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "config.h"
#include "constants.h"
#include "sunset_calc.h"

using namespace std;
using namespace astro;
using namespace config;

// ANSI color codes matching twilight_table
namespace Colors {
const string ASTRONOMICAL = "\033[38;5;19m";   // Dark blue
const string NAUTICAL = "\033[38;5;33m";       // Medium blue
const string CIVIL = "\033[38;5;75m";          // Light blue
const string SUNRISE = "\033[38;5;220m";       // Yellow
const string SOLAR_NOON = "\033[38;5;226m";    // Bright yellow
const string GOLDEN_START = "\033[38;5;214m";  // Orange
const string SUNSET = "\033[38;5;208m";        // Dark orange
const string GOLDEN_END = "\033[38;5;202m";    // Red-orange
const string RESET = "\033[0m";
const string BOLD = "\033[1m";
}  // namespace Colors

// Event data structure
struct SolarEvent {
  string label;
  double sunAngle;
  double zenith;
  string colorCode;
  bool isMorning;  // true = use solarNoon - HA, false = use solarNoon + HA
  bool isNoon;     // special case for solar noon
};

// Convert decimal hours to HH:MM
static string formatTime(double fhr) {
  if (fhr < 0) return "--:--";
  int hr = static_cast<int>(floor(fhr));
  int min = static_cast<int>(floor((fhr - hr) * 60));
  ostringstream oss;
  oss << setfill('0') << setw(2) << hr << ":" << setw(2) << min;
  return oss.str();
}

// Format relative time
static string formatRelative(double timeDiff) {
  bool isLate = timeDiff < 0;
  double absTime = abs(timeDiff);
  int hours = static_cast<int>(absTime);
  int mins = static_cast<int>((absTime - hours) * 60);

  ostringstream oss;
  oss << (isLate ? "+" : "-") << setfill('0') << setw(2) << hours << ":"
      << setw(2) << mins;
  return oss.str();
}

// Calculate hour angle
static double calcHourAngle(double zenithAngle, double latitude, double delta) {
  double h0 = zenithAngle * kDeg2Rad;
  double phi = latitude * kDeg2Rad;
  double d = delta * kDeg2Rad;

  double cosH = (cos(h0) - sin(phi) * sin(d)) / (cos(phi) * cos(d));
  if (cosH < -1.0 || cosH > 1.0) return -1.0;

  return acos(cosH) * kRad2Deg;
}

// Convert sun angle to zenith
static double sunAngleToZenith(double sunAngle) {
  if (sunAngle < 0) {
    return 90.0 - sunAngle;
  } else {
    return 90.0 + sunAngle;
  }
}

// Column widths
namespace ColWidth {
const int ANGLE = 4;
const int EVENT = 30;
const int TIME = 5;
const int RELATIVE = 7;
}  // namespace ColWidth

// Generate repeated string
static string repeatStr(const string& s, int n) {
  string result;
  for (int i = 0; i < n; ++i) result += s;
  return result;
}

// Right-align with padding
static string padRight(const string& s, int displayWidth, int extraBytes = 0) {
  int padding = displayWidth - (s.length() - extraBytes);
  if (padding <= 0) return s;
  return string(padding, ' ') + s;
}

int main() {
  // Get current time
  time_t now = time(nullptr);
  tm ltm;
#ifdef _WIN32
  localtime_s(&ltm, &now);
#else
  localtime_r(&now, &ltm);
#endif

  int year = ltm.tm_year + 1900;
  int month = ltm.tm_mon + 1;
  int day = ltm.tm_mday;
  double currentTime = ltm.tm_hour + ltm.tm_min / 60.0 + ltm.tm_sec / 3600.0;

  // Get location from config
  double latitude = config::location::kDefaultLatitude;
  double longitude = config::location::kDefaultLongitude;
  int timezone = config::location::kDefaultTimezone;

  // Calculate using library
  sunset_calc::SunsetCalculator calc;
  double solarNoon, delta;
  double sunsetTime = calc.getSunset(year, month, day, latitude, longitude,
                                     timezone, &solarNoon, &delta);

  if (sunsetTime >= 24.0) {
    cerr << "ERROR: Invalid calculation" << endl;
    return 1;
  }

  // Define events in chronological order
  SolarEvent events[] = {
      {"Astronomical twilight starts", -18.0, sunAngleToZenith(18.0),
       Colors::ASTRONOMICAL, true, false},
      {"Nautical twilight starts", -12.0, sunAngleToZenith(12.0),
       Colors::NAUTICAL, true, false},
      {"Civil twilight starts", -6.0, sunAngleToZenith(6.0), Colors::CIVIL,
       true, false},
      {"Sunrise", 0.0, 90.0 - kStandardSunsetElevation, Colors::SUNRISE, true,
       false},
      {"Solar noon", 0.0, 0.0, Colors::SOLAR_NOON, false, true},
      {"Sunset", 0.0, 90.0 - kStandardSunsetElevation, Colors::SUNSET, false,
       false},
      {"Golden hour starts", -6.0, sunAngleToZenith(-6.0), Colors::GOLDEN_START,
       false, false},
      {"Golden hour ends", 4.0, sunAngleToZenith(4.0), Colors::GOLDEN_END,
       false, false},
      {"Civil twilight ends", 6.0, sunAngleToZenith(6.0), Colors::CIVIL, false,
       false},
      {"Nautical twilight ends", 12.0, sunAngleToZenith(12.0), Colors::NAUTICAL,
       false, false},
      {"Astronomical twilight ends", 18.0, sunAngleToZenith(18.0),
       Colors::ASTRONOMICAL, false, false}};

  // Build table borders
  string topBorder = "┌─" + repeatStr("─", ColWidth::ANGLE + 1) + "─┬─" +
                     repeatStr("─", ColWidth::EVENT) + "─┬─" +
                     repeatStr("─", ColWidth::TIME) + "─┬─" +
                     repeatStr("─", ColWidth::RELATIVE) + "─┐";
  string midBorder = "├─" + repeatStr("─", ColWidth::ANGLE + 1) + "─┼─" +
                     repeatStr("─", ColWidth::EVENT) + "─┼─" +
                     repeatStr("─", ColWidth::TIME) + "─┼─" +
                     repeatStr("─", ColWidth::RELATIVE) + "─┤";
  string botBorder = "└─" + repeatStr("─", ColWidth::ANGLE + 1) + "─┴─" +
                     repeatStr("─", ColWidth::EVENT) + "─┴─" +
                     repeatStr("─", ColWidth::TIME) + "─┴─" +
                     repeatStr("─", ColWidth::RELATIVE) + "─┘";

  // Print header
  cout << "\nSolar Events - " << put_time(&ltm, "%B %d, %Y") << endl;
  cout << "Location: " << latitude << "°N, " << longitude << "°W" << endl;
  cout << "Current time: " << put_time(&ltm, "%H:%M:%S") << endl;

  cout << endl;
  cout << Colors::BOLD << topBorder << Colors::RESET << endl;
  cout << Colors::BOLD << "│ " << left << setw(ColWidth::ANGLE + 1) << "Angle"
       << " │ " << setw(ColWidth::EVENT) << "Event"
       << " │ " << setw(ColWidth::TIME) << "Time"
       << " │ " << setw(ColWidth::RELATIVE) << "Relative"
       << " │" << Colors::RESET << endl;
  cout << Colors::BOLD << midBorder << Colors::RESET << endl;

  // Print events
  for (const auto& event : events) {
    double eventTime;

    // Format angle
    ostringstream angleOss;
    if (event.isNoon) {
      angleOss << "  --";
      eventTime = solarNoon;
    } else {
      if (event.sunAngle >= 0) {
        angleOss << "+" << fixed << setprecision(0) << event.sunAngle << "°";
      } else {
        angleOss << fixed << setprecision(0) << event.sunAngle << "°";
      }

      double HA_deg = calcHourAngle(event.zenith, latitude, delta);

      if (HA_deg < 0) {
        // Event doesn't occur
        string angleStr = padRight(angleOss.str(), ColWidth::ANGLE + 1, 1);
        cout << "│ " << event.colorCode << angleStr << Colors::RESET << " │ "
             << event.colorCode << left << setw(ColWidth::EVENT) << event.label
             << Colors::RESET << " │ " << event.colorCode
             << setw(ColWidth::TIME) << "--:--" << Colors::RESET << " │ "
             << event.colorCode << right << setw(ColWidth::RELATIVE) << "N/A"
             << Colors::RESET << " │" << endl;
        continue;
      }

      double HA = HA_deg / 15.0;
      eventTime = event.isMorning ? solarNoon - HA : solarNoon + HA;
    }

    string angleStr = padRight(angleOss.str(), ColWidth::ANGLE + 1, 1);
    double timeDiff = eventTime - currentTime;
    string relativeStr = formatRelative(timeDiff);

    cout << "│ " << event.colorCode << angleStr << Colors::RESET << " │ "
         << event.colorCode << left << setw(ColWidth::EVENT) << event.label
         << Colors::RESET << " │ " << event.colorCode << setw(ColWidth::TIME)
         << formatTime(eventTime) << Colors::RESET << " │ " << event.colorCode
         << right << setw(ColWidth::RELATIVE) << relativeStr << Colors::RESET
         << " │" << endl;
  }

  cout << Colors::BOLD << botBorder << Colors::RESET << endl;
  cout << endl;

  return 0;
}
