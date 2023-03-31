#include <iostream>
#include <cmath>
#include <ctime>

using namespace std;

const double PI = 3.14159265;
const double deg2rad = PI / 180.0;
const double rad2deg = 180.0 / PI;
const double sunRadius = 0.26667; // Sun's apparent radius, in degrees

double getJulianDate(int year, int month, int day) {
  cout << "the input date is " << year << "-" << month << "-" << day << endl;
  int a = (14 - month) / 12;
  int y = year + 4800 - a;
  int m = month + 12 * a - 3;
  int jd = day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
  cout << "the Julian Date is " << jd << endl;
  return jd + 0.5;
  
}

double getSunset(int year, int month, int day, double latitude, double longitude, int timezone) {
  double jd = getJulianDate(year, month, day);
  double t = (jd - 2451545.0) / 36525.0; // fraction of a century since Jan 1, 2000

  // Geometric Mean Longitude of the Sun
  double L0 = 280.46646 + t *(36000.76983+t*0.0003032)  ;
  cout << "The Geometric Mean Longitude of the Sun is " << L0 << " or " << fmod(L0,360) << " degrees" << endl; 

  double L = 280.460 + 36000.771 * t;
  cout << "L = " << L << endl;

  // Mean Anomaly of the Sun
  double g = 357.528 + 35999.050 * t;

  double M =357.52911+t*(35999.05029 - 0.0001537*t);
  cout << "The Mean Anomaly of the Sun is " << M << " or " << g << endl;

  //// eccentricity of Earth's orbit (25.4)
  double e = 0.016708634 - (0.000042037 * t) - (0.0000001267 * pow(t, 2));

  // Sun's equation of center
  double C =
    +(1.914602 - (0.004817 * t) - (0.000014 * pow(t, 2))) * sin(M * deg2rad)
    + (0.019993 - (0.000101 * t)) * sin(M * deg2rad * 2)
    + (0.000289 * sin(M * deg2rad * 3));

  // Sun's true geometric longitude
  double Ltrue = (L0 + C);
  Ltrue = Ltl0rue.CorrectDegreeRange();

  // Sun's true anomaly
  double n = (M + C);
  n = n.CorrectDegreeRange();

  // U.S. Naval Observatory function for radius vector.
  // Compare to Meeus (25.5)
  double R = 1.00014
    - 0.01671 * cos(M * deg2rad)
    - 0.00014 * cos(2 * M * deg2rad);

  // correction "omega" for nutation and aberration
  double O = 125.04 - (1934.136 * t);

  // apparent longitude L (lambda) of the Sun
  double Lapp = Ltrue - 0.00569 - (0.00478 * sin(O * deg2rad));

  // obliquity of the ecliptic (22.2)
  double U = t / 100;
  double e0 =
    new Angle(23, 26, 21.448).Degrees
    - new Angle(0, 0, 4680.93).Degrees * U
    - 1.55 * pow(U, 2)
    + 1999.25 * pow(U, 3)
    - 51.38 * pow(U, 4)
    - 249.67 * pow(U, 5)
    - 39.05 * pow(U, 6)
    + 7.12 * pow(U, 7)
    + 27.87 * pow(U, 8)
    + 5.79 * pow(U, 9)
    + 2.45 * pow(U, 10);


  // correction for parallax (25.8)
  double eCorrected = e0 + 0.00256 * cos(O * deg2rad);

  // Sun's right ascension a
  double a = atan2(
			cos(eCorrected * deg2rad) * sin(Lapp * deg2rad),
			cos(Lapp * deg2rad));

  // declination d
  double d = Math.Asin(sin(eCorrected * deg2rad) * sin(Lapp * deg2rad));

  // solar coordinates
  RightAscension ra = new RightAscension(a.ToDegrees());
  Angle dec = new Angle(d.ToDegrees());

  // Calculate the solar declination angle
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
