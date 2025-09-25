/**
   Most of this code is developed from James Still's post on calculating solar
   coordinates: https://squarewidget.com/solar-coordinates/
   with references to
   https://aa.usno.navy.mil/faq/sun_approx
   Meeus, Jean (1991). Astronomical Algorithms.
*/

#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

// settings
const int debug = -1;
/* debug value
  -1 print sunset and commute only
   0 print major calculations
   1 print each calculation step, including alternate values
   2 print every variable value
*/

const bool do_NOAA = true;

// define constants
const double PI = atan(1) * 4;
const double deg2rad = PI / 180.;
const double rad2deg = 180. / PI;

double getJulianDate(int year, int month, int day) {
  cout << "The input date is " << year << "-" << month << "-" << day << endl;
  int a = (14 - month) / 12;
  int y = year + 4800 - a;
  int m = month + 12 * a - 3;
  int jd =
    day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;
  if (debug > 0) cout << "\tthe Julian date is " << jd << endl;
  return jd;  // are we supposed to return noon?
}

double getJ2000(double jd) {
  // convert Julian Date to J2000 epoch, that is, the Julian Date since Jan 1,
  // 2000
  double J2000 = jd - 2451545.0;
  if (debug > 0) cout << "\tthe J2000 date is " << J2000 << endl;
  return J2000;
}

double getJulianCentury(double J2000) {
  // convert J2000 date to Julian Ephemeris Century, that is, fraction of a
  // Julian century
  double T = J2000 / 36525.;
  if (debug > 0) cout << "\tthe Julian century is " << T << endl;
  return T;
}

void printDeg(double theta) {
  cout << theta;
  if (theta >= 360 || theta < 0) {
    cout << " or " << fmod(theta, 360);
  }
  cout << " degrees";
}

double meanLongitude(double t) {
  // Geometric Mean Longitude of the Sun
  // referred to the mean equinox of the time or sidreal time at Greenwich or
  // Universal Time
  // calculated in degrees
  // Meeus pg. 163, Eq. 25.2
  // based on J2000 longitude
  double L_1 = 280.460 + 36000.771 * t;                              // USNO
  double L_2 = 280.46646 + 36000.76983 * t + 0.0003032 * pow(t, 2);  // NOAA
  double L_5 = 280.4664567 + 36000.76982779 * t + 0.03032028 * pow(t, 2) +
    pow(t, 3) / 49931 - pow(t, 4) / 15300 - pow(t, 5) / 2e6;
  if (debug > 0) {
    cout << "Geometric Mean Longitude of the Sun" << endl;
    cout << "\t   linear: L = " << setprecision(6) << left;
    printDeg(L_1);
    cout << " (USNO)" << endl;
    cout << "\tquadratic: L = ";
    printDeg(L_2);
    cout << " (NOAA)" << endl;
    cout << "\t  quintic: L = ";
    printDeg(L_5);
    cout << endl;
  }
  if (do_NOAA)
    return fmod(L_2, 360);
  else
    return fmod(L_5, 360);
}

double meanAnomaly(double t) {
  // Mean Anomaly of the Sun
  // Meeus pg. 163, Eq. 25.3
  double M_1 = 357.528 + 35999.050 * t;
  double M_2 = 357.52911 + 35999.05029 * t - 0.0001537 * pow(t, 2);  // NOAA
  double M_3 = 357.52772 + 35999.050340 * t - 0.0001603 * pow(t, 2) +
    pow(t, 3) / 300000;  // X1 in Eq. 16, Reda & Andreas (2008)
  if (debug > 0) {
    cout << "Mean Anomaly of the Sun" << endl;
    cout << "\t   linear: M = " << setprecision(6);
    printDeg(M_1);
    cout << " (USNO)" << endl;
    cout << "\tquadratic: M = ";
    printDeg(M_2);
    cout << " (NOAA)" << endl;
    cout << "\t    cubic: M = ";
    printDeg(M_3);
    cout << " " << endl;
  }
  if (do_NOAA)
    return fmod(M_2, 360.0);
  else
    return fmod(M_3, 360.0);
}

