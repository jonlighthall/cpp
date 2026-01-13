# Sunset Calculator Library - Integration Guide

## Overview

The **sunset_calc** library provides a portable, lightweight astronomical calculation engine for determining sunset and sunrise times. It's designed to work on both desktop C++ systems and embedded platforms like Arduino.

### Calculation Accuracy

This library implements **authoritative, production-quality ephemeris calculations**:
- **NOAA algorithms** - Uses formulas from the National Oceanic and Atmospheric Administration
- **Meeus (1991)** - Based on *Astronomical Algorithms*, the standard reference for astronomical calculations
- **USNO methods** - Implements U.S. Naval Observatory formulas where applicable
- **High precision** - Includes nutation corrections, equation of center with multiple harmonic terms, and quintic expansions
- **Physical constants** - Uses measured values (e.g., solar radius from Mercury transit observations)

This is NOT a simplified approximation - it provides the same level of accuracy as professional astronomical software, suitable for scientific and navigation applications.

### Algorithm Defaults by Function

The library **defaults to NOAA** algorithms (authoritative, widely adopted, appropriate precision):

| Function | Default Algorithm | Alternative(s) | Purpose |
|----------|-------------------|----------------|----------|
| `meanLongitude()` | NOAA (quadratic) | USNO (linear), LASKAR (quintic) | Geometric mean longitude |
| `meanAnomaly()` | NOAA (quadratic) | USNO (linear), LASKAR (cubic) | Mean solar anomaly |
| `equationOfCenter()` | NOAA (multi-harmonic) | USNO (constant) | Sun's equation of center |
| `obliquityOfEcliptic()` | NOAA (Lieske 1977 cubic) | USNO (linear), LASKAR (10th-order) | Earth's axial tilt |
| `longitudeAscendingNode()` | Reda & Andreas SPA (cubic) | NOAA (linear) | Ascending node for nutation |

**Design rationale**: NOAA provides the best balance of accuracy and simplicity. Academic variants (Laskar, Reda & Andreas) offer higher precision for research but aren't needed for typical applications. The library makes NOAA the default while keeping alternatives accessible for validation and comparison.

## Files

### Core Library
- **`include/sunset_calc.h`** - Public API header (Arduino-compatible)
- **`src/sunset_calc.cpp`** - Implementation
- **`include/constants.h`** - Astronomical constants (required dependency)

### Example Code
- **`examples/sunset_display.h`** - Desktop class showing library usage
- **`examples/sunset_wemos_sketch.ino`** - Arduino Wemos sketch example

## Quick Start

### Desktop C++

```cpp
#include "sunset_calc.h"
using namespace sunset_calc;

// Create calculator
SunsetCalculator calc;

// Get sunset for New York on Jan 10, 2026 (sea level)
// Parameters: year, month, day, latitude, longitude, timezone_offset, altitude_meters
double sunset = calc.getSunset(2026, 1, 10, 40.7128, -74.0060, -5, 0.0);

// Alternatively, at elevation (e.g., Denver at 1609 meters):
// double sunset = calc.getSunset(2026, 1, 10, 39.7392, -104.9903, -7, 1609.0);

// Convert to hours:minutes:seconds
int hour, minute, second;
SunsetCalculator::decimalHoursToHMS(sunset, hour, minute, second);

printf("Sunset: %02d:%02d:%02d\n", hour, minute, second);
```

### Arduino

```cpp
#include "sunset_calc.h"
using namespace sunset_calc;

SunsetCalculator calc;

// In your Arduino loop:
int year = 2026, month = 1, day = 10;
double latitude = 40.7128, longitude = -74.0060;
int timezone = -5;  // EST
double altitude = 0.0;  // meters above sea level

double sunset = calc.getSunset(year, month, day, latitude, longitude, timezone, altitude);
// Use sunset time for your application
```

## API Reference

### Main Functions

#### `double getSunset(int year, int month, int day, double latitude, double longitude, int timezone, double altitude_meters = 0.0, double* out_solarNoon = nullptr, double* out_delta = nullptr)`

