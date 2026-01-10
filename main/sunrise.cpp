#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "colors.h"
#include "config.h"
#include "constants.h"
#include "format_utils.h"
#include "morning_table.h"
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
  double sunriseTime = calc.getSunrise(year, month, day, latitude, longitude,
                                       set_timezone, &solarNoon, &delta);

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

  cout << endl;
  if (timeToLeave > 0) {
    int leaveHours = static_cast<int>(timeToLeave);
    int leaveMins = static_cast<int>((timeToLeave - leaveHours) * 60);
    cout << "Leave by " << leaveHHMM << " (in "
         << timeToEnglish(leaveHours, leaveMins) << ") to arrive by "
         << sunriseHHMM << " (sunrise)" << endl;
  } else {
    int lateHours = static_cast<int>(-timeToLeave);
    int lateMins = static_cast<int>((-timeToLeave - lateHours) * 60);
    cout << "*** YOU SHOULD HAVE LEFT " << timeToEnglish(lateHours, lateMins)
         << " AGO TO ARRIVE BY " << sunriseHHMM << " (SUNRISE) ***" << endl;

    // If you leave now, when do you arrive?
    double arrivalTime = currentTime + commuteHours;
    double afterBy = arrivalTime - sunriseTime;
    if (afterBy > 0) {
      int afterHours = static_cast<int>(afterBy);
      int afterMins = static_cast<int>((afterBy - afterHours) * 60);
      string arrivalHHMM = format_utils::formatHHMM(arrivalTime);
      cout << "If you leave NOW, you'll arrive at " << arrivalHHMM << " ("
           << timeToEnglish(afterHours, afterMins) << " after sunrise)" << endl;
    }
  }

  // Morning twilight table with commute columns
  printMorningTable(solarNoon, latitude, delta, currentTime, commuteMinutes);

  return 0;
}
