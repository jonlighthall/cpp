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

using namespace std;

// Constants
static const double PI = atan(1) * 4;
static const double deg2rad = PI / 180.;
static const double rad2deg = 180. / PI;

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
  double zenith;    // Zenith angle for calculation
  string colorCode;
};

// Calculate hour angle for a given zenith angle
static double calcHourAngle(double zenith, double latitude, double delta) {
  // Convert to radians
  double h0 = zenith * deg2rad;
  double phi = latitude * deg2rad;
  double d = delta * deg2rad;

  double cosH = (cos(h0) - sin(phi) * sin(d)) / (cos(phi) * cos(d));

  // Check if sun reaches this angle at this latitude
  if (cosH < -1.0 || cosH > 1.0) {
    return -1.0;  // Event doesn't occur
  }

  return acos(cosH) * rad2deg;
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

void printTwilightTable(double solarNoon, double latitude, double delta,
                        double currentTime, double commuteMinutes) {
  // Define twilight events
  // Sun angle: negative = above horizon, positive = below horizon
  // Zenith = 90 + sunAngle (for below horizon events)
  vector<TwilightEvent> events = {
      {"Golden hour starts", -6.0, 90.0 - 6.0, Colors::GOLDEN_START},
      {"Sunset", 0.0, 90.833,
       Colors::SUNSET},  // Standard sunset with refraction
      {"Golden hour ends", 4.0, 90.0 + 4.0, Colors::GOLDEN_END},
      {"Civil twilight ends", 6.0, 90.0 + 6.0, Colors::CIVIL},
      {"Nautical twilight ends", 12.0, 90.0 + 12.0, Colors::NAUTICAL},
      {"Astronomical twilight ends", 18.0, 90.0 + 18.0, Colors::ASTRONOMICAL}};

  double commuteHours = commuteMinutes / 60.0;

  // Print table header
  cout << endl;
  cout << Colors::BOLD
       << "┌───────┬────────────────────────────────┬─────────┬─────────┬───────────┐"
       << Colors::RESET << endl;
  cout << Colors::BOLD
       << "│ Angle │ Event                          │ Time    │ Leaving │ Departure │"
       << Colors::RESET << endl;
  cout << Colors::BOLD
       << "├───────┼────────────────────────────────┼─────────┼─────────┼───────────┤"
       << Colors::RESET << endl;

  // Calculate and print each event
  for (const auto& event : events) {
    double HA_deg = calcHourAngle(event.zenith, latitude, delta);

    // Format angle with sign
    ostringstream angleStr;
    if (event.sunAngle >= 0) {
      angleStr << "+" << fixed << setprecision(0) << event.sunAngle;
    } else {
      angleStr << fixed << setprecision(0) << event.sunAngle;
    }

    if (HA_deg < 0) {
      // Event doesn't occur at this latitude/date
      cout << "│ " << event.colorCode << right << setw(4) << angleStr.str()
           << "°" << Colors::RESET << " │ " << event.colorCode << left
           << setw(30) << event.label << Colors::RESET << " │ "
           << event.colorCode << " --:-- " << Colors::RESET << " │ "
           << event.colorCode << "  N/A  " << Colors::RESET << " │ "
           << event.colorCode << "   --:--  " << Colors::RESET << " │"
           << endl;
      continue;
    }

    double HA = HA_deg / 15.0;
    double eventTime = solarNoon + HA;
    double departureTime = eventTime - commuteHours;
    double timeToDepart = departureTime - currentTime;

    string countdownStr = formatCountdown(timeToDepart);

    cout << "│ " << event.colorCode << right << setw(4) << angleStr.str() << "°"
         << Colors::RESET << " │ " << event.colorCode << left << setw(30)
         << event.label << Colors::RESET << " │ " << event.colorCode << " "
         << formatTime(eventTime) << " " << Colors::RESET << " │ "
         << event.colorCode << " " << countdownStr << " " << Colors::RESET
         << " │ " << event.colorCode << "   " << formatTime(departureTime)
         << "   " << Colors::RESET << "│" << endl;
  }

  // Print table footer
  cout << Colors::BOLD
       << "└───────┴────────────────────────────────┴─────────┴─────────┴───────────┘"
       << Colors::RESET << endl;
  cout << endl;
}
