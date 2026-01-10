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

#include "colors.h"
#include "config.h"
#include "constants.h"
#include "format_utils.h"
#include "solar_utils.h"
#include "sunset_calc.h"
#include "table_layout.h"
#include "text_utils.h"

using namespace std;
using namespace astro;
using namespace config;

// ANSI color codes unified in Colors (see include/colors.h)

// Event data structure
struct SolarEvent {
  string label;
  double sunAngle;
  double zenith;
  string colorCode;
  bool isMorning;  // true = use solarNoon - HA, false = use solarNoon + HA
  bool isNoon;     // special case for solar noon
};

// Use shared utilities (see include/*.h)
using namespace table_layout::col;
using table_layout::twilight::RELATIVE;

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
      {"Astronomical twilight starts", -18.0,
       solar_utils::sunAngleToZenith(18.0), Colors::ASTRONOMICAL, true, false},
      {"Nautical twilight starts", -12.0, solar_utils::sunAngleToZenith(12.0),
       Colors::NAUTICAL, true, false},
      {"Civil twilight starts", -6.0, solar_utils::sunAngleToZenith(6.0),
       Colors::CIVIL, true, false},
      {"Sunrise", 0.0, 90.0 - kStandardSunsetElevation, Colors::SUNSET, true,
       false},
      {"Solar noon", 0.0, 0.0, Colors::SOLAR_NOON, false, true},
      {"Golden hour starts", -6.0, solar_utils::sunAngleToZenith(-6.0),
       Colors::GOLDEN_START, false, false},
      {"Sunset", 0.0, 90.0 - kStandardSunsetElevation, Colors::SUNSET, false,
       false},
      {"Golden hour ends", 4.0, solar_utils::sunAngleToZenith(4.0),
       Colors::GOLDEN_END, false, false},
      {"Civil twilight ends", 6.0, solar_utils::sunAngleToZenith(6.0),
       Colors::CIVIL, false, false},
      {"Nautical twilight ends", 12.0, solar_utils::sunAngleToZenith(12.0),
       Colors::NAUTICAL, false, false},
      {"Astronomical twilight ends", 18.0, solar_utils::sunAngleToZenith(18.0),
       Colors::ASTRONOMICAL, false, false}};

  // Build table borders
  string topBorder = "┌─" + text_utils::repeatStr("─", ANGLE + 1) + "─┬─" +
                     text_utils::repeatStr("─", EVENT) + "─┬─" +
                     text_utils::repeatStr("─", TIME) + "─┬─" +
                     text_utils::repeatStr("─", RELATIVE) + "─┐";
  string midBorder = "├─" + text_utils::repeatStr("─", ANGLE + 1) + "─┼─" +
                     text_utils::repeatStr("─", EVENT) + "─┼─" +
                     text_utils::repeatStr("─", TIME) + "─┼─" +
                     text_utils::repeatStr("─", RELATIVE) + "─┤";
  string botBorder = "└─" + text_utils::repeatStr("─", ANGLE + 1) + "─┴─" +
                     text_utils::repeatStr("─", EVENT) + "─┴─" +
                     text_utils::repeatStr("─", TIME) + "─┴─" +
                     text_utils::repeatStr("─", RELATIVE) + "─┘";

  // Print header
  cout << "\nSolar Events - " << put_time(&ltm, "%B %d, %Y") << endl;
  cout << "Location: " << latitude << "°N, " << longitude << "°W" << endl;
  cout << "Current time: " << put_time(&ltm, "%H:%M:%S") << endl;

  cout << endl;
  cout << Colors::BOLD << topBorder << Colors::RESET << endl;
  cout << Colors::BOLD << "│ " << left << setw(ANGLE + 1) << "Angle"
       << " │ " << setw(EVENT) << "Event"
       << " │ " << setw(TIME) << "Time"
       << " │ " << setw(RELATIVE) << "Rel."
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

      double HA_deg = solar_utils::calcHourAngle(event.zenith, latitude, delta);

      if (HA_deg < 0) {
        // Event doesn't occur
        string angleStr = text_utils::padRight(angleOss.str(), ANGLE + 1, 1);
        cout << "│ " << event.colorCode << angleStr << Colors::RESET << " │ "
             << event.colorCode << left << setw(EVENT) << event.label
             << Colors::RESET << " │ " << event.colorCode << setw(TIME)
             << "--:--" << Colors::RESET << " │ " << event.colorCode << right
             << setw(RELATIVE) << "N/A" << Colors::RESET << " │" << endl;
        continue;
      }

      double HA = HA_deg / 15.0;
      eventTime = event.isMorning ? solarNoon - HA : solarNoon + HA;
    }

    // Degree symbol is present for non-noon rows (extraBytes=1), but not for
    // noon (extraBytes=0)
    int extraBytes = event.isNoon ? 0 : 1;
    string angleStr =
        text_utils::padRight(angleOss.str(), ANGLE + 1, extraBytes);
    double timeDiff = eventTime - currentTime;
    string relativeStr = format_utils::formatSignedHHMM(timeDiff);

    cout << "│ " << event.colorCode << angleStr << Colors::RESET << " │ "
         << event.colorCode << left << setw(EVENT) << event.label
         << Colors::RESET << " │ " << event.colorCode << setw(TIME)
         << format_utils::formatHHMM(eventTime) << Colors::RESET << " │ "
         << event.colorCode << right << setw(RELATIVE) << relativeStr
         << Colors::RESET << " │" << endl;
  }

  cout << Colors::BOLD << botBorder << Colors::RESET << endl;
  cout << endl;

  return 0;
}
