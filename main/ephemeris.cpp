/**
 * Educational Ephemeris Calculator
 *
 * This program demonstrates multiple astronomical calculation methods
 * (NOAA, USNO, Laskar) for computing solar coordinates and events.
 *
 * It shows every step of the calculation with full debug output,
 * allowing comparison between different algorithms and data sources.
 *
 * Authoritative results are provided by the sunset_calc library.
 * This program adds pedagogical context showing calculation steps.
 *
 * References:
 * - James Still: https://squarewidget.com/solar-coordinates/
 * - USNO: https://aa.usno.navy.mil/faq/sun_approx
 * - NOAA: https://www.esrl.noaa.gov/gmd/grad/solcalc/
 * - Reda, I., & Andreas, A. (2008). Solar position algorithm for solar radiati
 * - Meeus, Jean (1991). Astronomical Algorithms.
 */

#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "config_ephemeris.h"
#include "config_location.h"
#include "constants.h"
#include "solar_utils.h"
#include "sunset_calc.h"

using namespace std;
using namespace astro;
using namespace config;

// settings
const int kDebugLevel = config::debug::kDefaultDebugLevel;
/* debug value
  -1 print sunset and commute only
   0 print major calculations
   1 print each calculation step, including alternate values
   2 print every variable value
*/

const bool do_NOAA = config::algorithm::kUseNoaa;

double getJulianDate(int year, int month, int day) {
  if (kDebugLevel >= -1)
    cout << "  Input date:   " << year << "-" << month << "-" << day << endl;

  // Call library version for calculation
  sunset_calc::SunsetCalculator calc;
  double jd = calc.getJulianDate(year, month, day);

  if (kDebugLevel > 0) cout << "\tthe Julian date is " << (long long)jd << endl;
  return jd;  // are we supposed to return noon?
}

double getJ2000(double jd) {
  // Call library version for calculation
  sunset_calc::SunsetCalculator calc;
  double J2000 = calc.getJ2000(jd);

  if (kDebugLevel > 0) cout << "\tthe J2000 date is " << J2000 << endl;
  return J2000;
}

