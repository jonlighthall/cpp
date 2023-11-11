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

// define constants
const double PI = atan(1)*4;
const double deg2rad = PI / 180.0;
const double rad2deg = 180.0 / PI;

double getJulianDate(int year, int month, int day) {
  cout << "the input date is " << year << "-" << month << "-" << day << endl;
  int a = (14 - month) / 12;
  int y = year + 4800 - a;
  int m = month + 12 * a - 3;
  int jd = day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
  cout << "the Julian Date is " << jd << endl;
  return jd + 0.5;  
}

double getJ2000 (double jd) {
  // convert Julian Date to J2000 epoch, that is, the Julian Date since Jan 1, 2000
  double J2000 = jd - 2451545.0;
  cout << " the J2000 date is " << J2000 << endl;
  return J2000;
}

double getJulianCentury (double J2000) {
  // convert J2000 date to Julian Ephemeris Century, that is, fraction of a Julian century 
  double T = J2000  / 36525.0;
  cout << "the Julian Ephemeris Century is " << T << endl;
  return T;
}

void printDeg(double theta) {
  cout << theta;
  if (theta>=360 || theta<0) {
    cout << " or " << fmod(theta,360);
  }
  cout << " degrees";
}

double meanLongitude(double t) {
  // Geometric Mean Longitude of the Sun
  // referred to the mean equinox of the time
  // calculated in degrees
  // Meeus pg. 163, eq. 25.2
  // based on J2000 longitude
  double L_1 = 280.460     + 36000.771      * t;
  double L_3 = 280.46646   + 36000.76983    * t + 0.0003032  * pow(t,2); // NOAA
  double L_5 = 280.4664567 + 36000.76982779 * t + 0.03032028 * pow(t,2) + pow(t,3)/49931-pow(t,4)/15300 - pow(t,5)/2e6;
  cout << "The Geometric Mean Longitude of the Sun is " << endl;
  cout << "\t    linear: L = " << setprecision(7); printDeg(L_1); cout << endl;
  cout << "\t quadratic: L = "; printDeg(L_3); cout << " (NOAA)" << endl;
  cout << "\t   quintic: L = "; printDeg(L_5); cout << endl;
  return fmod(L_3,360);
}

double meanAnomaly(double t) {
  // Mean Anomaly of the Sun
  //Meeus pg. 163, eq. 25.3
  double M_1 = 357.528   + 35999.050   * t;
  double M_2 = 357.52911 + 35999.05029 * t - 0.0001537 * pow(t,2); // NOAA
  cout << "The Mean Anomaly of the Sun is " << endl;
  cout << "\t    linear: M = "; printDeg(M_1); cout << endl;
  cout << "\t quadratic: M = "; printDeg(M_2); cout << " (NOAA)" << endl;
  return fmod(M_2,360.0);
}

double equationOfCenter(double t, double M) {
  // Sun's equation of center
  // Geocentric apparent ecliptic longitude of the Sun (adjusted for aberration):
  double C_0 = 1.915 * sin(M * deg2rad)
    + 0.020 * sin(2 * M * deg2rad);

  double C_2 = (1.914602 - (0.004817 * t) - (0.000014 * pow(t, 2))) * sin(M * deg2rad)
    + (0.019993 - (0.000101 * t)) * sin(2 * M * deg2rad)
    + (0.000289 * sin(3 * M * deg2rad));

  cout << "The Sun's equation of center" << endl;
  cout << "\t  constant: C = " << C_0 << endl;
  cout << "\t quadratic: C = " << C_2 << " (NOAA)" <<endl;
  
  return C_2;
}

double eccentricity(double t) {
  // eccentricity of Earth's orbit
  // input is Julian century
  // Meeus pg. 163, eq. 25.4
  double e = 0.016708634 - (0.000042037 * t) - (0.0000001267 * pow(t, 2));
  cout << "eccentricity = " << e << endl;
  return e;
}

double radiusVector(double e, double nu) {
  // U.S. Naval Observatory function for radius vector.
  // Compare to Meeus (25.5)
  double M = nu;
  double R = 1.00014
    - e * cos(M * deg2rad)
    - 0.00014 * cos(2 * M * deg2rad);

  double R_NOAA = (1.000001018 * (1 - pow(e,2))) / (1 + e * cos(nu*deg2rad));
  cout << "radius vector" << endl;
  cout << "\t R = " << R << " au" << endl;

  cout << "\t  or " << R_NOAA << " au" << endl;  
  return R_NOAA;
}

