/**
 * Morning Twilight Table - Color-coded display of dawn events
 *
 * Displays a table showing departure times, ETAs, and arrival times
 * for morning twilight events, color-coded from astronomical twilight
 * (dark blue) through golden hour (warm tones).
 */

#include "morning_table.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "colors.h"
#include "constants.h"
#include "solar_utils.h"
#include "table_render.h"

using namespace std;
using namespace astro;

void printMorningTable(double solarNoon, double latitude, double delta,
                       double currentTime, double commuteMinutes,
                       double workdayHours) {
  // Define morning events (before solar noon)
  // Convention: sunAngle < 0 => above horizon; sunAngle > 0 => below
  vector<table_render::SolarEvent> morningEvents = {
      {"Astronomical twilight begins", 18.0,
       solar_utils::sunAngleToZenith(18.0), Colors::ASTRONOMICAL},
      {"Nautical twilight begins", 12.0, solar_utils::sunAngleToZenith(12.0),
       Colors::NAUTICAL},
      {"Civil twilight begins", 6.0, solar_utils::sunAngleToZenith(6.0),
       Colors::CIVIL},
      {"Golden hour starts", 4.0, solar_utils::sunAngleToZenith(4.0),
       Colors::GOLDEN_START},
      {"Sunrise", 0.0, 90.0 - kStandardSunsetElevation, Colors::SUNSET},
      {"Golden hour ends", -6.0, solar_utils::sunAngleToZenith(-6.0),
       Colors::GOLDEN_END},
  };

  double commuteHours = commuteMinutes / kMinutesPerHour;

  // ===== MORNING ARRIVAL SECTION =====
  table_render::printDepartureTable("MORNING ARRIVAL (Single Commute)",
                                    morningEvents, solarNoon, latitude, delta,
                                    currentTime, commuteHours,
                                    /*isMorning=*/true);

  // ===== EVENING DEPARTURE SECTION =====
  // Mirror the morning events for sunset times with double commute + workday
  vector<table_render::SolarEvent> eveningEvents = {
      {"Golden hour ends", -6.0, solar_utils::sunAngleToZenith(-6.0),
       Colors::GOLDEN_END},
      {"Sunset", 0.0, 90.0 - kStandardSunsetElevation, Colors::SUNSET},
      {"Golden hour starts", 4.0, solar_utils::sunAngleToZenith(4.0),
       Colors::GOLDEN_START},
      {"Civil twilight ends", 6.0, solar_utils::sunAngleToZenith(6.0),
       Colors::CIVIL},
      {"Nautical twilight ends", 12.0, solar_utils::sunAngleToZenith(12.0),
       Colors::NAUTICAL},
      {"Astronomical twilight ends", 18.0, solar_utils::sunAngleToZenith(18.0),
       Colors::ASTRONOMICAL}};

  double doubleCommuteHours = 2.0 * commuteHours;
  double workdayHoursToDepart = doubleCommuteHours + workdayHours;

  ostringstream titleOss;
  titleOss << "EVENING DEPARTURE (Home Commute: 2× Commute + " << fixed
           << setprecision(1) << workdayHours << "-hour Work Day)";

  table_render::printDepartureTable(titleOss.str(), eveningEvents, solarNoon,
                                    latitude, delta, currentTime,
                                    workdayHoursToDepart,
                                    /*isMorning=*/false);
  cout << endl;
}