double getJulianCentury(double J2000) {
  // Call library version for calculation
  sunset_calc::SunsetCalculator calc;
  double T = calc.getJulianCentury(J2000);

  if (kDebugLevel > 0) cout << "\tthe Julian century is " << T << endl;
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

  sunset_calc::SunsetCalculator calc;
  double L_1 = calc.meanLongitude(t, sunset_calc::Algorithm::USNO);
  double L_2 = calc.meanLongitude(t, sunset_calc::Algorithm::NOAA);
  double L_5 = calc.meanLongitude(t, sunset_calc::Algorithm::LASKAR);

  if (kDebugLevel > 0) {
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
    return L_2;
  else
    return L_5;
}

double meanAnomaly(double t) {
  // Mean Anomaly of the Sun, i.e., Mean Solar Anomaly
  //
  // WHAT IS MEAN SOLAR ANOMALY?
  // The Mean Solar Anomaly is the angle between the Sun's position in its
  // elliptical orbit and its position at perihelion (closest point to Earth),
  // measured from Earth's center, assuming the Sun moves at a constant angular
  // velocity.
  //
  // In simpler terms: It's a measure of "where the Sun is" in its yearly
  // journey around the sky, expressed as an angle in degrees (0° to 360°).
  //
  // - At 0°: Sun is at perihelion (early January, closest to Earth)
  // - At 180°: Sun is at aphelion (early July, farthest from Earth)
  // - The Sun's actual orbital speed varies due to Earth's elliptical orbit
  // - Mean anomaly assumes constant speed, making calculations simpler
  //
  // INPUT: the variable 't' is the number of Julian centuries since J2000.0
  // epoch (January 1, 2000, 12:00 TT)
  //
  // OUTPUT: angle in degrees between perihelion and current position
  //
  // References:
  //   Meeus pg. 163, Eq. 25.3

  sunset_calc::SunsetCalculator calc;
  double M_1 = calc.meanAnomaly(t, sunset_calc::Algorithm::USNO);
  double M_2 = calc.meanAnomaly(t, sunset_calc::Algorithm::NOAA);
  double M_3 = calc.meanAnomaly(t, sunset_calc::Algorithm::LASKAR);

  if (kDebugLevel > 0) {
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

  // Constants from NOAA solar calculator and USNO algorithms
  // See: https://www.esrl.noaa.gov/gmd/grad/solcalc/

  sunset_calc::SunsetCalculator calc;
  double C_0 = calc.equationOfCenter(t, M, sunset_calc::Algorithm::USNO);
  double C_2 = calc.equationOfCenter(t, M, sunset_calc::Algorithm::NOAA);

  if (kDebugLevel > 0) {
    cout << "Equation of center" << endl;
    cout << "\t constant: C = " << C_0 << " (USNO)" << endl;
    cout << "\tquadratic: C = " << C_2 << " (NOAA)" << endl;
  }
  return C_2;
}

double longitudeAscendingNode(double t) {
  // Longitude of the ascending node of the Moon's mean orbit on the ecliptic,
  // measured from the mean equinox of the date.
  //
  // References:
  // - NOAA: https://www.esrl.noaa.gov/gmd/grad/solcalc/
  // - Reda, I., & Andreas, A. (2008). Solar position algorithm for solar
  // radiation
  //   applications. NREL Technical Report NREL/TP-560-34302. National Renewable
  //   Energy Laboratory, Solar Radiation Research Laboratory.

  // Show both formulations for pedagogical comparison
  sunset_calc::SunsetCalculator calc;

  double omega_noaa = calc.longitudeAscendingNode(
      t, sunset_calc::LongitudeAscendingNodeFormulation::NOAA_LINEAR);
  double omega_reda = calc.longitudeAscendingNode(
      t, sunset_calc::LongitudeAscendingNodeFormulation::REDA_ANDREAS_SPA);

  if (kDebugLevel > 0) {
    cout << "Longitude of the ascending node" << endl;
    cout << "\t  NOAA (linear):     " << omega_noaa * kRad2Deg << " degrees"
         << endl;
    cout << "\tReda & Andreas SPA:  " << omega_reda * kRad2Deg << " degrees"
         << endl;
  }

  // Use higher precision by default (Reda & Andreas)
  return omega_reda;
}

double nutationInLongitude(double Omega, double JCE, double X1) {
  // nutation in longitude
  // Longitude of the periapsis or longitude of the pericenter
  // omega is in radians
  double DPsi = -0.00569 - (0.00478 * sin(Omega));
  if (kDebugLevel > 0) {
    cout << "Nutation in longitude" << endl;
    cout << "\t        DPsi = " << DPsi << " degrees" << endl;
  }

  if (kDebugLevel > 1) {
    cout << "\t        DPSi = " << DPsi * kDeg2Rad << " radians" << endl;

    double X0 = 297.85036 + 44526.7111480 * JCE - 0.0019142 * pow(JCE, 2) +
                pow(JCE, 3) / 189474.;  // Eq. 15
    double X3 = 93.27191 + 483202.017538 * JCE - 0.0036825 * pow(JCE, 2) +
                pow(JCE, 3) / 327270.;  // Eq. 18
    double X4 = Omega;                  // Eq. 19

    // print values
    cout << "\tX0 = " << X0 << " degrees" << endl;
    cout << "\tX1 = " << X1 << " degrees" << endl;
    cout << "\tX3 = " << X3 << " degrees" << endl;
    cout << "\tX4 = " << X4 * kRad2Deg << " degrees" << endl;

    // convert to radians
    X0 *= kDeg2Rad;
    X1 *= kDeg2Rad;
    X3 *= kDeg2Rad;

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
    SDPsi *= kDeg2Rad;
    cout << "\t      SDPsi = " << SDPsi << " radians" << endl;
  }

  return DPsi;
}

double eccentricity(double t) {
  // Call library version for calculation
  sunset_calc::SunsetCalculator calc;
  double e = calc.eccentricity(t);

  if (kDebugLevel > 0) {
    cout << "eccentricity\n\t e = " << e << endl;
  }
  return e;
}

double radiusVector(double e, double nu) {
  // the distance between the centres of the Sun and the Earth, in au
  // U.S. Naval Observatory function for radius vector.
  // c.f. Meeus Eq. 25.5

  // convert inputs to radians
  nu *= kDeg2Rad;

  double R = 1.00014 - e * cos(nu) - 0.00014 * cos(2 * nu);

  // initial value for the semi-major axis for J2000
  // Bretagnon, P.: 1982, Astron. Astrophys. 114, 278
  double a = 1.00000101778;  // in au

  double R_NOAA = (a * (1 - pow(e, 2))) / (1 + e * cos(nu));

  if (kDebugLevel > 0) {
    cout << "Radius vector" << endl;
    cout << "\tR = " << R << " au (USNO)" << endl;
    cout << "\t or " << R_NOAA << " au (NOAA)" << endl;
  }
  return R_NOAA;
}

double getSunSize_Debug(double rad_vec_au = 1) {
  // angular semidiameter of the Sun, in degrees
  // Sun's apparent radius
  // input is radial vector in au

  // constants
  const double au = kAuMeters;  // Astronomical unit in m

  // physical size of the sun
  // measured solar radius from Mercury transits
  // https://arxiv.org/abs/1203.4898
  // 696,342 ± 65km
  const double sun_radi_m = kSolarRadiusMeters;
  if (kDebugLevel > 1) {
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
      sun_diam_rad * kRad2Deg;  // angular size in degrees

  // return semidiameter
  const double sun_radi_deg = sun_diam_deg / 2;

  if (kDebugLevel > 0) {
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
  if (kDebugLevel > 0) {
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

  sunset_calc::SunsetCalculator calc;
  double epsilon_1 = calc.obliquityOfEcliptic(T, sunset_calc::Algorithm::USNO);
  double epsilon_L = calc.obliquityOfEcliptic(T, sunset_calc::Algorithm::NOAA);
  double epsilon_NGT =
      calc.obliquityOfEcliptic(T, sunset_calc::Algorithm::LASKAR);

  if (kDebugLevel > 0) {
    cout << "Obliquity of the ecliptic" << endl;
    cout << "\t linear: ";
    printDeg(epsilon_1);
    cout << " (USNO)" << endl;
    cout << "\t  cubic: ";
    printDeg(epsilon_L);
    cout << " Lieske et al. 1977 (NOAA)" << endl;
    cout << "\t 10poly: ";
    printDeg(epsilon_NGT);
    cout << " Laskar 1986" << endl;
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
  if (kDebugLevel > 0) {
    cout << "   calculated with M and R.A." << endl;
    cout << "\tE = ";
    printDeg(EqT);
    cout << " (USNO)" << endl;
  }
  if (kDebugLevel > 1)
    cout << "\tE = " << fmod(EqT, 360) * 4 << " minutes" << endl;

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
              kRad2Deg;  // Result in minutes
  if (kDebugLevel > -1) {
    cout << "\tE = ";
    printDeg(E2);
    cout << endl;
  }

  return EqT;
}

double equationOfTime3(double epsilon, double L, double e, double M) {
  // We use the approximation of the equation of time given by W.M. Smart,
  // Text-Book on Spherical Astronomy, Cambridge University Press, 1956, p. 149:

  if (kDebugLevel > 1) {
    double e2 = epsilon / 2.0;
    cout << "\te/2 rad = " << e2 << endl;
    double te2 = tan(e2);
    cout << "\ttan e/2 = " << te2 << endl;
    cout << "\ty = " << te2 * te2 << endl;
  }
  double y = pow(tan(epsilon / 2.0), 2);
  if (kDebugLevel > 1) cout << "\t     y =  " << y << endl;

  // convert inputs to radians
  L *= kDeg2Rad;
  M *= kDeg2Rad;

  if (kDebugLevel > 1) {
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

  if (kDebugLevel > 0) {
    cout << "   calculated using Smart (1956)" << endl;
    cout << "\tE = " << E << " radians" << endl;
    cout << "\tE = " << E * kRad2Deg << " degrees" << endl;
  }
  if (kDebugLevel > 1) {
    cout << "\tE = " << E * kRad2Deg * 4 << " minutes" << endl;
  }
  if (kDebugLevel > -1)
    cout << "\tE = " << E * kRad2Deg / 15 << " hours" << endl;
  return E * kRad2Deg / 15;  // hours
}

double getZenith_Debug(
    double e, double nu,
    double altitude_meters = config::location::kDefaultObserverAltitude) {
  double R = radiusVector(e, nu);
  if (kDebugLevel > 0) {
    // get the apparent size of the sun
    cout << "Apparent size of the Sun" << endl;
    // the default size is based on a solar distance of 1 au
    cout << "   default: ";
  }
  double r_def = getSunSize_Debug();
  if (kDebugLevel > 0) cout << "   calculated: ";
  double r_cal = getSunSize_Debug(R);

  // select value to use
  double sun_radi_deg;
  if (do_NOAA)
    sun_radi_deg = r_def;
  else
    sun_radi_deg = r_cal;

  // The elevation of the sun is the sum of the angular radius of the sun and
  // the angular refraction of the atmosphere.
  const double atmo_refrac = kAtmosphericRefraction;
  const double h0_cal = -(sun_radi_deg + atmo_refrac);

  // Apply altitude correction
  double h0_alt = h0_cal;
  if (altitude_meters > 0) {
    double altitudeCorrection = -2.076 * sqrt(altitude_meters) / 60.0;
    h0_alt += altitudeCorrection;
    if (kDebugLevel > 1) {
      cout << "Altitude correction: " << altitudeCorrection << " degrees"
           << endl;
      cout << "Adjusted elevation: " << h0_alt << " degrees" << endl;
    }
  }

  // The default value is based on an apparent angular radius of the sun of
  // 0.26667 degrees (based on a solar distance of 1 au), and an atmospheric
  // refraction of 0.5667 degrees. The result is rounded to three decimal
  // places.
  const double h0_def = kStandardSunsetElevation;
  if (kDebugLevel > 0) {
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

  if (kDebugLevel > 1) {
    cout << "\th0 = " << h0 / 15 << " hours" << endl;
    cout << "\th0 = " << h0 * 4 << " minutes" << endl;
  }

  // the zenith is adjusted by the elevation of the sun
  double zenith = 90.0 - h0;

  if (kDebugLevel > -1) {
    cout << "Zenith" << endl;
    cout << "\tz = " << zenith << " degrees" << endl;
  }
  if (kDebugLevel > 1) {
    cout << "\tz = " << zenith / 15 << " hours" << endl;
    cout << "\tz = " << zenith * 4 << " minutes" << endl;
  }

  return zenith;
}

double hourAngle(double h0, double phi, double delta) {
  // Calculate the local hour angle

  if (kDebugLevel > 1) {
    // convert inputs to radians for debug display
    double h0_rad = h0 * kDeg2Rad;
    double phi_rad = phi * kDeg2Rad;
    double delta_rad = delta * kDeg2Rad;

    cout << "   h0" << endl;
    cout << "\t" << cos(h0_rad) << endl;
    cout << "   phi" << endl;
    cout << "\t" << sin(phi_rad) << endl;
    cout << "\t" << cos(phi_rad) << endl;
    cout << "   delta" << endl;
    cout << "\t" << sin(delta_rad) << endl;
    cout << "\t" << cos(delta_rad) << endl;
  }

  // Call library version for calculation
  sunset_calc::SunsetCalculator calc;
  double H = calc.hourAngle(h0, phi, delta);

  if (kDebugLevel > -1) {
    cout << "Hour angle\n\tHA = " << H << " degrees" << endl;
  }
  if (kDebugLevel > 1) {
    cout << "          or " << H / 15.0 << " hours" << endl;
  }
  return H;
}

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

double getSolarNoon(double longitude, double set_timezone) {
  // apparent angular speed of the sun
  double const w = kDegreesPerHour;  // 15 degrees per hour

  // divide the geographic longitude by the angular speed to get the solar time
  // zone
  double sol_tz = longitude / w;

  double tz_diff = set_timezone - sol_tz;

  // solar noon is... noon, plus the timezone difference
  double sol_noon = 12 + tz_diff;

  if (kDebugLevel > 0) {
    cout << "The specified timezone is " << set_timezone << " hours" << endl;
    cout << "    The solar timezone is " << sol_tz << " hours" << endl;
    cout << "          a difference of " << tz_diff << " hours" << endl;
    cout << "Solar noon\n\t" << sol_noon << endl;
  }
  if (kDebugLevel > 1) {
    cout << "                       or " << tz_diff * 60 << " minutes" << endl;
    cout << "Solar noon display: " << hour2time(sol_noon) << endl;
  }
  return sol_noon;
}

double getSunset_Debug(
    int year, int month, int day, double latitude, double longitude,
    int set_timezone,
    double altitude_meters = config::location::kDefaultObserverAltitude,
    double* out_solarNoon = nullptr, double* out_delta = nullptr) {
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
  if (kDebugLevel > 0) {
    cout << "True longitude of the Sun" << endl;
    cout << "\t           l = " << setprecision(7);
    printDeg(l);
    cout << endl;
  }

  // Sun's true anomaly
  double nu = (M + C);
  if (kDebugLevel > -1) {
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
  if (kDebugLevel > -1) {
    cout << "Apparent longitude of the Sun\n\t     lambda  = ";
    printDeg(lambda);
    cout << endl;
  }

  // nutation in obliquity
  // correction for parallax (Meeus Eq. 25.8)
  // The true or instantaneous obliquity includes the nutation.
  double Depsilon = 0.00256 * cos(Omega);
  double epsilon = epsilon0 + Depsilon;
  if (kDebugLevel > 1)
    cout << "Nutation in obliquity\n\t      Depsilon =  " << Depsilon << endl;
  if (kDebugLevel > -1) {
    cout << "Instantaneous obliquity of the Sun\n\t     epsilon = " << epsilon
         << " including nutation" << endl;
  }

  // convert to radians
  lambda *= kDeg2Rad;
  epsilon *= kDeg2Rad;

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

  if (kDebugLevel > 1) {
    // Longitude of the periapsis or longitude of the pericenter
    double tanbar = atan2(
        cos(alpha), sin(alpha) * cos(epsilon) + tan(delta) * sin(epsilon));
    if (cos(alpha) < 0) tanbar += kPi;

    cout << "Longitude of the periapsis" << endl;
    cout << "\ttanbar = " << tanbar << " radians" << endl;
    cout << "\ttanbar = " << tanbar * kRad2Deg << " degrees" << endl;

    cout << "\t      cos(R.A.) = " << cos(alpha) << endl;
  }
  // convert to degrees
  alpha *= kRad2Deg;
  delta *= kRad2Deg;

  if (kDebugLevel > 0) {
    cout << "Solar coordinates:" << endl;
    cout << "\tright ascension = " << alpha << " degrees" << endl;
    cout << "\t    declination = " << delta << " degrees (NOAA)" << endl;
  }

  /*
   * It all leads to this: calculate the Equation of Time
   */

  double e = eccentricity(t);
  if (kDebugLevel > -1) cout << "Equation of Time" << endl;

  if (kDebugLevel > 0)
    // Equation 2
    equationOfTime2(M, alpha, DPsi, epsilon, L, e);

  // Equation 3
  double E = equationOfTime3(epsilon, L, e, M);

  // adjust solar noon
  double solarNoon = getSolarNoon(longitude, set_timezone);
  solarNoon -= E;
  if (kDebugLevel > -1)
    cout << "Corrected solar noon\n\t" << solarNoon << " or "
         << hour2time(solarNoon) << endl;

  // calculate zenith
  double zenith = getZenith_Debug(e, nu, altitude_meters);

  // calculate hour angle
  double HA_deg = hourAngle(zenith, latitude, delta);

  double HA = HA_deg / 15.;

  // Convert to local solar time
  double sunsetTime = solarNoon + HA;
  double sunriseTime = solarNoon - HA;

  // Authoritative result from library (single source of truth)
  {
    sunset_calc::SunsetCalculator calc;
    double libSolarNoon = solarNoon;
    double libDelta = delta;
    double libSunset = calc.getSunset(
        year, month, day, latitude, longitude, set_timezone,
        config::location::kDefaultObserverAltitude, &libSolarNoon, &libDelta);

    // Overwrite with authoritative values to avoid divergence
    solarNoon = libSolarNoon;
    delta = libDelta;
    sunsetTime = libSunset;
    sunriseTime = solarNoon - HA;  // reuse HA for symmetry display
  }

  // Print the result
  if (kDebugLevel >= -1) {
    cout << "  Sunrise time: " << hour2time(sunriseTime) << endl;
    cout << "  Sunset time:  " << hour2time(sunsetTime) << endl;
    cout << "========================================" << endl;
  }

  // Store output parameters for caller
  if (out_solarNoon) *out_solarNoon = solarNoon;
  if (out_delta) *out_delta = delta;

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

  // Set the location and timezone from config
  double latitude = config::location::kDefaultLatitude;
  double longitude = config::location::kDefaultLongitude;
  int set_timezone = config::location::kDefaultTimezone;

  cout << "========================================" << endl;
  cout << "  Ephemeris Calculator" << endl;
  cout << "========================================" << endl;

  // Calculate the sunset time and algorithm details
  double solarNoon, delta;
  getSunset_Debug(year, month, day, latitude, longitude, set_timezone,
                  config::location::kDefaultObserverAltitude, &solarNoon,
                  &delta);

  return 0;
}