double equationOfCenter(double t, double M) {
  // Sun's equation of center
  // Geocentric apparent ecliptic longitude of the Sun (adjusted for
  // aberration):

  // convert inputs to radians
  M *= deg2rad;

  double C_0 = 1.915 * sin(M) + 0.020 * sin(2 * M);

  double C_2 = (1.914602 - (0.004817 * t) - (0.000014 * pow(t, 2))) * sin(M) +
    (0.019993 - (0.000101 * t)) * sin(2 * M) +
    (0.000289 * sin(3 * M));

  if (debug > 0) {
    cout << "Equation of center" << endl;
    cout << "\t constant: C = " << C_0 << " (USNO)" << endl;
    cout << "\tquadratic: C = " << C_2 << " (NOAA)" << endl;
  }
  return C_2;
}

double longitudeAscendingNode(double t) {
  // Longitude of the ascending node of the Moon's mean orbit on the ecliptic,
  // measured form the mean equinox of the date. Taken from Ibrahim Reda and
  // Afshin Andreas, Solar position algorithm for solar radiation applications,
  // NREL Technical Report NREL/TP-560-34302 (2008).

  // Used to correction for nutation and aberration.
  double Omega_1 = 125.04 - 1934.136 * t;
  double Omega_3 = 125.04452 - 1934.136261 * t + 0.0020708 * pow(t, 2) +
    pow(t, 3) / 450000;  // X4 in Eq. 19, Reda & Andreas (2008)

  if (debug > 0) {
    cout << "Longitude of the ascending node" << endl;
    cout << "\t  lin: Omega = " << Omega_1 << " degrees (NOAA)" << endl;
    cout << "\tcubic: Omega = " << Omega_3 << " degrees" << endl;
  }
  return Omega_3 * deg2rad;
}

double nutationInLongitude(double Omega, double JCE, double X1) {
  // nutation in longitude
  // Longitude of the periapsis or longitude of the pericenter
  // omega is in radians
  double DPsi = -0.00569 - (0.00478 * sin(Omega));
  if (debug > 0) {
    cout << "Nutation in longitude" << endl;
    cout << "\t        DPsi = " << DPsi << " degrees" << endl;
  }

  if (debug > 1) {
    cout << "\t        DPSi = " << DPsi * deg2rad << " radians" << endl;

    double X0 = 297.85036 + 44526.7111480 * JCE - 0.0019142 * pow(JCE, 2) +
      pow(JCE, 3) / 189474.;  // Eq. 15
    double X3 = 93.27191 + 483202.017538 * JCE - 0.0036825 * pow(JCE, 2) +
      pow(JCE, 3) / 327270.;  // Eq. 18
    double X4 = Omega;                  // Eq. 19

    // print values
    cout << "\tX0 = " << X0 << " degrees" << endl;
    cout << "\tX1 = " << X1 << " degrees" << endl;
    cout << "\tX3 = " << X3 << " degrees" << endl;
    cout << "\tX4 = " << X4 * rad2deg << " degrees" << endl;

    // convert to radians
    X0 *= deg2rad;
    X1 *= deg2rad;
    X3 *= deg2rad;

    // Eq. 20, Reda & Andreas (2008)
    double DPsi0 = (-171996 - 174.2 * JCE) * sin(X4);
    double DPsi1 = (-13187 - 1.6 * JCE) * sin(X0 * -2 + X3 * 2 + X4 * 2);
    double DPsi2 = (-2274 - 0.2 * JCE) * sin(X3 * 2 + X4 * 2);
    double DPsi3 = (-2062 + 0.2 * JCE) * sin(X4 * 2);
    double DPsi4 = (1426 - 3.4 * JCE) * sin(X1);

    cout << "\t      DPsi0 = " << DPsi0 << " 0.1 milli arcseconds" << endl;
    cout << "\t      DPsi1 = " << DPsi1 << " 0.1 milli arcseconds" << endl;
    cout << "\t      DPsi2 = " << DPsi2 << " 0.1 milli arcseconds" << endl;
    cout << "\t      DPsi3 = " << DPsi3 << " 0.1 milli arcseconds" << endl;
    cout << "\t      DPsi4 = " << DPsi4 << " 0.1 milli arcseconds" << endl;

    double SDPsi = DPsi0 + DPsi1 + DPsi2 + DPsi3 + DPsi4;
    cout << "\t      SDPsi = " << SDPsi << " 0.1 milli arcseconds" << endl;
    SDPsi /= 10000;
    cout << "\t      SDPsi = " << SDPsi << " arcseconds" << endl;
    SDPsi /= 3600;
    cout << "\t      SDPsi = " << SDPsi << " degrees" << endl;
    SDPsi *= deg2rad;
    cout << "\t      SDPsi = " << SDPsi << " radians" << endl;
  }

  return DPsi;
}

