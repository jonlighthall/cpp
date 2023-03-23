#include <iostream>
#include <cmath>
#include <ctime>

using namespace std;

const double PI = 3.14159265;
const double deg2rad = PI / 180.0;
const double rad2deg = 180.0 / PI;
const double sunRadius = 0.26667; // Sun's apparent radius, in degrees

double getJulianDate(int year, int month, int day) {
  int a = (14 - month) / 12;
  int y = year + 4800 - a;
  int m = month + 12 * a - 3;
  int jd = day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
  return jd + 0.5;
}

double getSunset(int year, int month, int day, double latitude, double longitude, int timezone) {
  double jd = getJulianDate(year, month, day);
  double t = (jd - 2451545.0) / 36525.0;

  // Calculate the solar declination angle
  double L = 280.460 + 36000.771 * t;
  double g = 357.528 + 35999.050 * t;
  double lambda = L + 1.915 * sin(g * deg2rad) + 0.020 * sin(2 * g * deg2rad);
  double epsilon = 23.439 - 0.013 * t;
  double delta = asin(sin(epsilon * deg2rad) * sin(lambda * deg2rad)) * rad2deg;

  // Calculate the local hour angle
  double zenith = 90.0 - sunRadius;
  double cosz = cos(zenith * deg2rad);
  double sinz = sin(zenith * deg2rad);
  double cosphi = cos(latitude * deg2rad);
  double sinphi = sin(latitude * deg2rad);
  double cosdelta = cos(delta * deg2rad);
  double sindelta = sin(delta * deg2rad);
  double cosH = (cosz - sindelta * sinphi) / (cosdelta * cosphi);
  double H = acos(cosH) * rad2deg;

  // Convert to local solar time
  double solarNoon = 12.0 - longitude / 15.0 - timezone;
  double sunsetTime = solarNoon - H / 15.0;

  return sunsetTime;
}

int main() {
  // Get the current date and time
  time_t now = time(0);
  tm *ltm = localtime(&now);
  int year = ltm->tm_year + 1900;
  int month = ltm->tm_mon + 1;
  int day = ltm->tm_mday;

  // Set the location and timezone
  double latitude = 30.4275784357249; // New Orleans
  double longitude = -90.0914955109431;
  int timezone = -5;

  // Calculate the sunset time
  double sunset = getSunset(year, month, day, latitude, longitude, timezone);
  int hours = static_cast<int>(sunset);
  int minutes = static_cast<int>((sunset - hours) * 60);

  // Print the result
  cout << "Sunset time: " << hours << ":" << minutes << endl;

  return 0;
}
