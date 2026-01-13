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
#include "solar_utils.h"
#include "sunset_calc.h"
#include "twilight_table.h"

using namespace std;
using namespace astro;
using namespace config;

// Utility function to convert fractional hours to HH:MM format

string hour2time(double fhr, bool do_fractional_second = true,
                 bool show_seconds = true) {
  // convert fractional hour into hr:min:sec string
  auto hr = int(floor(fhr));
  double fmin = (fhr - hr) * 60;
  auto min = int(floor(fmin));
  double fsec = (fmin - min) * 60;

  struct Time {
    int hr;
    int min;
    double fsec;
  };

  Time time;
  if (do_fractional_second) {
    time.hr = hr;
    time.min = min;
    time.fsec = fsec;
  } else {
    time.hr = hr;
    time.min = min;
    time.fsec = int(floor(fsec));
  }

  std::ostringstream time_string;
  time_string << std::setfill('0') << std::setw(2) << time.hr << ":"
              << std::setfill('0') << std::setw(2) << time.min;

  if (show_seconds) {
    time_string << ":" << std::fixed << std::setprecision(2)
                << std::setfill('0') << std::setw(5) << time.fsec;
  }

  return time_string.str();
}

string timeToEnglish(int hours, int minutes) {
  // Convert hours and minutes to plain English
  std::ostringstream english_time;

  if (hours == 0 && minutes == 0) {
    return "0 minutes";
  }

  if (hours > 0) {
    english_time << hours << " hour";
    if (hours > 1) english_time << "s";

    if (minutes > 0) {
      english_time << " " << minutes << " minute";
      if (minutes > 1) english_time << "s";
    }
  } else if (minutes > 0) {
    english_time << minutes << " minute";
    if (minutes > 1) english_time << "s";
  }

  return english_time.str();
}

int main() {
  // Get the current date and time
  time_t now = time(nullptr);
  tm ltm;
#ifdef _WIN32
  localtime_s(&ltm, &now);
#else
  localtime_r(&now, &ltm);
#endif
  int year = ltm.tm_year + 1900;
  int month = ltm.tm_mon + 1;
  int day = ltm.tm_mday;

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
  double currentTime = ltm.tm_hour + ltm.tm_min / 60.0 + ltm.tm_sec / 3600.0;
  double timeDifference = civilTwilightEndTime - currentTime;

  // Convert the time difference to hours and minutes
  int diffHours = static_cast<int>(timeDifference);
  int diffMinutes = static_cast<int>((timeDifference - diffHours) * 60);

  // Print the time difference in plain English
  cout << " (" << timeToEnglish(diffHours, diffMinutes)
       << " until civil twilight ends)" << endl;

  // Calculate leave time (subtract commute time)
  const double commuteMinutes = config::commute::kDefaultCommuteMinutes;
  double leaveTime = civilTwilightEndTime - (commuteMinutes / kMinutesPerHour);
  double timeToLeave = leaveTime - currentTime;

  // Convert leave time to readable format (no seconds)
  string leaveTimeStr = hour2time(leaveTime, false, false);

  // Convert time to leave to hours and minutes
  int leaveHours = static_cast<int>(timeToLeave);
  int leaveMins = static_cast<int>((timeToLeave - leaveHours) * 60);

  // Get civil twilight ending time as string for summary (no seconds)
  string civilTwilightStr = hour2time(civilTwilightEndTime, false, false);

  // Print the clear summary
  cout << endl;
  if (timeToLeave > 0) {
    cout << "Leave by " << leaveTimeStr << " (in "
         << timeToEnglish(leaveHours, leaveMins) << ") to get home by "
         << civilTwilightStr << " (civil twilight ends)" << endl;
  } else {
    cout << "*** YOU SHOULD HAVE LEFT "
         << timeToEnglish(std::abs(leaveHours), std::abs(leaveMins))
         << " AGO TO GET HOME BY " << civilTwilightStr
         << " (CIVIL TWILIGHT END) ***" << endl;

    // Calculate how late you'll be if you leave now
    double arrivalTime = currentTime + (commuteMinutes / kMinutesPerHour);
    double lateBy = arrivalTime - civilTwilightEndTime;

    if (lateBy > 0) {
      int lateHours = static_cast<int>(lateBy);
      int lateMins = static_cast<int>((lateBy - lateHours) * kMinutesPerHour);
      string arrivalTimeStr = hour2time(arrivalTime, false, false);

      cout << "If you leave NOW, you'll be home at " << arrivalTimeStr << " ("
           << timeToEnglish(lateHours, lateMins)
           << " after civil twilight ends)" << endl;
    }
  }

  // Print the twilight table with all events
  printTwilightTable(solarNoon, latitude, delta, currentTime, commuteMinutes);

  return 0;
}
