#ifndef COLORS_H
#define COLORS_H

// Shared ANSI color codes for terminal output across sunset/twilight programs.
// Simple, educational constants kept in a single place to avoid duplication.
// These are safe to use on typical Linux/macOS terminals. Windows may require
// ANSI support.

namespace Colors {
// Twilight progression (deep blue → indigo → sky blue)
// Based on Rayleigh scattering physics for atmospheric light
constexpr const char* ASTRONOMICAL = "\033[38;5;21m";  // Deep blue (ID 21)
constexpr const char* NAUTICAL = "\033[38;5;57m";  // Deep violet/indigo (ID 57)
constexpr const char* CIVIL =
    "\033[38;5;39m";  // Deep sky blue (ID 39) - "Blue Hour"

// Sun events (warm tones: gold → orange → ochre)
constexpr const char* SUNRISE =
    "\033[38;5;208m";  // Safety orange (ID 208) - strong scattering
constexpr const char* SOLAR_NOON =
    "\033[38;5;229m";  // Wheat/pale yellow (ID 229) - peak brightness
constexpr const char* GOLDEN_START =
    "\033[38;5;220m";  // Bright gold (ID 220) - sun visible, low angle
constexpr const char* SUNSET =
    "\033[38;5;208m";  // Safety orange (ID 208) - horizon scattering
constexpr const char* GOLDEN_END =
    "\033[38;5;172m";  // Dark ochre (ID 172) - fading into twilight

// Text styles
constexpr const char* RESET = "\033[0m";
constexpr const char* BOLD = "\033[1m";
}  // namespace Colors

#endif  // COLORS_H
