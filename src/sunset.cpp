/** 
    Most of this code is developed from James Still's post on calculating solar coordinates:
    https://squarewidget.com/solar-coordinates/
    https://aa.usno.navy.mil/faq/sun_approx
*/

#include <iostream>
#include <iomanip>   
#include <cmath>
#include <ctime>

using namespace std;

const double PI = atan(1)*4;
const double deg2rad = PI / 180.0;
const double rad2deg = 180.0 / PI;

double getSunSize() {
  //const double sunRadius = 0.26667; // Sun's apparent radius, in degrees

  const double au = 149597870700; // Astronomical unit in m
  //  const double sol_radi_m = 6.95700e8; // solar radius in m
  const double sun_radi_m = 696342e3;  // measured solar radius from Mercury transits
  const double sun_diam_m = sun_radi_m*2; // diameter of the sun in m
  const double sun_diam_rad = 2*atan(sun_diam_m/(2*au)); // angular size in radians
  const double sun_diam_deg = sun_diam_rad*rad2deg; // angular size in degrees

  cout << "The angular size of the sun is " << sun_diam_deg << " degrees" << endl;
  
  const double sun_radi_deg = sun_diam_deg/2;  
  return sun_radi_deg;
}

const double sunnum = getSunSize();

double getJulianDate(int year, int month, int day) {
  cout << "the input date is " << year << "-" << month << "-" << day << endl;
  int a = (14 - month) / 12;
  int y = year + 4800 - a;
  int m = month + 12 * a - 3;
  int jd = day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
  cout << "the Julian Date is " << jd << endl;
  return jd + 0.5;  
}

double cent_frac(int jd) {
  // Julian Ephemeris Century
  // Calculate the number of Julian centuries since J2000.0
  double t = (jd - 2451545.0) / 36525.0; // fraction of a century since Jan 1, 2000
  cout << "the Julian Ephemeris Century is " << t << endl;
  return t;
}

// A function to calculate the obliquity of the ecliptic in degrees
double obliquityOfEcliptic(double T) {
  // input is Julian Ephemeris Century
    
  // Calculate the obliquity of the ecliptic in arcseconds
  double epsilon = 84381.448 - 46.815 * T - 0.00059 * T * T + 0.001813 * T * T * T;
    
  // Convert arcseconds to degrees
  epsilon = epsilon / 3600.0;
    
  return epsilon;
}

