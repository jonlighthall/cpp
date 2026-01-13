/**
 * Ephemeris Educational Program Configuration
 *
 * Debug and algorithm selection settings for the ephemeris algorithm comparison
 * program. Not used by production programs (sunset, sunrise, twilight) or the
 * library.
 *
 * The ephemeris program is primarily pedagogical, showing calculation steps and
 * comparing multiple algorithm variants. These settings allow customization of
 * the educational output level and which algorithms to display.
 */

#ifndef CONFIG_EPHEMERIS_H
#define CONFIG_EPHEMERIS_H

namespace config {
namespace debug {

// Verbosity level for ephemeris educational output
constexpr int kDefaultDebugLevel = -1;
/*
 * Debug level values:
 *  -1: Print sunset/sunrise and algorithm comparisons only (minimal output)
 *   0: Print major calculations
 *   1: Print each calculation step, including alternate values
 *   2: Print every variable value (very verbose)
 */

}  // namespace debug

namespace algorithm {

// Which algorithm family to highlight by default in comparisons
// Note: ephemeris shows all three algorithms (NOAA, USNO, LASKAR) regardless
// This flag only indicates which one is "the recommendation"
constexpr bool kUseNoaa =
    true;  // true = recommend NOAA, false = recommend LASKAR

}  // namespace algorithm
}  // namespace config

#endif  // CONFIG_EPHEMERIS_H
