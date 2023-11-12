/** 
    Most of this code is developed from James Still's post on calculating solar coordinates:
    https://squarewidget.com/solar-coordinates/
    with references to 
    https://aa.usno.navy.mil/faq/sun_approx
    Meeus, Jean (1991). Astronomical Algorithms.
*/

#include <iostream>
#include <iomanip>   
#include <cmath>
#include <ctime>

using namespace std;

// settings
const int debug=2;

// define constants
const double PI = atan(1)*4;
const double deg2rad = PI / 180.;
const double rad2deg = 180. / PI;

double getJulianDate(int year, int month, int day) {
  cout << "the input date is " << year << "-" << month << "-" << day << endl;
  int a = (14 - month) / 12;
  int y = year + 4800 - a;
  int m = month + 12 * a - 3;
  int jd = day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
  cout << "the Julian Date is " << jd << endl;
  return jd; // are we supposed to return noon?  
}

double getJ2000 (double jd) {
  // convert Julian Date to J2000 epoch, that is, the Julian Date since Jan 1, 2000
  double J2000 = jd - 2451545.0;
  cout << " the J2000 date is " << J2000 << endl;
  return J2000;
}

double getJulianCentury (double J2000) {
  // convert J2000 date to Julian Ephemeris Century, that is, fraction of a Julian century 
  double T = J2000  / 36525.;
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
  // Meeus pg. 163, Eq. 25.2
  // based on J2000 longitude
  double L_1 = 280.460     + 36000.771      * t; // USNO
  double L_3 = 280.46646   + 36000.76983    * t + 0.0003032  * pow(t,2); // NOAA
  double L_5 = 280.4664567 + 36000.76982779 * t + 0.03032028 * pow(t,2) + pow(t,3)/49931-pow(t,4)/15300 - pow(t,5)/2e6;
  if (debug>0) {
    cout << "The Geometric Mean Longitude of the Sun is " << endl;
    cout << "\t    linear: L = " << setprecision(7); printDeg(L_1); cout << " (USNO)" << endl;
    cout << "\t quadratic: L = "; printDeg(L_3); cout << " (NOAA)" << endl;
    cout << "\t   quintic: L = "; printDeg(L_5); cout << endl;
  }
  return fmod(L_3,360);
}

double meanAnomaly(double t) {
  // Mean Anomaly of the Sun
  //Meeus pg. 163, Eq. 25.3
  double M_1 = 357.528   + 35999.050   * t;
  double M_2 = 357.52911 + 35999.05029 * t - 0.0001537 * pow(t,2); // NOAA
  if (debug>0) {
    cout << "The Mean Anomaly of the Sun is " << endl;
    cout << "\t    linear: M = "; printDeg(M_1); cout << " (USNO)" << endl;
    cout << "\t quadratic: M = "; printDeg(M_2); cout << " (NOAA)" << endl;
  }
  return fmod(M_2,360.0);
}

double equationOfCenter(double t, double M) {
  // Sun's equation of center
  // Geocentric apparent ecliptic longitude of the Sun (adjusted for aberration):

  // convert inputs to radians
  M*=deg2rad;

  double C_0 = 1.915 * sin(M)
    + 0.020 * sin(2*M);

  double C_2 = (1.914602 - (0.004817 * t) - (0.000014 * pow(t,2))) * sin(M)
    + (0.019993 - (0.000101 * t)) * sin(2*M)
    + (0.000289 * sin(3*M));

  if (debug>0) {
    cout << "The Sun's equation of center" << endl;
    cout << "\t  constant: C = " << C_0 << " (USNO)" << endl;
    cout << "\t quadratic: C = " << C_2 << " (NOAA)" << endl;
  }
  return C_2;
}

double nuation(double t) {
  // Longitude of the ascending node of the Moon's mean orbit on the ecliptic, measured form the
  // mean equinox of the date. Used to correction for nutation and aberration.
  double omega_1 = 125.04    - 1934.136 * t;
  double omega_3 = 125.04452 - 1934.136261 * t + 0.0020708 * pow(t,2) + pow(t,3) / 450000;

  if (debug>0) {
    cout << "Nuation" << endl;
    cout << "\t linear: omega = " << omega_1 << " degrees (NOAA)" << endl;
    cout << "\t  cubic: omega = " << omega_3 << " degrees" << endl;
  }
  return omega_1*deg2rad;
}

double eccentricity(double t) {
  // eccentricity of Earth's orbit
  // input is Julian century
  // Meeus pg. 163, Eq. 25.4
  double e = 0.016708634 - (0.000042037 * t) - (0.0000001267 * pow(t, 2));
  
  if (debug>0) {
    cout << "eccentricity = " << e << endl;
  }
  return e;
}

