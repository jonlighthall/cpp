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

#include "constants.h"

using namespace std;
using namespace astro;

// ANSI color codes for terminal output
namespace Colors {
// Golden hour - warm oranges/yellows
const string GOLDEN_START = "\033[38;5;214m";  // Orange
const string SUNSET = "\033[38;5;208m";        // Dark orange
const string GOLDEN_END = "\033[38;5;202m";    // Red-orange

// Twilight - progressively darker blues
const string CIVIL = "\033[38;5;75m";         // Light blue
const string NAUTICAL = "\033[38;5;33m";      // Medium blue
const string ASTRONOMICAL = "\033[38;5;19m";  // Dark blue

// Reset
const string RESET = "\033[0m";

// Bold
const string BOLD = "\033[1m";
}  // namespace Colors

// Structure for twilight event data
struct TwilightEvent {
  string label;
  double sunAngle;  // Degrees below horizon (negative = above)
  double zenith;    // Zenith angle for calculation (90 - sunAngle)
  string colorCode;
};

// Helper: Convert sun angle (degrees below horizon) to zenith angle
// Relationship: zenith = 90 - sunAngle (derived from angle definitions)
static double sunAngleToZenith(double sunAngle) { return 90.0 - sunAngle; }

// Calculate hour angle for a given zenith angle
static double calcHourAngle(double zenithAngle, double latitude, double delta) {
  // Convert to radians
  double h0 = zenithAngle * kDeg2Rad;
  double phi = latitude * kDeg2Rad;
  double d = delta * kDeg2Rad;

  double cosH = (cos(h0) - sin(phi) * sin(d)) / (cos(phi) * cos(d));

  // Check if sun reaches this angle at this latitude
  if (cosH < -1.0 || cosH > 1.0) {
    return -1.0;  // Event doesn't occur
  }

  return acos(cosH) * kRad2Deg;
}

// Convert fractional hour to HH:MM string
static string formatTime(double fhr) {
  if (fhr < 0) return "--:--";

  int hr = static_cast<int>(floor(fhr));
  int min = static_cast<int>(floor((fhr - hr) * 60));

  ostringstream oss;
  oss << setfill('0') << setw(2) << hr << ":" << setw(2) << min;
  return oss.str();
}

// Convert time difference to compact format (-hh:mm or +hh:mm)
static string formatCountdown(double timeDiff) {
  bool isLate = timeDiff < 0;
  double absTime = std::abs(timeDiff);
  int hours = static_cast<int>(absTime);
  int mins = static_cast<int>((absTime - hours) * 60);

  ostringstream oss;
  oss << (isLate ? "+" : "-") << setfill('0') << setw(2) << hours << ":"
      << setw(2) << mins;
  return oss.str();
}

// Column width constants (display width)
namespace ColWidth {
const int ANGLE = 4;      // e.g., "+18" (without °)
const int EVENT = 30;     // Event name
const int TIME = 5;       // "hh:mm"
const int LEAVING = 6;    // "-hh:mm" or "+hh:mm"
const int DEPARTURE = 5;  // "hh:mm" or "PAST"
}  // namespace ColWidth

// Helper to generate repeated string (works with multi-byte UTF-8)
static string repeatStr(const string& s, int n) {
  string result;
  for (int i = 0; i < n; ++i) {
    result += s;
  }
  return result;
}

// Pad a string containing UTF-8 to a display width (right-aligned)
static string padRight(const string& s, int displayWidth, int extraBytes = 0) {
  int padding = displayWidth - (s.length() - extraBytes);
  if (padding <= 0) return s;
  return string(padding, ' ') + s;
}

// Pad a string containing UTF-8 to a display width (left-aligned)
static string padLeft(const string& s, int displayWidth, int extraBytes = 0) {
  int padding = displayWidth - (s.length() - extraBytes);
  if (padding <= 0) return s;
  return s + string(padding, ' ');
}