double eccentricity(double t) {
  // eccentricity of Earth's orbit
  // input is Julian century
  // Meeus pg. 163, Eq. 25.4
  double e = 0.016708634 - (0.000042037 * t) - (0.0000001267 * pow(t, 2));

  if (debug > 0) {
    cout << "eccentricity\n\t e = " << e << endl;
  }
  return e;
}

double radiusVector(double e, double nu) {
  // the distance between the centres of the Sun and the Earth, in au
  // U.S. Naval Observatory function for radius vector.
  // c.f. Meeus Eq. 25.5

  // convert inputs to radians
  nu *= deg2rad;

  double R = 1.00014 - e * cos(nu) - 0.00014 * cos(2 * nu);

  // initial value for the semi-major axis for J2000
  // Bretagnon, P.: 1982, Astron. Astrophys. 114, 278
  double a = 1.00000101778;  // in au

  double R_NOAA = (a * (1 - pow(e, 2))) / (1 + e * cos(nu));

  if (debug > 0) {
    cout << "Radius vector" << endl;
    cout << "\tR = " << R << " au (USNO)" << endl;
    cout << "\t or " << R_NOAA << " au (NOAA)" << endl;
  }
  return R_NOAA;
}

double getSunSize(double rad_vec_au = 1) {
  // angular semidiameter of the Sun, in degrees
  // Sun's apparent radius
  // input is radial vector in au

  // constants
  const double au = 149597870700;  // Astronomical unit in m

  // physical size of the sun
  // measured solar radius from Mercury transits
  // https://arxiv.org/abs/1203.4898
  // 696,342 ± 65km
  const double sun_radi_m = 696342e3;
  if (debug > 1) {
    const double sol_radi_m = 6.95700e8;  // solar radius in m
    cout << "Radius of the sun"
         << "\n\t " << sun_radi_m / sol_radi_m << " R_sol" << endl;
  }
  const double sun_diam_m = sun_radi_m * 2;  // diameter of the sun in m

  // distance to the sun
  // convert radial vector to m
  const double rad_vec_m = rad_vec_au * au;

  // apparent size of the sun
  const double sun_diam_rad =
    2 * atan(sun_diam_m / (2 * rad_vec_m));  // angular size in radians
  const double sun_diam_deg =
    sun_diam_rad * rad2deg;  // angular size in degrees

  // return semidiameter
  const double sun_radi_deg = sun_diam_deg / 2;

  if (debug > 0) {
    cout << "Angular size of the sun" << endl;
    cout << "\t" << sun_diam_deg << " degrees";
    cout << " or " << sun_diam_deg * 60 << " arcminutes"
         << endl;  // should be between ~31.5 32.5 arcminutes
    cout << "\t" << sun_radi_deg << " degrees (semidiameter)" << endl;
  }

  return sun_radi_deg;
}

double dms2deg(double deg, double min, double sec) {
  double ang = deg + min / 60. + sec / 3600.;
  if (debug > 0) {
    if (deg != 0) cout << deg << "\u00b0";
    if ((deg != 0) && (min != 0)) cout << min << "'";
    cout << sec << "'' = " << setprecision(10) << ang << " degrees" << endl;
  }
  return ang;
}