double getSunSize(double rad_vec_au = 1) {
  // angular semidiameter of the Sun, in degrees
  // Sun's apparent radius
  // input is radial vector in au
  
  // constants
  const double au = 149597870700; // Astronomical unit in m
  const double sol_radi_m = 6.95700e8; // solar radius in m

  // physical size of the sun
  // measured solar radius from Mercury transits
  // https://arxiv.org/abs/1203.4898
  // 696,342 ± 65km
  const double sun_radi_m = 696342e3;
  cout << "The radius of the sun is" << "\n\t " << sun_radi_m/sol_radi_m << " R_sol" << endl;
  const double sun_diam_m = sun_radi_m*2; // diameter of the sun in m

  // distance to the sun
  // convert radial vector to m
  const double rad_vec_m=rad_vec_au*au;

 
  // apparent size of the sun
  const double sun_diam_rad = 2*atan(sun_diam_m/(2*rad_vec_m)); // angular size in radians
  const double sun_diam_deg = sun_diam_rad*rad2deg; // angular size in degrees

  cout << "The angular size of the sun is " << endl;
  cout << "\t" << sun_diam_deg << " degrees";
  cout << " or " << sun_diam_deg*60 << " arcminutes"; //should be between ~31.5 32.5 arcminutes
  cout << endl;
  
  // return semidiameter
  const double sun_radi_deg = sun_diam_deg/2;  
  return sun_radi_deg;
  //const double sunRadius = 0.26667; 
}

// A function to calculate the obliquity of the ecliptic in degrees
double obliquityOfEcliptic(double T) {
  // input is Julian Ephemeris Century
    
  // define reference angles
  double theta1=23.0+(26.0/60.0)+(21.448/pow(60.0,2));
  cout << "theta1 = " << theta1 << endl;
  double otheta1 =   23.439;
  cout << "      or " << otheta1 << endl;
  
  double theta2=4680.93/pow(60.0,2);
  cout << "theta2 = " << theta2 << endl;
  double otheta2 = 0.013 * 100.0;
  cout << "      or " << otheta2 << endl;
  
  double epsilon_1 = otheta1 - otheta2 * T/100;
  cout << " or linear: " << epsilon_1 << endl;

  // Calculate the obliquity of the ecliptic in arcseconds
  double epsilon_NOAA = 84381.448 - 46.815 * T - 0.00059 * T * T + 0.001813 * T * T * T;
    
  // Convert arcseconds to degrees
  epsilon_NOAA = epsilon_NOAA / 3600.0;

  // obliquity of the ecliptic (22.2)
  double U = T / 100.0;
  
  double epsilon_10 =
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
  
  cout << "     cubic: " << epsilon_NOAA << " degrees" << endl;
  cout << " or series: " << epsilon_10 << endl;
   
  return epsilon_NOAA;
}



double getSunset(int year, int month, int day, double latitude, double longitude, int timezone) {
  // date
  double jd = getJulianDate(year, month, day);
  double j2000 = getJ2000(jd);
  double t = getJulianCentury(j2000);

  // solar coordinates
  double L = meanLongitude(t);
  double M = meanAnomaly(t);
  
  double C=equationOfCenter(t,M);

  // Sun's true geometric longitude
  double l = (L + C);
  cout << "true longitude" << endl;
  cout << "\t            l = "; printDeg(l); cout << endl;
 
  // Sun's true anomaly
  double nu = (M + C);
  cout << "true anomaly" << endl;
  cout << "\t           nu = "; printDeg(nu); cout << endl;
 
  double e = eccentricity(t);
  double R = radiusVector(e,nu);
  getSunSize();
  getSunSize(R);

  // correction "omega" for nutation and aberration
  double O = 125.04 - (1934.136 * t);

  // apparent longitude L (lambda) of the Sun
  double Lapp = l - 0.00569 - (0.00478 * sin(O * deg2rad));
  cout << "apparent longitude  = " << Lapp << endl;

  // Calculate the solar declination angle
  
  double epsilon=obliquityOfEcliptic(t);

  // Print the obliquity of the ecliptic for this date
  cout << "The obliquity of the ecliptic for " << year << "-" << month << "-" << day << " is: " << endl;
  
  // correction for parallax (25.8)
  double eCorrected = epsilon + 0.00256 * cos(O * deg2rad);
  cout << "         or " << eCorrected << " corrected for parallax" << endl;

  // Sun's right ascension a
  double alpha = atan2(
		       cos(eCorrected * deg2rad) * sin(Lapp * deg2rad),
		       cos(Lapp * deg2rad)) *rad2deg ;
  cout << "right ascension = " << alpha << " degrees" << endl;

  // declination d
  double delta = asin(sin(eCorrected * deg2rad) * sin(Lapp * deg2rad)) * rad2deg;
  cout << "declination = " << delta << " degrees" << endl;
  double delta2 = asin(sin(epsilon * deg2rad) * sin(l * deg2rad)) * rad2deg;
  cout << "         or = " << delta2 << " degrees" << endl;

  // Calculate the local hour angle
  const double sun_radi_deg = getSunSize();
  double zenith = 90.0 - sun_radi_deg;
  double cosz = cos(zenith * deg2rad);
  //double sinz = sin(zenith * deg2rad);

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
