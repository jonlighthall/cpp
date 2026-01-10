/**
 * Twilight Table - Color-coded display of twilight events
 *
 * Displays a table showing departure times, ETAs, and arrival times
 * for various twilight events, color-coded from golden hour (orange)
 * through astronomical twilight (dark blue).
 */

#include "twilight_table.h"

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

// ANSI color codes unified in Colors (see include/colors.h)

// Structure for twilight event data
struct TwilightEvent {
  string label;
  double sunAngle;  // Degrees below horizon (negative = above)
  double zenith;    // Zenith angle for calculation (90 - sunAngle)
  string colorCode;
};

// Use shared solar utility (see include/solar_utils.h)

// Use shared calcHourAngle from solar_utils

// Use shared formatting and layout utilities (see include/*.h)
using namespace table_layout::col;
using table_layout::sunset::DEPARTURE;
using table_layout::sunset::LEAVING;

void printTwilightTable(double solarNoon, double latitude, double delta,
                        double currentTime, double commuteMinutes) {
  // Define twilight events using constants from constants.h
  // Sun angle: negative = above horizon, positive = below horizon
  // Zenith angles are derived from sun angles using: zenith = 90 - sunAngle
  //
  // NOTE: Sunset uses the standard zenith angle which includes:
  //   - Sun's angular radius: 0.26667°
  //   - Atmospheric refraction: 0.5667°
  //   - Total correction: 0.833° (kStandardSunsetElevation)
  //   - Zenith = 90° - (-0.833°) = 90.833°

  vector<TwilightEvent> events = {
      {"Golden hour starts", -6.0, solar_utils::sunAngleToZenith(-6.0),
       Colors::GOLDEN_START},
      {"Sunset", 0.0, 90.0 - kStandardSunsetElevation, Colors::SUNSET},
      {"Golden hour ends", 4.0, solar_utils::sunAngleToZenith(4.0),
       Colors::GOLDEN_END},
      {"Civil twilight ends", 6.0, solar_utils::sunAngleToZenith(6.0),
       Colors::CIVIL},
      {"Nautical twilight ends", 12.0, solar_utils::sunAngleToZenith(12.0),
       Colors::NAUTICAL},
      {"Astronomical twilight ends", 18.0, solar_utils::sunAngleToZenith(18.0),
       Colors::ASTRONOMICAL}};

  double commuteHours = commuteMinutes / kMinutesPerHour;

  // Build border strings based on column widths
  // Add 1 to ANGLE for the ° symbol (2-byte UTF-8)
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

  // Print table header
  cout << endl;
  cout << Colors::BOLD << topBorder << Colors::RESET << endl;
  cout << Colors::BOLD << "│ " << left << setw(ANGLE + 1) << "Angle"
       << " │ " << setw(EVENT) << "Event"
       << " │ " << setw(TIME) << "Time"
       << " │ " << setw(LEAVING) << "Leave"
       << " │ " << setw(DEPARTURE) << "Dept"
       << " │" << Colors::RESET << endl;
  cout << Colors::BOLD << midBorder << Colors::RESET << endl;

  // Calculate and print each event
  for (const auto& event : events) {
    double HA_deg = solar_utils::calcHourAngle(event.zenith, latitude, delta);

    // Format angle with sign (° is 2 bytes UTF-8, so add 1 to display width)
    ostringstream angleOss;
    if (event.sunAngle >= 0) {
      angleOss << "+" << fixed << setprecision(0) << event.sunAngle << "°";
    } else {
      angleOss << fixed << setprecision(0) << event.sunAngle << "°";
    }
    // Pad manually for UTF-8: display width is ANGLE+1, extra byte is 1
    string angleStr = text_utils::padRight(angleOss.str(), ANGLE + 1, 1);

    if (HA_deg < 0) {
      // Event doesn't occur at this latitude/date
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
    double eventTime = solarNoon + HA;
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

  // Print table footer
  cout << Colors::BOLD << botBorder << Colors::RESET << endl;
  cout << endl;
}