double radiusVector(double e, double nu) {
  // the distance between the centres of the Sun and the Earth, in au
  // U.S. Naval Observatory function for radius vector.
  // c.f. Meeus Eq. 25.5

  // convert inputs to radians
  nu*=deg2rad;

  double R = 1.00014
    - e * cos(nu)
    - 0.00014 * cos(2*nu);

  // initial value for the semi-major axis for J2000
  // Bretagnon, P.: 1982, Astron. Astrophys. 114, 278
  double a = 1.00000101778; // in au
  
  double R_NOAA = (a * (1 - pow(e,2))) / (1 + e * cos(nu));

  if (debug>0) {
    cout << "radius vector" << endl;
    cout << "\t R = " << R << " au (USNO)" << endl;
    cout << "\t  or " << R_NOAA << " au (NOAA)" << endl;
  }
  return R_NOAA;
}

double getSunSize(double rad_vec_au = 1) {
  // angular semidiameter of the Sun, in degrees
  // Sun's apparent radius
  // input is radial vector in au
  
  // constants
  const double au = 149597870700; // Astronomical unit in m

  // physical size of the sun
  // measured solar radius from Mercury transits
  // https://arxiv.org/abs/1203.4898
  // 696,342 ± 65km
  const double sun_radi_m = 696342e3; 
  if (debug>1) {
    const double sol_radi_m = 6.95700e8; // solar radius in m
    cout << "The radius of the sun is" << "\n\t " << sun_radi_m/sol_radi_m << " R_sol" << endl;
  }
  const double sun_diam_m = sun_radi_m*2; // diameter of the sun in m

  // distance to the sun
  // convert radial vector to m
  const double rad_vec_m=rad_vec_au*au;
 
  // apparent size of the sun
  const double sun_diam_rad = 2*atan(sun_diam_m/(2*rad_vec_m)); // angular size in radians
  const double sun_diam_deg = sun_diam_rad*rad2deg; // angular size in degrees

  if (debug>0) {
    cout << "The angular size of the sun is " << endl;
    cout << "\t" << sun_diam_deg << " degrees";
    cout << " or " << sun_diam_deg*60 << " arcminutes"; //should be between ~31.5 32.5 arcminutes
    cout << endl;
  }
  
  // return semidiameter
  const double sun_radi_deg = sun_diam_deg/2;  
  return sun_radi_deg;
  //const double sunRadius = 0.26667; 
}

double dms2deg(double deg, double min, double sec) {
  double ang = deg
    + min/60.
    + sec/3600.;
  if (debug>0) {
    if (deg != 0)
      cout << deg << "\u00b0";
    if ((deg != 0) && (min != 0))
      cout << min << "'";
    cout << sec << "'' = " << ang << " degrees" << endl;
  }
  return ang;
}

// A function to calculate the obliquity of the ecliptic in degrees
double obliquityOfEcliptic(double T) {
  // obliquity of the ecliptic (Meesus Eq. 22.2)
  // All of these expressions are for the mean obliquity, that is, without the nutation of the
  // equator included.
  // input is Julian Ephemeris Century
  // output in degrees
    
  // define reference angles
  // the initial values adopted by the JAU (Grenoble, 1976):
  double const epsilon0 = dms2deg(23,26,21.448);
    
  // This uses Laskar’s tenth-degree polynomial fit:
  // Laskar, J., Astronomy and Astrophysics, 157: 68 (1986).
  // Table 8. Formulas for the precession. The obliquity is given in arcseconds and the time t is
  // measured in units of 10,000 Julian years from J2000 (JD 2451545.0). NGT denotes our solution
  // (Numerical General Theory). L denotes the solution of Lieske et al. (1977).

  // convert Julian centuries to 10,000 Julian years
  double const U = T / 100.0;

  // Calculate the obliquity of the ecliptic in arcseconds
  double const t0 = epsilon0 * 3600.;
  double const t = 4680.93; // the O(1) term is included here for reference to earlier versions

  if (debug > 1) {
    cout << "epsilon0 = " << epsilon0 << " degrees" << endl;
    cout << "        or " << t0 << " arcseconds" << endl;
    double const theta2 = dms2deg(0,0,t);
    cout << "  theta2 = " << theta2 << endl;
    cout << "        or " << t << endl;
  }
  
  double epsilon_1 = t0 - t * U; // USNO
  
  // Lieske et al. (1977).
  // Used by NOAA  
  // From 1984, the Jet Propulsion Laboratory's DE series of computer-generated ephemerides took
  // over as the fundamental ephemeris of the Astronomical Almanac.
  double epsilon_L = t0
    - 4681.5 * U
    - 5.9 * pow(U,2)
    + 1813 * pow(U,3);
  
  double epsilon_NGT = epsilon_1
    - 1.55 * pow(U, 2)
    + 1999.25 * pow(U, 3)
    - 51.38 * pow(U, 4)
    - 249.67 * pow(U, 5)
    - 39.05 * pow(U, 6)
    + 7.12 * pow(U, 7)
    + 27.87 * pow(U, 8)
    + 5.79 * pow(U, 9)
    + 2.45 * pow(U, 10);
  
  // Convert arcseconds to degrees
  epsilon_1 *= 1/3600.;
  epsilon_L *= 1/3600.;
  epsilon_NGT *= 1/3600.;  

  if (debug>0) {
    cout << "The obliquity of the ecliptic is " << endl;
    cout << "\t linear: "; printDeg(epsilon_1); cout << " (USNO)" << endl;
    cout << "\t  cubic: "; printDeg(epsilon_L); cout << " Lieske et al. 1977 (NOAA)" << endl;
    cout << "\t 10poly: " << epsilon_NGT << " Laskar 1985" << endl;
  }
   
  return epsilon_L;
}

