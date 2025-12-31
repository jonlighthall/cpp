#ifndef TWILIGHT_TABLE_H
#define TWILIGHT_TABLE_H

#include <string>

/**
 * Print a color-coded table of twilight events with ETA information.
 *
 * Events displayed (with sun angles relative to horizon):
 *   - Start of golden hour (-6°)
 *   - Sunset (0°)
 *   - End of golden hour (+4°)
 *   - End of civil twilight (+6°)
 *   - End of nautical twilight (+12°)
 *   - End of astronomical twilight (+18°)
 *
 * @param solarNoon     Corrected solar noon time (fractional hours)
 * @param latitude      Observer's latitude in degrees
 * @param delta         Solar declination in degrees
 * @param currentTime   Current time (fractional hours)
 * @param commuteMinutes Commute duration in minutes
 */
void printTwilightTable(double solarNoon, double latitude, double delta,
                        double currentTime, double commuteMinutes);

#endif  // TWILIGHT_TABLE_H
