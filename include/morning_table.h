#ifndef MORNING_TABLE_H
#define MORNING_TABLE_H

#include <string>

/**
 * Print a color-coded table of morning twilight events with ETA information.
 *
 * Events displayed (sun angles relative to horizon):
 *   - Astronomical twilight begins (+18°)
 *   - Nautical twilight begins (+12°)
 *   - Civil twilight begins (+6°)
 *   - Golden hour starts (−6°)
 *   - Sunrise (0°, standard elevation correction applied)
 *   - Golden hour ends (+4°)
 *
 * @param solarNoon      Corrected solar noon time (fractional hours)
 * @param latitude       Observer's latitude in degrees
 * @param delta          Solar declination in degrees
 * @param currentTime    Current local time (fractional hours)
 * @param commuteMinutes Commute duration in minutes
 */
void printMorningTable(double solarNoon, double latitude, double delta,
                       double currentTime, double commuteMinutes);

#endif  // MORNING_TABLE_H