double equationOfTime1(double e, double nu, double latitude, double delta) {
  // Calculate the local hour angle
  
  // convert inputs to radians
  latitude*=deg2rad;
  delta*=deg2rad;  

  // get the apparent size of the sun
  double R = radiusVector(e,nu);
  getSunSize(R);
  const double sun_radi_deg = getSunSize();
  
  double zenith = 90.0 - sun_radi_deg;
  zenith*=deg2rad;
  double cosz = cos(zenith);
  //double sinz = sin(zenith);

  double cosphi = cos(latitude);
  double sinphi = sin(latitude);

  double cosdelta = cos(delta);
  double sindelta = sin(delta);

  double cosH = (cosz - sindelta * sinphi) / (cosdelta * cosphi);
  double H = acos(cosH) * rad2deg;
  if (debug>0) {
    cout << "equation of time = " << H << " degrees" << endl;
    cout << "                or " << H/15.0 << " hours" << endl;
  }
  return H;
}

double equationOfTime2(double q, double RA) {
  double EqT = q/15.0 - RA; //USNO
  if (debug>0) {
    cout << "equation of time = " << EqT << endl;
  }
  return EqT;
}

double equationOfTime3(double y,double L,double e,double M) {
  // We use the approximation of the equation of time given by W.M. Smart, Text-Book on Spherical
  // Astronomy, Cambridge University Press, 1956, p. 149:

  // convert inputs to radians
  L*=deg2rad;
  M*=deg2rad;

  if (debug > 1) {
    // print individual terms
    cout << y*sin(2*L)<< endl;
    cout << -2*e*sin(M)<< endl;
    cout << +4*e*y*sin(M)*cos(2*L)<< endl;
    cout << -(1/2.)*pow(y,2)*sin(4*L)<< endl;
    cout << -(5/4.)*pow(e,2)*sin(2*M)<< endl;
  }
  
  double E = y*sin(2*L)
    -2*e*sin(M)
    +4*e*y*sin(M)*cos(2*L)
    -(1/2.)*pow(y,2)*sin(4*L)
    -(5/4.)*pow(e,2)*sin(2*M);

  if (debug>0) {
    cout << "equation of time" << endl;
    cout << "\t E = " << E << " radians" << endl;
    cout << "\t E = " << E*rad2deg << " degrees" << endl;
    cout << "\t E = " << E*rad2deg*4 << " minutes" << endl;
  }
  return E;
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
 
  // nuation
  double omega=nuation(t);
  
  // apparent longitude L (lambda) of the Sun
  // true equinox of the date
  double lambda = l - 0.00569 - (0.00478 * sin(omega));
  cout << "  apparent longitude  = " << lambda << endl;
  lambda*=deg2rad;

  double epsilon=obliquityOfEcliptic(t);
 
  // correction for parallax (Meeus Eq. 25.8)
  // The true or instantaneous obliquity includes the nutation.
  double eCorrected = epsilon + 0.00256 * cos(omega);
  cout << "              or " << eCorrected << " including nutation" << endl;
 
  // Sun's right ascension a
  double alpha = atan2(
		       cos(eCorrected * deg2rad) * sin(lambda),
		       cos(lambda)) *rad2deg ;

  // Calculate the solar declination angle
  // declination d or delta
  double delta = asin(sin(eCorrected * deg2rad) * sin(lambda)) * rad2deg;
  double delta2 = asin(sin(epsilon * deg2rad) * sin(l * deg2rad)) * rad2deg;

  if (debug>0) {
    cout << "Solar coordinates:" << endl;
    cout << "\tright ascension = " << alpha << " degrees" << endl;
    cout << "\t    declination = " << delta << " degrees (NOAA)" << endl;
    cout << "\t             or = " << delta2 << " degrees" << endl;
  }

  double e = eccentricity(t);
  double H=equationOfTime1(e,nu,latitude,delta);
  double EqT=equationOfTime2(L,alpha);

  if (debug > 1) {
    double e2 = eCorrected / 2.0 * deg2rad;
    cout << "e/2 rad = " << e2  << endl;
    double te2 = tan(e2);
    cout << "tan e/2 = " << te2 << endl;
    cout << "y = " << te2*te2 << endl;
  }

  double y = pow(tan(eCorrected / 2.0 * deg2rad),2);
  cout << "y = " << y << endl;
  double E=equationOfTime3(y,L,e,M);

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