Calculate sunset time for a given date and location.

**Parameters:**
- `year` - Calendar year (e.g., 2026)
- `month` - Month number (1-12)
- `day` - Day of month (1-31)
- `latitude` - Geographic latitude in degrees (-90 to 90)
- `longitude` - Geographic longitude in degrees (-180 to 180)
- `timezone` - UTC offset in hours (e.g., -5 for EST, 1 for CET)
- `altitude_meters` - (optional) Observer altitude above sea level in meters (default: 0)
- `out_solarNoon` - (optional) Pointer to receive solar noon time
- `out_delta` - (optional) Pointer to receive solar declination angle

**Returns:** Sunset time as decimal hours (0-24, e.g., 17.25 = 5:15 PM)

**Note:** Higher altitude extends sunset time. For example, at 1609 meters (Denver), sunset appears approximately 3-4 minutes later than at sea level.

#### `double getSunrise(int year, int month, int day, double latitude, double longitude, int timezone, double altitude_meters = 0.0, double* out_solarNoon = nullptr, double* out_delta = nullptr)`

Calculate sunrise time (symmetric calculation from sunset method).

**Parameters:** Same as `getSunset()`

**Returns:** Sunrise time as decimal hours

**Note:** Higher altitude extends sunrise time. For example, at 1609 meters (Denver), sunrise appears approximately 3-4 minutes earlier than at sea level.

#### `static void decimalHoursToHMS(double hours, int& outHours, int& outMinutes, int& outSeconds)`

Convert decimal hours to hours, minutes, seconds components.

**Parameters:**
- `hours` - Time in decimal hours (e.g., 17.25)
- `outHours` - Reference to store hour (0-23)
- `outMinutes` - Reference to store minute (0-59)
- `outSeconds` - Reference to store second (0-59)

**Example:**
```cpp
int h, m, s;
SunsetCalculator::decimalHoursToHMS(17.5416, h, m, s);
// h=17, m=32, s=34
```

#### `static void decimalHoursToString(double hours, char* buffer, int bufferSize, bool includeMinutes = true, bool includeSeconds = true)`

Format decimal hours as a string (requires `snprintf`).

**Parameters:**
- `hours` - Time in decimal hours
- `buffer` - Output character buffer
- `bufferSize` - Size of buffer (minimum 6 for "HH:MM", 9 for "HH:MM:SS")
- `includeMinutes` - Include minutes in output (default: true)
- `includeSeconds` - Include seconds in output (default: true)

**Example:**
```cpp
char timeStr[10];
SunsetCalculator::decimalHoursToString(17.25, timeStr, 10, true, true);
// timeStr = "17:15:00"
```

## Algorithm Selection (Research & Comparison)

The library includes support for multiple astronomical algorithm implementations to allow research-level comparison and validation. The **public API** defaults to NOAA algorithms (best balance of accuracy and efficiency), but advanced users can access alternative implementations.

### Available Algorithms

```cpp
enum class Algorithm {
  NOAA,    // Default: NOAA formulas (quadratic fits, well-balanced)
  USNO,    // U.S. Naval Observatory (linear approximations)
  LASKAR   // Laskar (1986) high-order polynomials (highest precision)
};
```

### Functions Supporting Algorithm Selection

The following helper functions can be called with optional `Algorithm` parameter:

```cpp
// In SunsetCalculator class (public methods):
double meanLongitude(double t, Algorithm algo = Algorithm::NOAA);
double meanAnomaly(double t, Algorithm algo = Algorithm::NOAA);
double equationOfCenter(double t, double M, Algorithm algo = Algorithm::NOAA);
double obliquityOfEcliptic(double T, Algorithm algo = Algorithm::NOAA);
```

### Source-Specific Algorithm Enums

Some functions have implementations from **different authoritative sources** rather than just polynomial order variants. These use function-specific enums to maintain semantic clarity about the source of each formulation.

#### Longitude of Ascending Node

