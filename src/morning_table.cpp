/**
 * Morning Twilight Table - Color-coded display of dawn events
 *
 * Displays a table showing departure times, ETAs, and arrival times
 * for morning twilight events, color-coded from astronomical twilight
 * (dark blue) through golden hour (warm tones).
 */

#include "morning_table.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "colors.h"
#include "constants.h"
#include "format_utils.h"
#include "solar_utils.h"
#include "table_layout.h"
#include "text_utils.h"

using namespace std;
using namespace astro;

// Structure for morning event data
struct MorningEvent {
  string label;
  double sunAngle;  // Degrees relative to horizon (negative = above)
  double zenith;    // Zenith angle for calculation (90 + sunAngle)
  string colorCode;
};

using namespace table_layout::col;
using table_layout::sunset::DEPARTURE;
using table_layout::sunset::LEAVING;

void printMorningTable(double solarNoon, double latitude, double delta,
                       double currentTime, double commuteMinutes,
                       double workdayHours) {
  // Define morning/sunset events
  // Convention: sunAngle < 0 => above horizon; sunAngle > 0 => below
  vector<MorningEvent> events = {
      {"Astronomical twilight begins", 18.0,
       solar_utils::sunAngleToZenith(18.0), Colors::ASTRONOMICAL},
      {"Nautical twilight begins", 12.0, solar_utils::sunAngleToZenith(12.0),
       Colors::NAUTICAL},
      {"Civil twilight begins", 6.0, solar_utils::sunAngleToZenith(6.0),
       Colors::CIVIL},
      {"Golden hour starts", 4.0, solar_utils::sunAngleToZenith(4.0),
       Colors::GOLDEN_START},
      {"Sunrise", 0.0, 90.0 - kStandardSunsetElevation, Colors::SUNSET},
      {"Golden hour ends", -6.0, solar_utils::sunAngleToZenith(-6.0),
       Colors::GOLDEN_END},
  };

  double commuteHours = commuteMinutes / kMinutesPerHour;
  double doubleCommuteHours = 2.0 * commuteHours;

  // Borders (add 1 to ANGLE for the ° symbol which is 2-byte UTF-8)
  string topBorder = "┌─" + text_utils::repeatStr("─", ANGLE + 1) + "─┬─" +
                     text_utils::repeatStr("─", EVENT) + "─┬─" +
                     text_utils::repeatStr("─", TIME) + "─┬─" +
                     text_utils::repeatStr("─", LEAVING) + "─┬─" +
                     text_utils::repeatStr("─", DEPARTURE) + "─┐";
  string midBorder = "├─" + text_utils::repeatStr("─", ANGLE + 1) + "─┼─" +
                     text_utils::repeatStr("─", EVENT) + "─┼─" +
                     text_utils::repeatStr("─", TIME) + "─┼─" +
                     text_utils::repeatStr("─", LEAVING) + "─┼─" +
                     text_utils::repeatStr("─", DEPARTURE) + "─┤";
  string botBorder = "└─" + text_utils::repeatStr("─", ANGLE + 1) + "─┴─" +
                     text_utils::repeatStr("─", EVENT) + "─┴─" +
                     text_utils::repeatStr("─", TIME) + "─┴─" +
                     text_utils::repeatStr("─", LEAVING) + "─┴─" +
                     text_utils::repeatStr("─", DEPARTURE) + "─┘";

  // ===== MORNING ARRIVAL SECTION =====
  cout << endl;
  cout << Colors::BOLD << "MORNING ARRIVAL (Single Commute)" << Colors::RESET
       << endl;
  cout << Colors::BOLD << topBorder << Colors::RESET << endl;
  cout << Colors::BOLD << "│ " << left << setw(ANGLE + 1) << "Angle"
       << " │ " << setw(EVENT) << "Event"
       << " │ " << setw(TIME) << "Time"
       << " │ " << setw(LEAVING) << "Leave"
       << " │ " << setw(DEPARTURE) << "Dept"
       << " │" << Colors::RESET << endl;
  cout << Colors::BOLD << midBorder << Colors::RESET << endl;

  // Morning rows (events before solar noon)
  for (const auto& event : events) {
    double HA_deg = solar_utils::calcHourAngle(event.zenith, latitude, delta);

    // Format angle with sign and degree symbol
    ostringstream angleOss;
    if (event.sunAngle >= 0) {
      angleOss << "+" << fixed << setprecision(0) << event.sunAngle << "°";
    } else {
      angleOss << fixed << setprecision(0) << event.sunAngle << "°";
    }
    string angleStr = text_utils::padRight(angleOss.str(), ANGLE + 1, 1);

    if (HA_deg < 0) {
      cout << "│ " << event.colorCode << angleStr << Colors::RESET << " │ "
           << event.colorCode << left << setw(EVENT) << event.label
           << Colors::RESET << " │ " << event.colorCode << setw(TIME) << "--:--"
           << Colors::RESET << " │ " << event.colorCode << right
           << setw(LEAVING) << "N/A" << Colors::RESET << " │ "
           << event.colorCode << setw(DEPARTURE) << "--:--" << Colors::RESET
           << " │" << endl;
      continue;
    }

    double HA = HA_deg / 15.0;
    double eventTime = solarNoon - HA;  // morning events occur before noon
    double departureTime = eventTime - commuteHours;
    double timeToDepart = departureTime - currentTime;

    string countdownStr = format_utils::formatSignedHHMM(timeToDepart);
    string departureStr =
        (timeToDepart < 0) ? " PAST" : format_utils::formatHHMM(departureTime);

    cout << "│ " << event.colorCode << angleStr << Colors::RESET << " │ "
         << event.colorCode << left << setw(EVENT) << event.label
         << Colors::RESET << " │ " << event.colorCode << setw(TIME)
         << format_utils::formatHHMM(eventTime) << Colors::RESET << " │ "
         << event.colorCode << right << setw(LEAVING) << countdownStr
         << Colors::RESET << " │ " << event.colorCode << setw(DEPARTURE)
         << departureStr << Colors::RESET << " │" << endl;
  }

  cout << Colors::BOLD << botBorder << Colors::RESET << endl;
  cout << endl;

  // ===== EVENING DEPARTURE SECTION =====
  // Mirror the morning events for sunset times with double commute + workday
  // These are afternoon events (after solar noon)
  vector<MorningEvent> eveningEvents = {
      {"Golden hour ends", -6.0, solar_utils::sunAngleToZenith(-6.0),
       Colors::GOLDEN_END},
      {"Sunset", 0.0, 90.0 - kStandardSunsetElevation, Colors::SUNSET},
      {"Golden hour starts", 4.0, solar_utils::sunAngleToZenith(4.0),
       Colors::GOLDEN_START},
      {"Civil twilight ends", 6.0, solar_utils::sunAngleToZenith(6.0),
       Colors::CIVIL},
      {"Nautical twilight ends", 12.0, solar_utils::sunAngleToZenith(12.0),
       Colors::NAUTICAL},
      {"Astronomical twilight ends", 18.0, solar_utils::sunAngleToZenith(18.0),
       Colors::ASTRONOMICAL}};

  cout << Colors::BOLD << "EVENING DEPARTURE (Home Commute: 2× Commute + "
       << fixed << setprecision(1) << workdayHours << "-hour Work Day)"
       << Colors::RESET << endl;
  cout << Colors::BOLD << topBorder << Colors::RESET << endl;
  cout << Colors::BOLD << "│ " << left << setw(ANGLE + 1) << "Angle"
       << " │ " << setw(EVENT) << "Event"
       << " │ " << setw(TIME) << "Time"
       << " │ " << setw(LEAVING) << "Leave"
       << " │ " << setw(DEPARTURE) << "Dept"
       << " │" << Colors::RESET << endl;
  cout << Colors::BOLD << midBorder << Colors::RESET << endl;

  // Evening rows (events after solar noon)
  for (const auto& event : eveningEvents) {
    double HA_deg = solar_utils::calcHourAngle(event.zenith, latitude, delta);

    // Format angle with sign and degree symbol
    ostringstream angleOss;
    if (event.sunAngle >= 0) {
      angleOss << "+" << fixed << setprecision(0) << event.sunAngle << "°";
    } else {
      angleOss << fixed << setprecision(0) << event.sunAngle << "°";
    }
    string angleStr = text_utils::padRight(angleOss.str(), ANGLE + 1, 1);

    if (HA_deg < 0) {
      cout << "│ " << event.colorCode << angleStr << Colors::RESET << " │ "
           << event.colorCode << left << setw(EVENT) << event.label
           << Colors::RESET << " │ " << event.colorCode << setw(TIME) << "--:--"
           << Colors::RESET << " │ " << event.colorCode << right
           << setw(LEAVING) << "N/A" << Colors::RESET << " │ "
           << event.colorCode << setw(DEPARTURE) << "--:--" << Colors::RESET
           << " │" << endl;
      continue;
    }

    double HA = HA_deg / 15.0;
    double eventTime = solarNoon + HA;  // evening events occur after noon

    // Departure time: leave workday at event time - 2x commute
    // (i.e., event time - workday - 2x commute from start of workday)
    double workdayHoursToDepart = doubleCommuteHours + workdayHours;
    double departureTime = eventTime - workdayHoursToDepart;
    double timeToDepart = departureTime - currentTime;

    string countdownStr = format_utils::formatSignedHHMM(timeToDepart);
    string departureStr =
        (timeToDepart < 0) ? " PAST" : format_utils::formatHHMM(departureTime);

    cout << "│ " << event.colorCode << angleStr << Colors::RESET << " │ "
         << event.colorCode << left << setw(EVENT) << event.label
         << Colors::RESET << " │ " << event.colorCode << setw(TIME)
         << format_utils::formatHHMM(eventTime) << Colors::RESET << " │ "
         << event.colorCode << right << setw(LEAVING) << countdownStr
         << Colors::RESET << " │ " << event.colorCode << setw(DEPARTURE)
         << departureStr << Colors::RESET << " │" << endl;
  }

  // Footer
  cout << Colors::BOLD << botBorder << Colors::RESET << endl;
  cout << endl;
}
