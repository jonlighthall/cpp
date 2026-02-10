#ifndef FORMAT_UTILS_H
#define FORMAT_UTILS_H

#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>

namespace format_utils {

// Format fractional hours to HH:MM. Returns "--:--" for negatives.
inline std::string formatHHMM(double fhr) {
  if (fhr < 0) return "--:--";
  int hr = static_cast<int>(std::floor(fhr));
  int min = static_cast<int>(std::floor((fhr - hr) * 60));
  std::ostringstream oss;
  oss << std::setfill('0') << std::setw(2) << hr << ":" << std::setw(2) << min;
  return oss.str();
}

// Format signed time difference to +/-HH:MM. Negative -> "-", positive -> "+".
inline std::string formatSignedHHMM(double timeDiff) {
  bool isLate = timeDiff < 0;
  double absTime = std::abs(timeDiff);
  int hours = static_cast<int>(absTime);
  int mins = static_cast<int>((absTime - hours) * 60);
  std::ostringstream oss;
  oss << (isLate ? "+" : "-") << std::setfill('0') << std::setw(2) << hours
      << ":" << std::setw(2) << mins;
  return oss.str();
}

// Format fractional hours to HH:MM:SS.ss (full precision time string).
// Useful for detailed display of calculated times.
// Options:
//   showSeconds: include seconds in output (default true)
//   fractionalSeconds: include fractional part of seconds (default true)
inline std::string formatHHMMSS(double fhr, bool showSeconds = true,
                                bool fractionalSeconds = true) {
  int hr = static_cast<int>(std::floor(fhr));
  double fmin = (fhr - hr) * 60.0;
  int min = static_cast<int>(std::floor(fmin));
  double fsec = (fmin - min) * 60.0;

  std::ostringstream oss;
  oss << std::setfill('0') << std::setw(2) << hr << ":" << std::setfill('0')
      << std::setw(2) << min;

  if (showSeconds) {
    if (fractionalSeconds) {
      oss << ":" << std::fixed << std::setprecision(2) << std::setfill('0')
          << std::setw(5) << fsec;
    } else {
      oss << ":" << std::setfill('0') << std::setw(2)
          << static_cast<int>(std::floor(fsec));
    }
  }

  return oss.str();
}

// Convert a time difference (in hours) to plain English, e.g. "2 hours 15
// minutes". Useful for user-facing countdown displays.
inline std::string timeToEnglish(int hours, int minutes) {
  std::ostringstream oss;
  if (hours == 0 && minutes == 0) return "0 minutes";

  if (hours != 0) {
    oss << std::abs(hours) << (std::abs(hours) == 1 ? " hour" : " hours");
  }
  if (minutes != 0) {
    if (hours != 0) oss << " ";
    oss << std::abs(minutes)
        << (std::abs(minutes) == 1 ? " minute" : " minutes");
  }
  return oss.str();
}

}  // namespace format_utils

#endif  // FORMAT_UTILS_H