```cpp
enum class LongitudeAscendingNodeFormulation {
  NOAA_LINEAR,          // NOAA's linear approximation
  REDA_ANDREAS_SPA      // Reda & Andreas (2008) cubic SPA algorithm
};

double longitudeAscendingNode(
  double t,
  LongitudeAscendingNodeFormulation form = LongitudeAscendingNodeFormulation::REDA_ANDREAS_SPA
);
```

**Parameters:**
- `t` - Julian centuries from J2000 epoch
- `form` - Which formulation to use (default: REDA_ANDREAS_SPA for higher precision)

**Returns:** Longitude of ascending node in radians

**Formulations:**
- **NOAA_LINEAR**: `125.04 - 1934.136 * t` (degrees) - Simple linear approximation from NOAA
- **REDA_ANDREAS_SPA**: `125.04452 - 1934.136261 * t + 0.0020708 * t² + t³/450000` (degrees) - Higher-precision cubic from Reda, I., & Andreas, A. (2008). Solar Position Algorithm for Solar Radiation Applications. NREL Technical Report NREL/TP-560-34302.

**When to use:**
- Default (REDA_ANDREAS_SPA) for most applications
- NOAA_LINEAR for comparison with NOAA calculator results
- Used internally for nutation calculations in high-precision ephemeris

**Example:**
```cpp
SunsetCalculator calc;
double t = 0.26;  // ~2026

// Default: higher precision
double omega = calc.longitudeAscendingNode(t);

// Compare with NOAA's simpler formula
double omega_noaa = calc.longitudeAscendingNode(
  t, LongitudeAscendingNodeFormulation::NOAA_LINEAR
);
```

### Research Example: Comparing Algorithm Differences

```cpp
#include "sunset_calc.h"
using namespace sunset_calc;

SunsetCalculator calc;

// For a given date, compare algorithm variants
double t = 0.26;  // Julian centuries from J2000
double L_noaa = calc.meanLongitude(t, Algorithm::NOAA);
double L_usno = calc.meanLongitude(t, Algorithm::USNO);
double L_laskar = calc.meanLongitude(t, Algorithm::LASKAR);

printf("Mean Longitude - NOAA: %.6f, USNO: %.6f, LASKAR: %.6f\n", L_noaa, L_usno, L_laskar);
```

### Recommendation

For standard applications, use the **default (NOAA)** algorithms:

```cpp
double sunset = calc.getSunset(2026, 1, 10, 40.71, -74.01, -5);  // Uses NOAA internally
```

Algorithm selection is provided **for research and educational purposes** to:
- Validate results against multiple sources
- Compare accuracy of different methods
- Study how algorithm choice affects calculated sunset times

## Integration Patterns

### Pattern 1: Simple Sunset Calculator

```cpp
// Minimal example
#include "sunset_calc.h"
using namespace sunset_calc;

SunsetCalculator calc;
double sunset = calc.getSunset(2026, 1, 10, 40.71, -74.01, -5);
printf("Sunset: %.2f hours\n", sunset);
```

### Pattern 2: Time Remaining Until Sunset

```cpp
double sunset = calc.getSunset(year, month, day, lat, lon, tz);
double currentTime = hour + minute / 60.0;
double timeRemaining = sunset - currentTime;

if (timeRemaining < 0) {
    printf("Sunset was %.1f hours ago\n", -timeRemaining);
} else {
    int h = (int)timeRemaining;
    int m = (int)((timeRemaining - h) * 60);
    printf("Sunset in %d:%02d\n", h, m);
}
```

### Pattern 2b: Commute Planning with Civil Twilight

For practical purposes like commute planning, **civil twilight ending** (6° below horizon) is often more useful than astronomical sunset. Civil twilight is when it becomes legally dark for driving purposes.