double getSunset(int year, int month, int day, double latitude, double longitude, int timezone) {
  double jd = getJulianDate(year, month, day);
  double t = cent_frac(jd);

  // Geometric Mean Longitude of the Sun, referred to the mean equinox of the time
  double L = 280.460 + 36000.771 * t;
  double L0 = 280.46646 + t *(36000.76983+t*0.0003032);
  double L5 = 280.4664567 + 36000.76982779 * t + 0.03032028 * pow(t,2) + pow(t,3)/49931-pow(t,4)/15300 - pow(t,5)/2e6;
  cout << "The Geometric Mean Longitude of the Sun is " << endl;
  cout << "\t    linear: L  = " << setprecision(7) <<L << endl;
  cout << "\t quadratic: L0 = ";
  if (L0>=360 || L0<0) {
    cout << L0 << " or " ;
    L0=fmod(L0,360);
  }
  cout << L0 << " degrees" << endl; 
  cout << "\t   quintic: L  = " << L5 << endl;

  // Mean Anomaly of the Sun
  double g = 357.528 + 35999.050 * t;
  double M = 357.52911 + 35999.05029 * t - 0.0001537 * pow(t,2);
  cout << "The Mean Anomaly of the Sun is " << endl;
  cout << "\t    linear: g = " << g << endl;
  cout << "\t quadratic: M = " << M << endl;

  // eccentricity of Earth's orbit (25.4)
  double e = 0.016708634 - (0.000042037 * t) - (0.0000001267 * pow(t, 2));
  cout << "eccentricity = " << e << endl;

  // Sun's equation of center
  double C =
    +(1.914602 - (0.004817 * t) - (0.000014 * pow(t, 2))) * sin(M * deg2rad)
    + (0.019993 - (0.000101 * t)) * sin(M * deg2rad * 2)
    + (0.000289 * sin(M * deg2rad * 3));
  cout << "sun's equation of center = " << C << endl;

  double C2 = 1.915 * sin(g * deg2rad) + 0.020 * sin(2 * g * deg2rad);
  cout << "                        or " << C2 << endl;  
  
  // Sun's true geometric longitude
  double Ltrue = (L0 + C);
  cout << "true geometric longitude = ";
  if (Ltrue>=360 || Ltrue<0) {
    cout << Ltrue << " or ";
    Ltrue = fmod(Ltrue,360);
  }
  cout << Ltrue << " degrees" << endl;

  // Sun's true anomaly
  double n = (M + C);
  cout << "true anomaly = " << n << endl;

  // U.S. Naval Observatory function for radius vector.
  // Compare to Meeus (25.5)
  double R = 1.00014
    - e * cos(M * deg2rad)
    - 0.00014 * cos(2 * M * deg2rad);
  cout << "radius vector = " << R << endl;

  // correction "omega" for nutation and aberration
  double O = 125.04 - (1934.136 * t);

  // apparent longitude L (lambda) of the Sun
  double Lapp = Ltrue - 0.00569 - (0.00478 * sin(O * deg2rad));
  cout << "apparent longitude  = " << Lapp << endl;

  // Calculate the solar declination angle
  double lambda = L + C2;
  cout << "       or " << lambda << " or " << fmod(lambda,360) << endl;

  // obliquity of the ecliptic (22.2)
  double U = t / 100.0;
  double theta1=23.0+(26.0/60.0)+(21.448/pow(60.0,2));
  cout << "theta1 = " << theta1 << endl;
  double otheta1 =   23.439;
  cout << "      or " << otheta1 << endl;
  
  double theta2=4680.93/pow(60.0,2);
  cout << "theta2 = " << theta2 << endl;
  double otheta2 = 0.013 * 100.0;
  cout << "      or " << otheta2 << endl;
  
  double e0 =
    theta1
    - theta2 * U
    - 1.55 * pow(U, 2)
    + 1999.25 * pow(U, 3)
    - 51.38 * pow(U, 4)
    - 249.67 * pow(U, 5)
    - 39.05 * pow(U, 6)
    + 7.12 * pow(U, 7)
    + 27.87 * pow(U, 8)
    + 5.79 * pow(U, 9)
    + 2.45 * pow(U, 10);

  // Print the obliquity of the ecliptic for this date
  cout << "The obliquity of the ecliptic for " << year << "-" << month << "-" << day << " is: " << endl;
  cout << "     cubic: " << obliquityOfEcliptic(t) << " degrees" << endl;
  cout << " or series: " << e0 << endl;

  double epsilon = otheta1 - otheta2 * t/100;
  cout << " or linear: " << epsilon << endl;
  
  // correction for parallax (25.8)
  double eCorrected = e0 + 0.00256 * cos(O * deg2rad);
  cout << "         or " << eCorrected << " corrected for parallax" << endl;

  // Sun's right ascension a
  double a = atan2(
		   cos(eCorrected * deg2rad) * sin(Lapp * deg2rad),
		   cos(Lapp * deg2rad)) *rad2deg ;
  cout << "right ascension = " << a << " degrees" << endl;

  // declination d
  double d = asin(sin(eCorrected * deg2rad) * sin(Lapp * deg2rad)) * rad2deg;
  cout << "declination = " << d << " degrees" << endl;
  double delta = asin(sin(epsilon * deg2rad) * sin(lambda * deg2rad)) * rad2deg;
  cout << "         or = " << delta << " degrees" << endl;
  

  // Calculate the local hour angle
  double zenith = 90.0 - sunnum; //getSunSize();
  double cosz = cos(zenith * deg2rad);
  double sinz = sin(zenith * deg2rad);

  double cosphi = cos(latitude * deg2rad);
  double sinphi = sin(latitude * deg2rad);

  double cosdelta = cos(delta * deg2rad);
  double sindelta = sin(delta * deg2rad);

  double cosH = (cosz - sindelta * sinphi) / (cosdelta * cosphi);
  double H = acos(cosH) * rad2deg;
  cout << "equation of time = " << H << " degrees" << endl;
  cout << "                or " << H/15.0 << " hours" << endl;

  // Convert to local solar time
  double solarNoon = 12.0 - longitude / 15.0 - timezone;
  cout << "solar noon = " << solarNoon << endl;
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
  int timezone = -6;

  // Calculate the sunset time
  double sunset = getSunset(year, month, day, latitude, longitude, timezone);
  int hours = static_cast<int>(sunset);
  int minutes = static_cast<int>((sunset - hours) * 60);

  // Print the result
  cout << "Sunset time: " << hours << ":" << minutes << endl;

  return 0;
}
