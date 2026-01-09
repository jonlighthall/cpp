/**
 * Default configuration values for the sunset program.
 *
 * These are user-configurable defaults that can be overridden
 * at runtime via command-line arguments or environment variables.
 * Naming follows Google C++ Style Guide: kConstantName for constants.
 *
 * To customize for your location, either:
 *   1. Modify these defaults and recompile
 *   2. Pass command-line arguments (when implemented)
 *   3. Set environment variables (when implemented)
 */

#ifndef CONFIG_H
#define CONFIG_H

namespace config {

// Default location: New Orleans, LA
namespace location {
constexpr double kDefaultLatitude = 30.4275784357249;
constexpr double kDefaultLongitude = -90.0914955109431;
constexpr int kDefaultTimezone = -6;  // CST (Central Standard Time)
}  // namespace location

// Commute settings
namespace commute {
constexpr double kDefaultCommuteMinutes = 37.0;
}  // namespace commute

// Debug/output settings
namespace debug {
constexpr int kDefaultDebugLevel = -1;
/*
 * Debug level values:
 *  -1: Print sunset and commute only (minimal output)
 *   0: Print major calculations
 *   1: Print each calculation step, including alternate values
 *   2: Print every variable value (verbose)
 */
}  // namespace debug

// Algorithm selection
namespace algorithm {
constexpr bool kUseNoaa = true;  // Use NOAA formulas (vs. other sources)
}  // namespace algorithm

}  // namespace config

#endif  // CONFIG_H
