/**
 * Shared time initialization utilities for main programs.
 *
 * Provides common date/time setup and current-time calculation used by
 * multiple main programs (sunrise, sunset, twilight).
 */

#ifndef TIME_INIT_H
#define TIME_INIT_H

#include <ctime>

namespace time_init {

// Populate a tm struct with the current local time (cross-platform).
// Returns the raw time_t value.
inline time_t getLocalTime(tm& ltm) {
  time_t now = time(nullptr);
#ifdef _WIN32
  localtime_s(&ltm, &now);
#else
  localtime_r(&now, &ltm);
#endif
  return now;
}

// Extract year, month, day from a tm struct.
inline void getDate(const tm& ltm, int& year, int& month, int& day) {
  year = ltm.tm_year + 1900;
  month = ltm.tm_mon + 1;
  day = ltm.tm_mday;
}

// Convert a tm struct to fractional hours (e.g., 14.5 = 2:30 PM).
inline double toFractionalHours(const tm& ltm) {
  return ltm.tm_hour + ltm.tm_min / 60.0 + ltm.tm_sec / 3600.0;
}

}  // namespace time_init

#endif  // TIME_INIT_H
