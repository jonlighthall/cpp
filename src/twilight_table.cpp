/**
 * Twilight Table - Color-coded display of twilight events
 *
 * Displays a table showing departure times, ETAs, and arrival times
 * for various twilight events, color-coded from golden hour (orange)
 * through astronomical twilight (dark blue).
 */

#include "twilight_table.h"

#include <string>
#include <vector>

#include "colors.h"
#include "constants.h"
#include "solar_utils.h"
#include "table_render.h"

using namespace std;
using namespace astro;

void printTwilightTable(double solarNoon, double latitude, double delta,
                        double currentTime, double commuteMinutes) {
  // Define twilight events using constants from constants.h
  // Sun angle: negative = above horizon, positive = below horizon
  // Zenith angles are derived from sun angles using: zenith = 90 - sunAngle
  //
  // NOTE: Sunset uses the standard zenith angle which includes:
  //   - Sun's angular radius: 0.26667°
  //   - Atmospheric refraction: 0.5667°
  //   - Total correction: 0.833° (kStandardSunsetElevation)
  //   - Zenith = 90° - (-0.833°) = 90.833°

  vector<table_render::SolarEvent> events = {
      {"Golden hour starts", -6.0, solar_utils::sunAngleToZenith(-6.0),
       Colors::GOLDEN_START},
      {"Sunset", 0.0, 90.0 - kStandardSunsetElevation, Colors::SUNSET},
      {"Golden hour ends", 4.0, solar_utils::sunAngleToZenith(4.0),
       Colors::GOLDEN_END},
      {"Civil twilight ends", 6.0, solar_utils::sunAngleToZenith(6.0),
       Colors::CIVIL},
      {"Nautical twilight ends", 12.0, solar_utils::sunAngleToZenith(12.0),
       Colors::NAUTICAL},
      {"Astronomical twilight ends", 18.0, solar_utils::sunAngleToZenith(18.0),
       Colors::ASTRONOMICAL}};

  double commuteHours = commuteMinutes / kMinutesPerHour;

  table_render::printDepartureTable("", events, solarNoon, latitude, delta,
                                    currentTime, commuteHours,
                                    /*isMorning=*/false);
  cout << endl;
}
