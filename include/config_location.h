/**
 * Location Configuration
 *
 * Geographic and observer positioning data used by all solar calculation
 * programs.
 *
 * To customize for your location, modify these values and recompile.
 * Future: Command-line arguments or environment variables for runtime override.
 */

#ifndef CONFIG_LOCATION_H
#define CONFIG_LOCATION_H

namespace config {
namespace location {

// Default location: New Orleans, LA
constexpr double kDefaultLatitude = 30.4275784357249;
constexpr double kDefaultLongitude = -90.0914955109431;
constexpr int kDefaultTimezone = -6;  // CST (Central Standard Time)
constexpr double kDefaultObserverAltitude = 1.8224;  // meters (6 feet)

}  // namespace location
}  // namespace config

#endif  // CONFIG_LOCATION_H