```cpp
#include "sunset_calc.h"
#include "solar_utils.h"  // For sunAngleToZenith, calcHourAngle

// Get solar parameters
double solarNoon, delta;
calc.getSunset(year, month, day, lat, lon, tz, altitude, &solarNoon, &delta);

// Calculate civil twilight ending (6° below horizon)
double civilTwilightZenith = solar_utils::sunAngleToZenith(6.0);
double civilTwilightHA = solar_utils::calcHourAngle(civilTwilightZenith, lat, delta);
double civilTwilightEnd = solarNoon + (civilTwilightHA / 15.0);

// Calculate when to leave for evening commute
const double commuteMinutes = 25.0;
double leaveTime = civilTwilightEnd - (commuteMinutes / 60.0);

printf("Leave by %02d:%02d to get home by civil twilight\n",
       (int)leaveTime, (int)((leaveTime - (int)leaveTime) * 60));
```

**Note**: Civil twilight ending occurs approximately 20-30 minutes after sunset, depending on latitude and time of year.

### Pattern 3: Arduino Display Integration

```cpp
// Update two displays every second
void updateDisplays() {
    // Display 1: Current NTP time
    displayTime(display1, currentHour, currentMinute);

    // Display 2: Time until sunset
    double sunset = calc.getSunset(year, month, day, LAT, LON, TZ);
    double now = currentHour + currentMinute / 60.0;
    double timeLeft = sunset - now;

    int h = (int)timeLeft;
    int m = (int)((timeLeft - h) * 60);
    displayTime(display2, h, m);
}
```

## Design Considerations

### Design Philosophy
The library was extracted from a larger sunset calculation program to enable reuse across different platforms (desktop, Arduino, embedded systems). Key design decisions:

- **Separation of concerns**: Pure calculation functions isolated from I/O and display logic
- **Minimal dependencies**: Only requires `<cmath>` and `constants.h` for maximum portability
- **Class-based encapsulation**: `SunsetCalculator` class encapsulates all state and methods
- **Static utilities**: Time conversion functions provided as static methods (no instance needed)
- **Optional outputs**: Functions can return additional data (solar noon, declination) via optional pointers
- **Namespace isolation**: `sunset_calc::` namespace prevents conflicts with existing code

