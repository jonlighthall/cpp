/** 
    Most of this code is developed from James Still's post on calculating solar coordinates:
    https://squarewidget.com/solar-coordinates/
    with references to 
    https://aa.usno.navy.mil/faq/sun_approx
    Meeus, Jean (1991). Astronomical Algorithms.
*/

#include <cmath>
#include <ctime>
#include <iomanip>   
#include <iostream>

using namespace std;

// settings
const int debug=1;

// define constants
const double PI = atan(1)*4;
const double deg2rad = PI / 180.;
const double rad2deg = 180. / PI;

double getJulianDate(int year, int month, int day) {
  cout << "The input date is " << year << "-" << month << "-" << day << endl;
  int a = (14 - month) / 12;
  int y = year + 4800 - a;
  int m = month + 12 * a - 3;
  int jd = day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
  cout << "\tthe Julian date is " << jd << endl;
  return jd; // are we supposed to return noon?  
}

double getJ2000 (double jd) {
  // convert Julian Date to J2000 epoch, that is, the Julian Date since Jan 1, 2000
  double J2000 = jd - 2451545.0;
  cout << "\tthe J2000 date is " << J2000 << endl;
  return J2000;
}

double getJulianCentury (double J2000) {
  // convert J2000 date to Julian Ephemeris Century, that is, fraction of a Julian century 
  double T = J2000  / 36525.;
  cout << "\tthe Julian century is " << T << endl;
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
  // referred to the mean equinox of the time or sidereal time
  // calculated in degrees
  // Meeus pg. 163, Eq. 25.2
  // based on J2000 longitude
  double L_1 = 280.460     + 36000.771      * t; // USNO
  double L_3 = 280.46646   + 36000.76983    * t + 0.0003032  * pow(t,2); // NOAA
  double L_5 = 280.4664567 + 36000.76982779 * t + 0.03032028 * pow(t,2) + pow(t,3)/49931-pow(t,4)/15300 - pow(t,5)/2e6;
  if (debug>0) {
    cout << "The Geometric Mean Longitude of the Sun is " << endl;
    cout << "\t   linear: L = " << setprecision(7); printDeg(L_1); cout << " (USNO)" << endl;
    cout << "\tquadratic: L = "; printDeg(L_3); cout << " (NOAA)" << endl;
    cout << "\t  quintic: L = "; printDeg(L_5); cout << endl;
  }
  return fmod(L_3,360);
}

double meanAnomaly(double t) {
  // Mean Anomaly of the Sun
  //Meeus pg. 163, Eq. 25.3
  double M_1 = 357.528   + 35999.050   * t;
  double M_2 = 357.52911 + 35999.05029 * t - 0.0001537 * pow(t,2); // NOAA
  double M_3 = 357.52772 + 35999.050340 *t - 0.0001603 * pow(t,2) + pow(t,3)/189474;
  if (debug>0) {
    cout << "The Mean Anomaly of the Sun is " << endl;
    cout << "\t   linear: M = "; printDeg(M_1); cout << " (USNO)" << endl;
    cout << "\tquadratic: M = "; printDeg(M_2); cout << " (NOAA)" << endl;
    cout << "\t    cubic: M = "; printDeg(M_3); cout << " " << endl;
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
    cout << "\t constant: C = " << C_0 << " (USNO)" << endl;
    cout << "\tquadratic: C = " << C_2 << " (NOAA)" << endl;
  }
  return C_2;
}

