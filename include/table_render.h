/**
 * Shared table rendering utilities for solar event tables.
 *
 * Provides common functions for building table borders, formatting angle
 * strings, and rendering event rows with color coding. Used by morning_table,
 * twilight_table, and the twilight main program to eliminate duplication.
 */

#ifndef TABLE_RENDER_H
#define TABLE_RENDER_H

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "colors.h"
#include "format_utils.h"
#include "solar_utils.h"
#include "table_layout.h"
#include "text_utils.h"

namespace table_render {

// ============================================================================
// Border Construction
// ============================================================================

// Build a horizontal border line for a table with the given column widths.
inline std::string buildBorder(const std::vector<int>& colWidths,
                               const std::string& cornerLeft,
                               const std::string& joint,
                               const std::string& cornerRight,
                               const std::string& fill = "─") {
  std::string result = cornerLeft;
  for (size_t i = 0; i < colWidths.size(); ++i) {
    result += fill + text_utils::repeatStr(fill, colWidths[i]) + fill;
    result += (i + 1 < colWidths.size()) ? joint : cornerRight;
  }
  return result;
}

// Convenience: build top/mid/bot borders for the given column widths.
struct TableBorders {
  std::string top, mid, bot;
};

inline TableBorders makeBorders(const std::vector<int>& colWidths) {
  return {buildBorder(colWidths, "┌", "┬", "┐"),
          buildBorder(colWidths, "├", "┼", "┤"),
          buildBorder(colWidths, "└", "┴", "┘")};
}

// ============================================================================
// Angle Formatting
// ============================================================================

// Format a sun angle with sign and degree symbol, padded to ANGLE+1 width.
inline std::string formatAngle(double sunAngle) {
  std::ostringstream oss;
  if (sunAngle >= 0) {
    oss << "+" << std::fixed << std::setprecision(0) << sunAngle << "°";
  } else {
    oss << std::fixed << std::setprecision(0) << sunAngle << "°";
  }
  return text_utils::padRight(oss.str(), table_layout::col::ANGLE + 1, 1);
}

// ============================================================================
// Event Data Structure
// ============================================================================

// Shared representation of a solar/twilight event used across table renderers.
struct SolarEvent {
  std::string label;
  double sunAngle;  // Degrees relative to horizon (negative = above)
  double zenith;    // Zenith angle for calculation
  std::string colorCode;
};

// ============================================================================
// Row Rendering — Departure-style tables (morning_table, twilight_table)
// ============================================================================

// Render a single event row for a departure-style table (5 columns:
// Angle, Event, Time, Leave countdown, Departure time).
// isMorning: true = event before solar noon, false = after.
// offsetHours: time subtracted from event time to get departure
//              (single commute for twilight, double+workday for morning
//              evening).
inline void renderDepartureRow(const SolarEvent& event, double solarNoon,
                               double latitude, double delta,
                               double currentTime, double offsetHours,
                               bool isMorning) {
  using namespace std;
  using namespace table_layout::col;

  std::string angleStr = formatAngle(event.sunAngle);
  double HA_deg = solar_utils::calcHourAngle(event.zenith, latitude, delta);

  if (HA_deg < 0) {
    // Event doesn't occur
    cout << "│ " << event.colorCode << angleStr << Colors::RESET << " │ "
         << event.colorCode << left << setw(EVENT) << event.label
         << Colors::RESET << " │ " << event.colorCode << setw(TIME) << "--:--"
         << Colors::RESET << " │ " << event.colorCode << right
         << setw(table_layout::sunset::LEAVING) << "N/A" << Colors::RESET
         << " │ " << event.colorCode << setw(table_layout::sunset::DEPARTURE)
         << "--:--" << Colors::RESET << " │" << endl;
    return;
  }

  double HA = HA_deg / 15.0;
  double eventTime = isMorning ? solarNoon - HA : solarNoon + HA;
  double departureTime = eventTime - offsetHours;
  double timeToDepart = departureTime - currentTime;

  std::string countdownStr = format_utils::formatSignedHHMM(timeToDepart);
  std::string departureStr =
      (timeToDepart < 0) ? " PAST" : format_utils::formatHHMM(departureTime);

  cout << "│ " << event.colorCode << angleStr << Colors::RESET << " │ "
       << event.colorCode << left << setw(EVENT) << event.label << Colors::RESET
       << " │ " << event.colorCode << setw(TIME)
       << format_utils::formatHHMM(eventTime) << Colors::RESET << " │ "
       << event.colorCode << right << setw(table_layout::sunset::LEAVING)
       << countdownStr << Colors::RESET << " │ " << event.colorCode
       << setw(table_layout::sunset::DEPARTURE) << departureStr << Colors::RESET
       << " │" << endl;
}

// Print a complete departure-style table section (border, header, rows,
// border). events: list of solar events to render. title: optional title line
// printed above the table (empty string suppresses).
inline void printDepartureTable(const std::string& title,
                                const std::vector<SolarEvent>& events,
                                double solarNoon, double latitude, double delta,
                                double currentTime, double offsetHours,
                                bool isMorning) {
  using namespace std;
  using namespace table_layout::col;

  // Column widths: ANGLE+1 (for °), EVENT, TIME, LEAVING, DEPARTURE
  std::vector<int> cols = {ANGLE + 1, EVENT, TIME,
                           table_layout::sunset::LEAVING,
                           table_layout::sunset::DEPARTURE};
  auto borders = makeBorders(cols);

  cout << endl;
  if (!title.empty()) {
    cout << Colors::BOLD << title << Colors::RESET << endl;
  }
  cout << Colors::BOLD << borders.top << Colors::RESET << endl;
  cout << Colors::BOLD << "│ " << left << setw(ANGLE + 1) << "Angle"
       << " │ " << setw(EVENT) << "Event"
       << " │ " << setw(TIME) << "Time"
       << " │ " << setw(table_layout::sunset::LEAVING) << "Leave"
       << " │ " << setw(table_layout::sunset::DEPARTURE) << "Dept"
       << " │" << Colors::RESET << endl;
  cout << Colors::BOLD << borders.mid << Colors::RESET << endl;

  for (const auto& event : events) {
    renderDepartureRow(event, solarNoon, latitude, delta, currentTime,
                       offsetHours, isMorning);
  }

  cout << Colors::BOLD << borders.bot << Colors::RESET << endl;
}

}  // namespace table_render

#endif  // TABLE_RENDER_H
