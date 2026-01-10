#ifndef TEXT_UTILS_H
#define TEXT_UTILS_H

#include <string>

namespace text_utils {

// Generate repeated string (works with multi-byte UTF-8 content without width
// calc)
inline std::string repeatStr(const std::string& s, int n) {
  std::string result;
  for (int i = 0; i < n; ++i) result += s;
  return result;
}

// Right-align with padding; extraBytes accounts for multi-byte symbols like "°"
// (2 bytes)
inline std::string padRight(const std::string& s, int displayWidth,
                            int extraBytes = 0) {
  int padding = displayWidth - static_cast<int>(s.length() - extraBytes);
  if (padding <= 0) return s;
  return std::string(padding, ' ') + s;
}

// Left-align with padding; extraBytes accounts for multi-byte symbols
inline std::string padLeft(const std::string& s, int displayWidth,
                           int extraBytes = 0) {
  int padding = displayWidth - static_cast<int>(s.length() - extraBytes);
  if (padding <= 0) return s;
  return s + std::string(padding, ' ');
}

}  // namespace text_utils

#endif  // TEXT_UTILS_H