// A function to calculate the obliquity of the ecliptic in degrees
// Earth's axial tilt
double obliquityOfEcliptic(double T) {
  // obliquity of the ecliptic (Meesus Eq. 22.2)
  // All of these expressions are for the mean obliquity, that is, without the
  // nutation of the equator included. input is Julian Ephemeris Century output
  // in degrees

  // define reference angles
  // the initial values adopted by the JAU (Grenoble, 1976):
  if (debug > 1) {
    cout << "Initial obliquity of the ecliptic, J2000" << endl;
    cout << "\t";
  }
  double const epsilon0 = dms2deg(23, 26, 21.448);

  // convert Julian centuries to 10,000 Julian years
  double const U = T / 100.0;

  // Calculate the obliquity of the ecliptic in arcseconds
  double const t0 = epsilon0 * 3600.;
  double const t = 4680.93;  // the O(1) term is included here for reference to
                             // earlier versions

  if (debug > 1) {
    cout << "\t      epsilon0 = " << epsilon0 << " degrees" << endl;
    cout << "\t              or " << t0 << " arcseconds" << endl;
    cout << "\t     ";
    double const theta2 = dms2deg(0, 0, t);
    cout << "\t        theta2 = " << theta2 << endl;
    cout << "\t              or " << t << endl;
  }

  // Linear fit:
  // Used by USNO
  double epsilon_1 = t0 - t * U;

  // Cubic fit:
  // Used by NOAA
  // L denotes the solution of Lieske et al. (1977).
  // From 1984, the Jet Propulsion Laboratory's DE series of computer-generated
  // ephemerides took over as the fundamental ephemeris of the Astronomical
  // Almanac.
  double epsilon_L = t0 - 4681.5 * U - 5.9 * pow(U, 2) + 1813 * pow(U, 3);

  // Tenth-degree polynomial fit:
  // Laskar, J., Astronomy and Astrophysics, 157: 68 (1986).
  // Table 8. Formulas for the precession. The obliquity is given in arcseconds
  // and the time t is measured in units of 10,000 Julian years from J2000 (JD
  // 2451545.0). NGT denotes our solution (Numerical General Theory).

  double epsilon_NGT = epsilon_1 - 1.55 * pow(U, 2) + 1999.25 * pow(U, 3) -
    51.38 * pow(U, 4) - 249.67 * pow(U, 5) -
    39.05 * pow(U, 6) + 7.12 * pow(U, 7) +
    27.87 * pow(U, 8) + 5.79 * pow(U, 9) + 2.45 * pow(U, 10);

  // Convert arcseconds to degrees
  epsilon_1 *= 1 / 3600.;
  epsilon_L *= 1 / 3600.;
  epsilon_NGT *= 1 / 3600.;

  if (debug > 0) {
    cout << "Obliquity of the ecliptic" << endl;
    cout << "\t linear: ";
    printDeg(epsilon_1);
    cout << " (USNO)" << endl;
    cout << "\t  cubic: ";
    printDeg(epsilon_L);
    cout << " Lieske et al. 1977 (NOAA)" << endl;
    cout << "\t 10poly: " << epsilon_NGT << " Laskar 1985" << endl;
  }
  if (do_NOAA)
    return epsilon_L;
  else
    return epsilon_NGT;
}

// The equation of time represents the difference between solar time and clock
// time due to the Earth's elliptical orbit and axial tilt.

double equationOfTime2(double M, double RA, double DPsi, double epsilon,
                       double L, double eccentricity) {
  double EqT = RA - (M + L);  // degrees
  if (debug > 0) {
    cout << "   calculated with M and R.A." << endl;
    cout << "\tE = ";
    printDeg(EqT);
    cout << " (USNO)" << endl;
  }
  if (debug > 1) cout << "\tE = " << fmod(EqT, 360) * 4 << " minutes" << endl;

  double E = EqT - 0.0057183 + DPsi * cos(epsilon);
  cout << "   corrected with DPsi and epsilon" << endl;
  cout << "\tE = ";
  printDeg(E);
  cout << " (corrected)" << endl;

  const double minutesPerDegree =
    4.0;  // Conversion factor from degrees to minutes

  double E2 = minutesPerDegree * 4 *
    (eccentricity * sin(M) - 2 * eccentricity * sin(2 * M) +
     4 * eccentricity * sin(3 * M) - 0.5 * eccentricity * sin(4 * M) -
     1.25 * pow(sin(RA - epsilon), 2)) *
    rad2deg;  // Result in minutes
  if (debug > -1) {
    cout << "\tE = ";
    printDeg(E2);
    cout << endl;
  }

  return EqT;
}