void printTwilightTable(double solarNoon, double latitude, double delta,
                        double currentTime, double commuteMinutes) {
  // Define twilight events using constants from constants.h
  // Sun angle: negative = above horizon, positive = below horizon
  // Zenith angles are derived from sun angles using: zenith = 90 - sunAngle

  vector<TwilightEvent> events = {
      {"Golden hour starts", -6.0, sunAngleToZenith(-6.0),
       Colors::GOLDEN_START},
      {"Sunset", 0.0, sunAngleToZenith(0.0), Colors::SUNSET},
      {"Golden hour ends", 4.0, sunAngleToZenith(4.0), Colors::GOLDEN_END},
      {"Civil twilight ends", 6.0, sunAngleToZenith(6.0), Colors::CIVIL},
      {"Nautical twilight ends", 12.0, sunAngleToZenith(12.0),
       Colors::NAUTICAL},
      {"Astronomical twilight ends", 18.0, sunAngleToZenith(18.0),
       Colors::ASTRONOMICAL}};

  double commuteHours = commuteMinutes / kMinutesPerHour;

  // Build border strings based on column widths
  // Add 1 to ANGLE for the ° symbol (2-byte UTF-8)
  string topBorder = "┌─" + repeatStr("─", ColWidth::ANGLE + 1) + "─┬─" +
                     repeatStr("─", ColWidth::EVENT) + "─┬─" +
                     repeatStr("─", ColWidth::TIME) + "─┬─" +
                     repeatStr("─", ColWidth::LEAVING) + "─┬─" +
                     repeatStr("─", ColWidth::DEPARTURE) + "─┐";
  string midBorder = "├─" + repeatStr("─", ColWidth::ANGLE + 1) + "─┼─" +
                     repeatStr("─", ColWidth::EVENT) + "─┼─" +
                     repeatStr("─", ColWidth::TIME) + "─┼─" +
                     repeatStr("─", ColWidth::LEAVING) + "─┼─" +
                     repeatStr("─", ColWidth::DEPARTURE) + "─┤";
  string botBorder = "└─" + repeatStr("─", ColWidth::ANGLE + 1) + "─┴─" +
                     repeatStr("─", ColWidth::EVENT) + "─┴─" +
                     repeatStr("─", ColWidth::TIME) + "─┴─" +
                     repeatStr("─", ColWidth::LEAVING) + "─┴─" +
                     repeatStr("─", ColWidth::DEPARTURE) + "─┘";

  // Print table header
  cout << endl;
  cout << Colors::BOLD << topBorder << Colors::RESET << endl;
  cout << Colors::BOLD << "│ " << left << setw(ColWidth::ANGLE + 1) << "Angle"
       << " │ " << setw(ColWidth::EVENT) << "Event"
       << " │ " << setw(ColWidth::TIME) << "Time"
       << " │ " << setw(ColWidth::LEAVING) << "Leave"
       << " │ " << setw(ColWidth::DEPARTURE) << "Dept"
       << " │" << Colors::RESET << endl;
  cout << Colors::BOLD << midBorder << Colors::RESET << endl;

  // Calculate and print each event
  for (const auto& event : events) {
    double HA_deg = calcHourAngle(event.zenith, latitude, delta);

    // Format angle with sign (° is 2 bytes UTF-8, so add 1 to display width)
    ostringstream angleOss;
    if (event.sunAngle >= 0) {
      angleOss << "+" << fixed << setprecision(0) << event.sunAngle << "°";
    } else {
      angleOss << fixed << setprecision(0) << event.sunAngle << "°";
    }
    // Pad manually for UTF-8: display width is ANGLE+1, extra byte is 1
    string angleStr = padRight(angleOss.str(), ColWidth::ANGLE + 1, 1);

    if (HA_deg < 0) {
      // Event doesn't occur at this latitude/date
      cout << "│ " << event.colorCode << angleStr << Colors::RESET << " │ "
           << event.colorCode << left << setw(ColWidth::EVENT) << event.label
           << Colors::RESET << " │ " << event.colorCode << setw(ColWidth::TIME)
           << "--:--" << Colors::RESET << " │ " << event.colorCode << right
           << setw(ColWidth::LEAVING) << "N/A" << Colors::RESET << " │ "
           << event.colorCode << setw(ColWidth::DEPARTURE) << "--:--"
           << Colors::RESET << " │" << endl;
      continue;
    }

    double HA = HA_deg / 15.0;
    double eventTime = solarNoon + HA;
    double departureTime = eventTime - commuteHours;
    double timeToDepart = departureTime - currentTime;

    string countdownStr = formatCountdown(timeToDepart);
    string departureStr =
        (timeToDepart < 0) ? " PAST" : formatTime(departureTime);

    cout << "│ " << event.colorCode << angleStr << Colors::RESET << " │ "
         << event.colorCode << left << setw(ColWidth::EVENT) << event.label
         << Colors::RESET << " │ " << event.colorCode << setw(ColWidth::TIME)
         << formatTime(eventTime) << Colors::RESET << " │ " << event.colorCode
         << right << setw(ColWidth::LEAVING) << countdownStr << Colors::RESET
         << " │ " << event.colorCode << setw(ColWidth::DEPARTURE)
         << departureStr << Colors::RESET << " │" << endl;
  }

  // Print table footer
  cout << Colors::BOLD << botBorder << Colors::RESET << endl;
  cout << endl;
}
