#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "colors.h"
#include "config_commute.h"
#include "config_location.h"
#include "constants.h"
#include "format_utils.h"
#include "morning_table.h"
#include "solar_utils.h"
#include "sunset_calc.h"

using namespace std;
using namespace astro;

// Minimal helper: plain-English time difference (hours, minutes)
static string timeToEnglish(int hours, int minutes) {
  ostringstream oss;
  if (hours != 0) {
    oss << abs(hours) << (abs(hours) == 1 ? " hour" : " hours");
  }
  if (minutes != 0) {
    if (hours != 0) oss << " ";
    oss << abs(minutes) << (abs(minutes) == 1 ? " minute" : " minutes");
  }
  if (hours == 0 && minutes == 0) oss << "0 minutes";
  return oss.str();
}

int main() {
  // Current local date/time
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

  // Location and timezone from config
  double latitude = config::location::kDefaultLatitude;
  double longitude = config::location::kDefaultLongitude;
  int set_timezone = config::location::kDefaultTimezone;

  // Compute sunrise using the library; also get solar noon and declination
  double solarNoon = 0.0;
  double delta = 0.0;
  sunset_calc::SunsetCalculator calc;
  double sunriseTime = calc.getSunrise(
      year, month, day, latitude, longitude, set_timezone,
      config::location::kDefaultObserverAltitude, &solarNoon, &delta);

  // Calculate civil twilight ending (6° below horizon on evening side)
  // This is when it's legally dark for driving purposes
  double civilTwilightZenith = solar_utils::sunAngleToZenith(6.0);
  double civilTwilightHA_deg =
      solar_utils::calcHourAngle(civilTwilightZenith, latitude, delta);
  double civilTwilightEndTime = (civilTwilightHA_deg < 0)
                                    ? 0.0
                                    : solarNoon + (civilTwilightHA_deg / 15.0);

  // Current time and friendly display
  cout << "Current time: " << put_time(&ltm, "%H:%M:%S");
  double currentTime = ltm.tm_hour + ltm.tm_min / 60.0 + ltm.tm_sec / 3600.0;
  double untilSunrise = sunriseTime - currentTime;
  int diffHours = static_cast<int>(untilSunrise);
  int diffMinutes = static_cast<int>((untilSunrise - diffHours) * 60);
  cout << " (" << timeToEnglish(diffHours, diffMinutes) << " until sunrise)"
       << endl;

  // Commute planning to arrive by sunrise
  const double commuteMinutes = config::commute::kDefaultCommuteMinutes;
  double commuteHours = commuteMinutes / kMinutesPerHour;
  double leaveTime = sunriseTime - commuteHours;
  double timeToLeave = leaveTime - currentTime;

  string leaveHHMM = format_utils::formatHHMM(leaveTime);
  string sunriseHHMM = format_utils::formatHHMM(sunriseTime);

  // Evening return commute: depart work to arrive home by civil twilight end
  const double workdayHours = 8.5;
  double doubleCommuteHours = 2.0 * commuteHours;
  double homeCommuteLeaveTime =
      civilTwilightEndTime - doubleCommuteHours - workdayHours;
  double timeToLeaveForHome = homeCommuteLeaveTime - currentTime;
  string homeCommuteHHMM = format_utils::formatHHMM(homeCommuteLeaveTime);
  string civilTwilightHHMM = format_utils::formatHHMM(civilTwilightEndTime);

  cout << endl;
  if (timeToLeave > 0) {
    int leaveHours = static_cast<int>(timeToLeave);
    int leaveMins = static_cast<int>((timeToLeave - leaveHours) * 60);
    cout << "Leave by " << leaveHHMM << " (in "
         << timeToEnglish(leaveHours, leaveMins) << ") to arrive by "
         << sunriseHHMM << " (sunrise)" << endl;
  } else {
    int lateHours = static_cast<int>(-timeToLeaveForHome);
    int lateMins = static_cast<int>((-timeToLeaveForHome - lateHours) * 60);
    cout << "*** YOU SHOULD HAVE LEFT " << timeToEnglish(lateHours, lateMins)
         << " AGO TO GET BACK HOME BY " << civilTwilightHHMM
         << " (CIVIL TWILIGHT END) ***" << endl;

    // If you leave now, when do you arrive home?
    double arrivalHomeTime = currentTime + doubleCommuteHours + workdayHours;
    double afterBy = arrivalHomeTime - civilTwilightEndTime;
    if (afterBy > 0) {
      int afterHours = static_cast<int>(afterBy);
      int afterMins = static_cast<int>((afterBy - afterHours) * 60);
      string arrivalHHMM = format_utils::formatHHMM(arrivalHomeTime);
      cout << "If you leave NOW, you'll be back home at " << arrivalHHMM << " ("
           << timeToEnglish(afterHours, afterMins)
           << " after civil twilight ends)" << endl;
    }
  }

  // Morning twilight table with commute columns
  printMorningTable(solarNoon, latitude, delta, currentTime, commuteMinutes);

  return 0;
}