double equationOfTime3(double epsilon, double L, double e, double M) {
  // We use the approximation of the equation of time given by W.M. Smart,
  // Text-Book on Spherical Astronomy, Cambridge University Press, 1956, p. 149:

  if (debug > 1) {
    double e2 = epsilon / 2.0;
    cout << "\te/2 rad = " << e2 << endl;
    double te2 = tan(e2);
    cout << "\ttan e/2 = " << te2 << endl;
    cout << "\ty = " << te2 * te2 << endl;
  }
  double y = pow(tan(epsilon / 2.0), 2);
  if (debug > 1) cout << "\t     y =  " << y << endl;

  // convert inputs to radians
  L *= deg2rad;
  M *= deg2rad;

  if (debug > 1) {
    // print individual terms
    cout << "\t" << y * sin(2 * L) << endl;
    cout << "\t" << -2 * e * sin(M) << endl;
    cout << "\t" << +4 * e * y * sin(M) * cos(2 * L) << endl;
    cout << "\t" << -(1 / 2.) * pow(y, 2) * sin(4 * L) << endl;
    cout << "\t" << -(5 / 4.) * pow(e, 2) * sin(2 * M) << endl;
  }

  double E = y * sin(2 * L) - 2 * e * sin(M) + 4 * e * y * sin(M) * cos(2 * L) -
    (1 / 2.) * pow(y, 2) * sin(4 * L) -
    (5 / 4.) * pow(e, 2) * sin(2 * M);

  if (debug > 0) {
    cout << "   calculated using Smart (1956)" << endl;
    cout << "\tE = " << E << " radians" << endl;
    cout << "\tE = " << E * rad2deg << " degrees" << endl;
    cout << "\tE = " << E * rad2deg * 4 << " minutes" << endl;
  }
  if (debug > -1)
    cout << "\tE = " << E * rad2deg / 15 << " hours" << endl;
  return E * rad2deg / 15;  // hours
}

double getZenith(double e, double nu) {
  double R = radiusVector(e, nu);
  if (debug > 0) {
    // get the apparent size of the sun
    cout << "Apparent size of the Sun" << endl;
    // the default size is based on a solar distance of 1 au
    cout << "   default: ";
  }
  double r_def = getSunSize();
  if (debug > 0) cout << "   calculated: ";
  double r_cal = getSunSize(R);

  // select value to use
  double sun_radi_deg;
  if (do_NOAA)
    sun_radi_deg = r_def;
  else
    sun_radi_deg = r_cal;

  // The elevation of the sun is the sum of the angular radius of the sun and
  // the angular refraction of the atmosphere.
  const double atmo_refrac = 0.5667;
  const double h0_cal = -(sun_radi_deg + atmo_refrac);
  // The default value is based on an apparent angular radius of the sun of
  // 0.26667 degrees (based on a solar distance of 1 au), and an atmospheric
  // refraction of 0.5667 degrees. The result is rounded to three decimal
  // places.
  const double h0_def = -0.833;
  if (debug > 0) {
    cout << "Elevation of the Sun" << endl;
    cout << "   default: " << endl;
    cout << "\th0 = " << h0_def << " degrees (NOAA)" << endl;
    cout << "\th0 = " << -(r_def + atmo_refrac) << " degrees" << endl;
    cout << "   calculated: " << endl;
    cout << "\th0 = " << h0_cal << " degrees" << endl;
  }

  // select value to use
  double h0;
  if (do_NOAA)
    h0 = h0_def;
  else
    h0 = h0_cal;

  if (debug > 0) {
    cout << "\th0 = " << h0 / 15 << " hours" << endl;
    cout << "\th0 = " << h0 * 4 << " minutes" << endl;
  }

  // the zenith is adjusted by the elevation of the sun
  double zenith = 90.0 - h0;

  if (debug > -1) {
    cout << "Zenith" << endl;
    cout << "\tz = " << zenith << " degrees" << endl;
  }
  if (debug > 0) {
    cout << "\tz = " << zenith / 15 << " hours" << endl;
    cout << "\tz = " << zenith * 4 << " minutes" << endl;
  }

  return zenith;
}

double hourAngle(double h0, double phi, double delta) {
  // Calculate the local hour angle

  // convert inputs to radians
  h0 *= deg2rad;
  phi *= deg2rad;
  delta *= deg2rad;

  if (debug > 1) {
    cout << "   h0" << endl;
    cout << "\t" << cos(h0) << endl;
    cout << "   phi" << endl;
    cout << "\t" << sin(phi) << endl;
    cout << "\t" << cos(phi) << endl;
    cout << "   delta" << endl;
    cout << "\t" << sin(delta) << endl;
    cout << "\t" << cos(delta) << endl;
  }

  double cosH = (cos(h0) - sin(phi) * sin(delta)) / (cos(phi) * cos(delta));
  double H = acos(cosH) * rad2deg;
  if (debug > 0) {
    cout << "Hour angle\n\tHA = " << H << " degrees" << endl;
    cout << "          or " << H / 15.0 << " hours" << endl;
  }
  return H;
}

