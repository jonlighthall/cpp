#ifndef COLORS_H
#define COLORS_H

// Shared ANSI color codes for terminal output across sunset/twilight programs.
// Simple, educational constants kept in a single place to avoid duplication.
// These are safe to use on typical Linux/macOS terminals. Windows may require
// ANSI support.

namespace Colors {
// Twilight blues
constexpr const char* ASTRONOMICAL = "\033[38;5;19m";  // Dark blue
constexpr const char* NAUTICAL = "\033[38;5;33m";      // Medium blue
constexpr const char* CIVIL = "\033[38;5;75m";         // Light blue

// Sun-related warm tones
constexpr const char* SUNRISE = "\033[38;5;220m";       // Yellow
constexpr const char* SOLAR_NOON = "\033[38;5;226m";    // Bright yellow
constexpr const char* GOLDEN_START = "\033[38;5;214m";  // Orange
constexpr const char* SUNSET = "\033[38;5;208m";        // Dark orange
constexpr const char* GOLDEN_END = "\033[38;5;202m";    // Red-orange

// Text styles
constexpr const char* RESET = "\033[0m";
constexpr const char* BOLD = "\033[1m";
}  // namespace Colors

#endif  // COLORS_H
