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

}  // namespace format_utils

#endif  // FORMAT_UTILS_H