double longitudeAscendingNode(double t) {
  // Longitude of the ascending node of the Moon's mean orbit on the ecliptic, measured form the
  // mean equinox of the date. Used to correction for nutation and aberration.
  double Omega_1 = 125.04    - 1934.136 * t;
  double Omega_3 = 125.04452 - 1934.136261 * t + 0.0020708 * pow(t,2) + pow(t,3) / 450000;

  if (debug>0) {
    cout << "Longitude of the ascending node" << endl;
    cout << "\t  lin: Omega = " << Omega_1 << " degrees (NOAA)" << endl;
    cout << "\tcubic: Omega = " << Omega_3 << " degrees" << endl;
  }
  return Omega_1*deg2rad;
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

  // return semidiameter
  const double sun_radi_deg = sun_diam_deg/2;  
  
  if (debug>0) {
    cout << "The angular size of the sun is " << endl;
    cout << "\t" << sun_diam_deg << " degrees";
    cout << " or " << sun_diam_deg*60 << " arcminutes" << endl; //should be between ~31.5 32.5 arcminutes
    cout << "\t" << sun_radi_deg << " degrees (semidiameter)" << endl;
  }
  
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

double equationOfTime2(double M, double alpha, double DPsi, double epsilon) {
  //  double EqT = q/15.0 - RA; //USNO
  double EqT = M - alpha; //degrees
  if (debug>0) {
    cout << "EqT = " << EqT << " degrees" <<endl;
    cout << "EqT = " << EqT*4 << " minutes" <<endl;
  }

  double E = M - 0.0057183 - alpha + DPsi * cos(epsilon);
  cout << "E = " << E << " degrees" <<endl;
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
  return E*rad2deg/15; // hours
}

double getZenith(double e, double nu) {
  
  // get the apparent size of the sun
  cout << "sun size:" << endl;
  double R = radiusVector(e,nu);

  cout << "default: ";
  getSunSize();
  
  cout << "calculated: ";
  const double sun_radi_deg = getSunSize(R);

  cout << "sun elevation h0" << endl;
  cout << "default: " << endl;
  cout << "\t " << -0.833 << " degrees" << endl;
  
  const double atmo_refrac = 0.5667;
  const double h0 = -(sun_radi_deg + atmo_refrac);
  cout << "calculated: " << endl;
  cout << "\t " << h0 << " degrees" << endl;
  
  double zenith = 90.0 - h0;

  cout << "zenith = " << zenith << " degrees" << endl;
  
  return zenith;
}

double hourAngle(double h0, double phi, double delta) {
  // Calculate the local hour angle
  
  // convert inputs to radians
  h0*=deg2rad;
  phi*=deg2rad;
  delta*=deg2rad;

  if (debug>1) {
    cout << "h0" << endl;
    cout << cos(h0)    << endl;
    cout << "phi" << endl;
    cout << sin(phi)   << endl;
    cout << cos(phi)   << endl;
    cout << "delta" << endl;
    cout << sin(delta) << endl;
    cout << cos(delta) << endl;
  }

  double cosH =   ( cos(h0) - sin(phi) * sin(delta) ) / ( cos(phi) * cos(delta) );
  double H = acos(cosH) * rad2deg;
  if (debug>0) {
    cout << "hour angle = " << H << " degrees" << endl;
    cout << "          or " << H/15.0 << " hours" << endl;
  }
  return H;
}

string hour2time (double fhr) {
  // convert fractional hour into hr:min:sec string
  int hr = floor(fhr);
  double fmin = (fhr - hr) * 60;
  int min = floor(fmin);
  double fsec = (fmin - min) * 60;
  int sec = floor(fsec);

  if (debug>1) {
    cout << "hour = " << fhr << endl;
    cout << "min = " << fmin << endl;
    cout << "sec = " << fsec << endl;
  }
  char time[64];
  sprintf(time,"%02d:%02d:%06.3f",hr,min,fsec);
  return string(time);
}

double getSolarNoon(double longitude, double timezone) {
  // apparent angular speed of the sun
  double const w=360./24.; // 15 degrees per hour
  
  // divide the geographic longitude by the angular speed to get the solar time zone
  double sol_tz=longitude/w;

  double tz_diff = timezone - sol_tz;

  // solar noon is... noon, plus the timezone difference
  double sol_noon = 12 + tz_diff;
  
  if (debug>0) {
    cout << "The specified timezone is " << timezone << " hours" << endl;
    cout << "    The solar timezone is " << sol_tz << " hours" << endl;
    cout << "          a difference of " << tz_diff << " hours" << endl;
    cout << "                       or " << tz_diff*60 << " minutes" << endl;
    cout << "Solar noon is " << sol_noon << " or " << hour2time(sol_noon) << endl;
  }
  return sol_noon;
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
  cout << "True longitude" << endl;
  cout << "\t           l = "; printDeg(l); cout << endl;
 
  // Sun's true anomaly
  double nu = (M + C);
  cout << "True anomaly" << endl;
  cout << "\t          nu = "; printDeg(nu); cout << endl;
 
  // longitude of the ascending node
  double Omega=longitudeAscendingNode(t);
  
  // apparent longitude L (lambda) of the Sun
  // true equinox of the date
  double lambda = l - 0.00569 - (0.00478 * sin(Omega));
  cout << " apparent longitude  = " << lambda << endl;
  lambda*=deg2rad;

  double epsilon=obliquityOfEcliptic(t);
 
  // correction for parallax (Meeus Eq. 25.8)
  // The true or instantaneous obliquity includes the nutation.
  double eCorrected = epsilon + 0.00256 * cos(Omega);
  cout << "              or " << eCorrected << " including nutation" << endl;
  // convert to radians
  eCorrected*=deg2rad;
 
  // Sun's right ascension a
  double alpha = atan2(cos(eCorrected) * sin(lambda),
		       cos(lambda));
  // convert to degrees
  alpha*=rad2deg;

  // Calculate the solar declination angle
  // declination d or delta
  double delta = asin(sin(eCorrected) * sin(lambda)) * rad2deg;
  double delta2 = asin(sin(epsilon * deg2rad) * sin(l * deg2rad)) * rad2deg;

  if (debug>0) {
    cout << "Solar coordinates:" << endl;
    cout << "\tright ascension = " << alpha << " degrees" << endl;
    cout << "\t    declination = " << delta << " degrees (NOAA)" << endl;
    cout << "\t             or = " << delta2 << " degrees" << endl;
  }

  // calculate equation of time
  // Equation 1 (no equation of time)

  // Equation 2  
  double EqT=equationOfTime2(M,alpha,0,epsilon);

  // Equation 3
  if (debug > 1) {
    double e2 = eCorrected / 2.0;
    cout << "e/2 rad = " << e2  << endl;
    double te2 = tan(e2);
    cout << "tan e/2 = " << te2 << endl;
    cout << "y = " << te2*te2 << endl;
  }
  double y = pow(tan(eCorrected / 2.0),2);
  cout << "y = " << y << endl;
  double e = eccentricity(t);
  double E=equationOfTime3(y,L,e,M);

  // adjust solar noon
  double solarNoon = getSolarNoon(longitude,timezone);
  solarNoon -= E;
  cout << "solar noon = " << solarNoon << " or " << hour2time(solarNoon) << endl;

  // calculate zenith
  double zenith=getZenith(e,nu);

  // calculate hour angle
  double H=hourAngle(zenith,latitude,delta);

  // Convert to local solar time
  double sunsetTime = solarNoon + H / 15.0;

  // Print the result
  cout << "Sunset time: " << hour2time(sunsetTime) << endl;
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

  return 0;
}
