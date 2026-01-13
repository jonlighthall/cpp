/**
 * Commute Configuration
 *
 * Commute timing and workday settings for the sunrise and sunset programs.
 * Not used by ephemeris or twilight.
 *
 * To customize your commute profile, modify these values and recompile.
 * Future: Command-line arguments or environment variables for runtime override.
 */

#ifndef CONFIG_COMMUTE_H
#define CONFIG_COMMUTE_H

namespace config {
namespace commute {

// One-way commute time in minutes
constexpr double kDefaultCommuteMinutes = 37.0;

// Workday length in hours (used for evening departure calculation)
// Example: 8.5 hours means leave work after 8.5 hours to plan return journey
constexpr double kWorkdayHours = 8.5;

}  // namespace commute
}  // namespace config

#endif  // CONFIG_COMMUTE_H
