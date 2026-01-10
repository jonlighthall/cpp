#ifndef TABLE_LAYOUT_H
#define TABLE_LAYOUT_H

// Shared table column width constants for twilight/sunset programs.
// Display widths in characters (excluding multi-byte UTF-8 overhead).

namespace table_layout {

// Shared columns (both programs)
namespace col {
constexpr int ANGLE =
    4;  // Sun angle, e.g., "+18" (degree symbol adds 1 extra byte)
constexpr int EVENT = 30;  // Event name/description
constexpr int TIME = 5;    // "hh:mm" format
}  // namespace col

// Twilight program specific
namespace twilight {
constexpr int RELATIVE = 6;  // "+/-hh:mm" format
}

// Sunset/twilight_table program specific
namespace sunset {
constexpr int LEAVING = 6;    // "+/-hh:mm" countdown
constexpr int DEPARTURE = 5;  // "hh:mm" or "PAST"
}  // namespace sunset

}  // namespace table_layout

#endif  // TABLE_LAYOUT_H