### Accuracy
- Calculations accurate to within ±2-3 minutes
- Includes atmospheric refraction and solar disk size
- Uses NOAA algorithm (reference: https://www.esrl.noaa.gov/gmd/grad/solcalc/)

### Performance
- `getSunset()` calculation: ~1-2ms on modern systems, ~5-10ms on Arduino
- Memory: ~500 bytes RAM for SunsetCalculator instance
- Can be called every loop iteration without performance concerns

### Platform Compatibility
- **Desktop**: Works with any C++11 or later compiler
- **Arduino**: Tested on Arduino Uno, Wemos D1 Mini
- **Dependencies**: Only requires `<cmath>` and `constants.h`
- **No floating-point libraries**: Safe for systems with limited FPU support

### Limitations & Design Constraints

**Current Limitations:**
- Observer altitude supported (extends sunset/sunrise by ~3-4 min per 1600m)
- Does not account for local terrain/obstructions
- Assumes spherical Earth (difference: <0.5 minutes for typical locations)
- Date range varies by algorithm (conservative: 1900-2100 for all variants)

**Known Future Work:**
- **Fractional timezones**: Currently requires integer hour offsets (e.g., -5, +1). Does not support +5:30, +9:45, etc.
- **Daylight Saving Time**: Not automatically handled; must apply DST offset externally
- **Polar regions**: No special handling for latitudes near ±90° where sun may not set/rise for extended periods
- **Date normalization**: Calculations crossing midnight may need interpretation

**Platform Constraints (Arduino):**
- **Target**: Wemos D1 Mini (limited RAM/flash)
- **Memory footprint**: ~500 bytes RAM for `SunsetCalculator` instance
- **Dependencies**: Only `<cmath>` and `constants.h` (no `iostream`, no STL containers)
- **Performance**: ~5-10ms per sunset calculation on Arduino, ~1-2ms on desktop

**Precision:**
- Sunset/sunrise: ±2-3 minutes typical accuracy
- "Good enough" for practical applications (commute planning, automation)
- Not intended for professional navigation or precise astronomical research

## Porting to Arduino

### Step 1: Setup Files
```
sketch/
  ├── sunset_wemos.ino
  ├── sunset_calc.h
  ├── sunset_calc.cpp
  └── constants.h
```

### Step 2: Dependencies
- None beyond standard Arduino libraries
- WiFi.h for NTP synchronization (optional)
- TM1637Display for 7-segment displays (optional)

### Step 3: Modify for Your Needs

**Location**: Edit coordinates in sketch
```cpp
const double LATITUDE = 40.7128;    // New York
const double LONGITUDE = -74.0060;  // New York
const int TIMEZONE = -5;            // EST
```

**Timezone**: Adjust for your location
```cpp
// Daylight Saving Time - adjust offset if needed
// Standard times: EST=-5, CST=-6, MST=-7, PST=-8
const int TIMEZONE = -5;
```

**Display Hardware**: Choose your display type
- TM1637 (4-digit 7-segment)
- SSD1306 (128x64 OLED)
- MAX7219 (8x8 LED matrix)
- LCD 16x2

### Step 4: Time Synchronization

Option A - NTP over WiFi (recommended):
```cpp
configTime(TIMEZONE * 3600, 0, "pool.ntp.org", "time.nist.gov");
```

Option B - Manual time set:
```cpp
time_t t = mktime(&timeinfo);  // Set manually if no WiFi
```

## Examples

### Example 1: Calculate Golden Hour

```cpp
// Golden hour: 1 hour before to 1 hour after sunset
double sunset = calc.getSunset(year, month, day, lat, lon, tz);
double now = currentHour + currentMinute / 60.0;

bool isGoldenHour = (now > (sunset - 1.0)) && (now < (sunset + 1.0));
```

### Example 2: Scheduled Notification

```cpp
double sunset = calc.getSunset(year, month, day, lat, lon, tz);
double leaveTime = sunset - 0.5;  // Leave 30 minutes before sunset

if (currentTime > leaveTime && !notificationSent) {
    sendNotification("Time to go home!");
    notificationSent = true;
}
```

### Example 3: Multi-Location Calculator

```cpp
struct Location {
    const char* name;
    double lat, lon;
    int tz;
};

Location locations[] = {
    {"New York", 40.7128, -74.0060, -5},
    {"London", 51.5074, -0.1278, 0},
    {"Tokyo", 35.6762, 139.6503, 9}
};

for (auto& loc : locations) {
    double sunset = calc.getSunset(2026, 1, 10, loc.lat, loc.lon, loc.tz);
    printf("%s: %02d:%02d\n", loc.name, (int)sunset,
           (int)((sunset - (int)sunset) * 60));
}
```

## Troubleshooting

### Build Errors

**Error: undefined reference to kDeg2Rad**
- Solution: Ensure `constants.h` is included and compiled

**Error: no matching function for getSunset**
- Solution: Ensure `sunset_calc.h` is included before use

### Runtime Issues

**Sunset time is 24.5 hours away**
- This means sunset has passed today; time remaining is for tomorrow
- Handle with: `if (timeRemaining < 0) timeRemaining += 24.0;`

**Time is off by several hours**
- Check timezone offset (signed integer, positive = East of UTC)
- Verify system time is synchronized with NTP

**Calculations seem wrong**
- Verify latitude/longitude are in correct format (degrees, not minutes)
- Check date is valid (1-31, 1-12 format)

## References

- **Solar Position Algorithm**: [James Still's Solar Coordinates](https://squarewidget.com/solar-coordinates/)
- **NOAA Calculator**: [ESRL Global Monitoring Laboratory](https://www.esrl.noaa.gov/gmd/grad/solcalc/)
- **USNO Algorithm**: [U.S. Naval Observatory](https://aa.usno.navy.mil/faq/sun_approx)
- **Astronomical Algorithms**: Jean Meeus (1991)

## License

Same as parent project (see LICENSE file)

## Contributing

To improve the library:
1. Test changes on both desktop and Arduino
2. Verify sunset times against NOAA calculator
3. Keep dependencies minimal
4. Document any changes to the public API
