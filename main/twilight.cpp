/**
 * Twilight - Solar event table
 *
 * Displays a table of solar events with times and relative countdowns
 */

#include <array>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "colors.h"
#include "config_location.h"
#include "constants.h"
#include "format_utils.h"
#include "solar_utils.h"
#include "sunset_calc.h"
#include "table_layout.h"
#include "table_render.h"
#include "time_init.h"

using namespace std;
using namespace astro;
using namespace config;

// Event data structure (extends SolarEvent with morning/noon flags)
struct TwilightEvent {
  string label;
  double sunAngle;
  double zenith;
  string colorCode;
  bool isMorning;  // true = use solarNoon - HA, false = use solarNoon + HA
  bool isNoon;     // special case for solar noon
};

using namespace table_layout::col;
using table_layout::twilight::RELATIVE;

int main() {
  tm ltm;
  time_init::getLocalTime(ltm);
  int year, month, day;
  time_init::getDate(ltm, year, month, day);
  double currentTime = time_init::toFractionalHours(ltm);

  double latitude = config::location::kDefaultLatitude;
  double longitude = config::location::kDefaultLongitude;
  int timezone = config::location::kDefaultTimezone;

  sunset_calc::SunsetCalculator calc;
  double solarNoon, delta;
  double sunsetTime = calc.getSunset(
      year, month, day, latitude, longitude, timezone,
      config::location::kDefaultObserverAltitude, &solarNoon, &delta);

  if (sunsetTime >= 24.0) {
    cerr << "ERROR: Invalid calculation" << endl;
    return 1;
  }

  std::array<TwilightEvent, 11> events = {
      {{"Astronomical twilight starts", -18.0,
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
        Colors::ASTRONOMICAL, false, false}}};

  // Build table borders using shared utility
  vector<int> cols = {ANGLE + 1, EVENT, TIME, RELATIVE};
  auto borders = table_render::makeBorders(cols);

  cout << "\nSolar Events - " << put_time(&ltm, "%B %d, %Y") << endl;
  cout << "Location: " << latitude << "°N, " << longitude << "°W" << endl;
  cout << "Current time: " << put_time(&ltm, "%H:%M:%S") << endl;

  cout << endl;
  cout << Colors::BOLD << borders.top << Colors::RESET << endl;
  cout << Colors::BOLD << "│ " << left << setw(ANGLE + 1) << "Angle"
       << " │ " << setw(EVENT) << "Event"
       << " │ " << setw(TIME) << "Time"
       << " │ " << setw(RELATIVE) << "Rel."
       << " │" << Colors::RESET << endl;
  cout << Colors::BOLD << borders.mid << Colors::RESET << endl;

  for (const auto& event : events) {
    double eventTime;

    string angleStr;
    if (event.isNoon) {
      angleStr = text_utils::padRight("  --", ANGLE + 1, 0);
      eventTime = solarNoon;
    } else {
      angleStr = table_render::formatAngle(event.sunAngle);

      double HA_deg = solar_utils::calcHourAngle(event.zenith, latitude, delta);

      if (HA_deg < 0) {
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

    double timeDiff = eventTime - currentTime;
    string relativeStr = format_utils::formatSignedHHMM(timeDiff);

    cout << "│ " << event.colorCode << angleStr << Colors::RESET << " │ "
         << event.colorCode << left << setw(EVENT) << event.label
         << Colors::RESET << " │ " << event.colorCode << setw(TIME)
         << format_utils::formatHHMM(eventTime) << Colors::RESET << " │ "
         << event.colorCode << right << setw(RELATIVE) << relativeStr
         << Colors::RESET << " │" << endl;
  }

  cout << Colors::BOLD << borders.bot << Colors::RESET << endl;
  cout << endl;

  return 0;
}
