/**
 * Sunset Commute Planner
 *
 * Simple evening commute planner that calculates when to leave work
 * to arrive home by civil twilight (6° below horizon, "legally dark").
 *
 * Usage: ./sunset
 * Output: Leave-by time and civil twilight ending time
 *
 * References:
 * - NOAA Solar Calculator: https://www.esrl.noaa.gov/gmd/grad/solcalc/
 * - Meeus, Jean (1991). Astronomical Algorithms.
 */

#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "config_commute.h"
#include "config_location.h"
#include "constants.h"
#include "format_utils.h"
#include "solar_utils.h"
#include "sunset_calc.h"
#include "time_init.h"
#include "twilight_table.h"

using namespace std;
using namespace astro;
using namespace config;

int main() {
  tm ltm;
  time_init::getLocalTime(ltm);
  int year, month, day;
  time_init::getDate(ltm, year, month, day);

  // Set the location and timezone from config
  double latitude = config::location::kDefaultLatitude;
  double longitude = config::location::kDefaultLongitude;
  int set_timezone = config::location::kDefaultTimezone;

  // Use library to calculate sunset and solar data
  double solarNoon, delta;
  sunset_calc::SunsetCalculator calc;
  calc.getSunset(year, month, day, latitude, longitude, set_timezone,
                 config::location::kDefaultObserverAltitude, &solarNoon,
                 &delta);

  // Calculate civil twilight ending (6° below horizon on evening side)
  // This is when it's legally dark for driving purposes
  double civilTwilightZenith = solar_utils::sunAngleToZenith(6.0);
  double civilTwilightHA_deg =
      solar_utils::calcHourAngle(civilTwilightZenith, latitude, delta);
  double civilTwilightEndTime = (civilTwilightHA_deg < 0)
                                    ? 0.0
                                    : solarNoon + (civilTwilightHA_deg / 15.0);

  cout << "Current time: " << put_time(&ltm, "%H:%M:%S");

  // Calculate the difference between the current time and civil twilight ending
  double currentTime = time_init::toFractionalHours(ltm);
  double timeDifference = civilTwilightEndTime - currentTime;

  // Convert the time difference to hours and minutes
  int diffHours = static_cast<int>(timeDifference);
  int diffMinutes = static_cast<int>((timeDifference - diffHours) * 60);

  // Print the time difference in plain English
  cout << " (" << format_utils::timeToEnglish(diffHours, diffMinutes)
       << " until civil twilight ends)" << endl;

  // Calculate leave time (subtract commute time)
  const double commuteMinutes = config::commute::kDefaultCommuteMinutes;
  double leaveTime = civilTwilightEndTime - (commuteMinutes / kMinutesPerHour);
  double timeToLeave = leaveTime - currentTime;

  // Convert leave time to readable format (no seconds)
  string leaveTimeStr = format_utils::formatHHMM(leaveTime);

  // Convert time to leave to hours and minutes
  int leaveHours = static_cast<int>(timeToLeave);
  int leaveMins = static_cast<int>((timeToLeave - leaveHours) * 60);

  // Get civil twilight ending time as string for summary (no seconds)
  string civilTwilightStr = format_utils::formatHHMM(civilTwilightEndTime);

  // Print the clear summary
  cout << endl;
  if (timeToLeave > 0) {
    cout << "Leave by " << leaveTimeStr << " (in "
         << format_utils::timeToEnglish(leaveHours, leaveMins)
         << ") to get home by " << civilTwilightStr << " (civil twilight ends)"
         << endl;
  } else {
    cout << "*** YOU SHOULD HAVE LEFT "
         << format_utils::timeToEnglish(std::abs(leaveHours),
                                        std::abs(leaveMins))
         << " AGO TO GET HOME BY " << civilTwilightStr
         << " (CIVIL TWILIGHT END) ***" << endl;

    // Calculate how late you'll be if you leave now
    double arrivalTime = currentTime + (commuteMinutes / kMinutesPerHour);
    double lateBy = arrivalTime - civilTwilightEndTime;

    if (lateBy > 0) {
      int lateHours = static_cast<int>(lateBy);
      int lateMins = static_cast<int>((lateBy - lateHours) * kMinutesPerHour);
      string arrivalTimeStr = format_utils::formatHHMM(arrivalTime);

      cout << "If you leave NOW, you'll be home at " << arrivalTimeStr << " ("
           << format_utils::timeToEnglish(lateHours, lateMins)
           << " after civil twilight ends)" << endl;
    }
  }

  // Print the twilight table with all events
  printTwilightTable(solarNoon, latitude, delta, currentTime, commuteMinutes);

  return 0;
}