string hour2time(double fhr, bool do_fractional_second = true) {
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
              << std::setfill('0') << std::setw(2) << time.min << ":"
              << std::fixed << std::setprecision(2) << std::setfill('0')
              << std::setw(5) << time.fsec;

  return time_string.str();
}

double getSolarNoon(double longitude, double set_timezone) {
  // apparent angular speed of the sun
  double const w = 360. / 24.;  // 15 degrees per hour

  // divide the geographic longitude by the angular speed to get the solar time
  // zone
  double sol_tz = longitude / w;

  double tz_diff = set_timezone - sol_tz;

  // solar noon is... noon, plus the timezone difference
  double sol_noon = 12 + tz_diff;

  if (debug > 0) {
    cout << "The specified timezone is " << set_timezone << " hours" << endl;
    cout << "    The solar timezone is " << sol_tz << " hours" << endl;
    cout << "          a difference of " << tz_diff << " hours" << endl;
    cout << "                       or " << tz_diff * 60 << " minutes" << endl;
    cout << "Solar noon\n\t" << sol_noon << " or " << hour2time(sol_noon)
         << endl;
  }
  return sol_noon;
}

double getSunset(int year, int month, int day, double latitude,
                 double longitude, int set_timezone) {
  // date
  double jd = getJulianDate(year, month, day);
  double j2000 = getJ2000(jd);
  double t = getJulianCentury(j2000);

  /* Calculate quantities based on time only */
  double L = meanLongitude(t);
  double M = meanAnomaly(t);
  double epsilon0 = obliquityOfEcliptic(t);

  /* First correction
   *
   *  Correct the longitude and anomaly of the Sun using the Equation of Center
   *
   */

  // Equation of center
  double C = equationOfCenter(t, M);

  // Sun's true geometric longitude
  double l = (L + C);
  if (debug > 0) {
    cout << "True longitude of the Sun" << endl;
    cout << "\t           l = " << setprecision(7);
    printDeg(l);
    cout << endl;
  }

  // Sun's true anomaly
  double nu = (M + C);
  if (debug > -1) {
    cout << "True anomaly of the Sun" << endl;
    cout << "\t          nu = " << setprecision(5);
    printDeg(nu);
    cout << endl;
  }

  /* Second correction
   *
   *  Correct the longitude and obliquity for nutation
   *
   */

  // Longitude of the ascending node
  double Omega = longitudeAscendingNode(t);

  // nutation in longitude
  double DPsi = nutationInLongitude(Omega, t, L);

  // apparent longitude of the Sun
  // true equinox of the date
  double lambda = l + DPsi;
  if (debug > -1) {
    cout << "Apparent longitude of the Sun\n\t     lambda  = ";
    printDeg(lambda);
    cout << endl;
  }

  // nutation in obliquity
  // correction for parallax (Meeus Eq. 25.8)
  // The true or instantaneous obliquity includes the nutation.
  double Depsilon = 0.00256 * cos(Omega);
  double epsilon = epsilon0 + Depsilon;
  if (debug > 1)
    cout << "Nutation in obliquity\n\t      Depsilon =  " << Depsilon << endl;
  if (debug > -1) {
    cout << "Instantaneous obliquity of the Sun\n\t     epsilon = " << epsilon
         << " including nutation" << endl;
  }

  // convert to radians
  lambda *= deg2rad;
  epsilon *= deg2rad;

  /*
   * Using the corrected quantities
   *    lambda
   *    nu
   *    epsilon
   * calculate the solar coordinates, R.A. and Dec.
   */

  // Sun's right ascension a
  double alpha = atan2(cos(epsilon) * sin(lambda), cos(lambda));

  // Calculate the solar declination angle
  // declination d or delta
  double delta = asin(sin(epsilon) * sin(lambda));

  if (debug > 1) {
    // Longitude of the periapsis or longitude of the pericenter
    double tanbar = atan2(
                          cos(alpha), sin(alpha) * cos(epsilon) + tan(delta) * sin(epsilon));
    if (cos(alpha) < 0) tanbar += PI;

    cout << "Longitude of the periapsis" << endl;
    cout << "\ttanbar = " << tanbar << " radians" << endl;
    cout << "\ttanbar = " << tanbar * rad2deg << " degrees" << endl;

    cout << "\t      cos(R.A.) = " << cos(alpha) << endl;
  }
  // convert to degrees
  alpha *= rad2deg;
  delta *= rad2deg;

  if (debug > 0) {
    cout << "Solar coordinates:" << endl;
    cout << "\tright ascension = " << alpha << " degrees" << endl;
    cout << "\t    declination = " << delta << " degrees (NOAA)" << endl;
  }

  /*
   * It all leads to this: calculate the Equation of Time
   */

  double e = eccentricity(t);
  if (debug > -1)
    cout << "Equation of Time" << endl;

  if (debug > 0)
    // Equation 2
    equationOfTime2(M, alpha, DPsi, epsilon, L, e);

  // Equation 3
  double E = equationOfTime3(epsilon, L, e, M);

  // adjust solar noon
  double solarNoon = getSolarNoon(longitude, set_timezone);
  solarNoon -= E;
  if (debug > -1)
    cout << "Corrected solar noon\n\t" << solarNoon << " or "
         << hour2time(solarNoon) << endl;

  // calculate zenith
  double zenith = getZenith(e, nu);

  // calculate hour angle
  double HA_deg = hourAngle(zenith, latitude, delta);

  double HA = HA_deg / 15.;

  // Convert to local solar time
  double sunsetTime = solarNoon + HA;
  double sunriseTime = solarNoon - HA;

  // Print the result
  if (debug > -1)
    cout << "Sunrise time: " << hour2time(sunriseTime) << endl;
  cout << " Sunset time: " << hour2time(sunsetTime) << endl;

  return sunsetTime;
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

  // Set the location and timezone
  double latitude = 30.4275784357249;  // New Orleans
  double longitude = -90.0914955109431;
  int set_timezone = -6;

  // Calculate the sunset time
  double sunsetTime =
    getSunset(year, month, day, latitude, longitude, set_timezone);

  // Print the current date and time
  if (debug > -1)
    cout << "\n"<< endl;
  cout << "Current time: " << put_time(&ltm, "%H:%M:%S") << endl;

  // Calculate the difference between the current time and the sunset time
  double currentTime = ltm.tm_hour + ltm.tm_min / 60.0 + ltm.tm_sec / 3600.0;
  double timeDifference = sunsetTime - currentTime;

  // Convert the time difference to hours, minutes, and seconds
  int diffHours = static_cast<int>(timeDifference);
  int diffMinutes = static_cast<int>((timeDifference - diffHours) * 60);
  int diffSeconds =
    static_cast<int>(((timeDifference - diffHours) * 60 - diffMinutes) * 60);

  // Print the time difference
  cout << "Time to sunset: " << std::setw(2) << std::right << diffHours << ":"
       << std::setfill('0') << std::setw(2) << diffMinutes << ":"
       << std::setfill('0') << std::setw(2) << diffSeconds << endl;

  // Calculate leave time (subtract commute time of 37 minutes)
  const double commuteMinutes = 37.0;
  double leaveTime = sunsetTime - (commuteMinutes / 60.0);
  double timeToLeave = leaveTime - currentTime;

  // Convert leave time to readable format
  string leaveTimeStr = hour2time(leaveTime, false);

  // Convert time to leave to hours, minutes, and seconds
  int leaveHours = static_cast<int>(timeToLeave);
  int leaveMins = static_cast<int>((timeToLeave - leaveHours) * 60);
  int leaveSecs = static_cast<int>(((timeToLeave - leaveHours) * 60 - leaveMins) * 60);

  // Get sunset time as string for summary
  string sunsetTimeStr = hour2time(sunsetTime, false);

  // Print the clear summary
  cout << endl;
  if (timeToLeave > 0) {
    cout << "*** LEAVE BY " << leaveTimeStr << " (in "
         << std::setfill(' ') << std::setw(2) << std::right << leaveHours << ":"
         << std::setfill('0') << std::setw(2) << leaveMins << ":"
         << std::setfill('0') << std::setw(2) << leaveSecs
         << ") TO GET HOME BY " << sunsetTimeStr << " (SUNSET) ***" << endl;
  } else {
    cout << "*** YOU SHOULD HAVE LEFT " << std::abs(leaveHours) << ":"
         << std::setfill('0') << std::setw(2) << std::abs(leaveMins) << ":"
         << std::setfill('0') << std::setw(2) << std::abs(leaveSecs)
         << " AGO TO GET HOME BY " << sunsetTimeStr << " (SUNSET) ***" << endl;
  }

  return 0;
}
