#ifndef MORNING_TABLE_H
#define MORNING_TABLE_H

#include <string>

/**
 * Print a color-coded table of morning twilight events with ETA information.
 *
 * Displays two sections:
 *   1. Morning arrival: Single commute time to arrive by sunrise/twilight
 * events
 *   2. Evening departure: Double commute time (to/from work) + work day to
 * depart by sunset/twilight events (assumes 8.5 hour work day)
 *
 * Morning section events (sun angles relative to horizon):
 *   - Astronomical twilight begins (+18°)
 *   - Nautical twilight begins (+12°)
 *   - Civil twilight begins (+6°)
 *   - Golden hour starts (−6°)
 *   - Sunrise (0°, standard elevation correction applied)
 *   - Golden hour ends (+4°)
 *
 * Evening section (return commute home):
 *   - Same events displayed for afternoon/sunset times
 *   - Departure times account for 2x commute + work day
 *
 * @param solarNoon       Corrected solar noon time (fractional hours)
 * @param latitude        Observer's latitude in degrees
 * @param delta           Solar declination in degrees
 * @param currentTime     Current local time (fractional hours)
 * @param commuteMinutes  Commute duration in minutes (one way)
 * @param workdayHours    Hours spent at work (default 8.5)
 */
void printMorningTable(double solarNoon, double latitude, double delta,
                       double currentTime, double commuteMinutes,
                       double workdayHours = 8.5);

#endif  // MORNING_TABLE_H
