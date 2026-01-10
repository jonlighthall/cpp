# Sunset Calculator Library - Quick Reference

## Files to Copy to Your Arduino Project

```
sketch/
  ├── sunset_calc.h          ← Copy from include/
  ├── sunset_calc.cpp        ← Copy from src/
  ├── constants.h            ← Copy from include/
  └── sunset_wemos.ino       ← Your Arduino sketch
```

## Basic Usage (5 lines of code)

```cpp
#include "sunset_calc.h"
using namespace sunset_calc;

SunsetCalculator calc;
double sunset = calc.getSunset(2026, 1, 10, 40.7128, -74.0060, -5);
// sunset = 17.291 hours (5:17 PM)
```

## Convert to Hours:Minutes

```cpp
int h, m, s;
SunsetCalculator::decimalHoursToHMS(sunset, h, m, s);
// h=17, m=17, s=26
```

## For Your Wemos Display Project

```cpp
// Display 1: NTP Time
displayTime(display1, currentHour, currentMinute);

// Display 2: Time until sunset
double sunset = calc.getSunset(year, month, day, LAT, LON, TZ);
double now = currentHour + currentMinute / 60.0;
double timeLeft = sunset - now;
if (timeLeft < 0) timeLeft += 24.0;  // Next day

int h = (int)timeLeft;
int m = (int)((timeLeft - h) * 60);
displayTime(display2, h, m);
```

## Arduino Wemos Setup Checklist

- [ ] Copy 3 files to sketch folder (sunset_calc.h/cpp, constants.h)
- [ ] Uncomment Arduino code in sunset_wemos_sketch.ino
- [ ] Update WiFi SSID/password
- [ ] Update LATITUDE, LONGITUDE, TIMEZONE for your location
- [ ] Update TM1637 pin assignments if needed
- [ ] Install TM1637Display library (if using it)
- [ ] Compile and upload

## Location Coordinates

Find your location:
1. Google Maps: right-click on your location, click coordinates
2. Format: Latitude (N is +, S is -), Longitude (E is +, W is -)

Examples:
- New York: 40.7128, -74.0060
- London: 51.5074, -0.1278
- Sydney: -33.8688, 151.2093
- Tokyo: 35.6762, 139.6503

## Timezone Offsets

```
UTC-12: Baker Island, American Samoa
UTC-10: Hawaii
UTC-8:  Pacific (Los Angeles, Vancouver)
UTC-7:  Mountain (Denver, Phoenix)  *Arizona no DST
UTC-6:  Central (Chicago, Mexico City)
UTC-5:  Eastern (New York, Miami)
UTC-4:  Atlantic (Halifax, Caracas)
UTC-3:  Brasília, Argentina
UTC-2:  Mid-Atlantic
UTC-1:  Azores, Cape Verde
UTC+0:  London, Dublin, Lisbon
UTC+1:  Central Europe (Paris, Berlin)
UTC+2:  Eastern Europe, Cairo, Istanbul
UTC+3:  Moscow, East Africa
UTC+4:  Dubai, Caucasus
UTC+5:  Pakistan, Kazakhstan
UTC+5:30: India
UTC+6:  Central Asia, Bangladesh
UTC+7:  Thailand, Vietnam, Indonesia
UTC+8:  China, Singapore, Australia (Perth)
UTC+9:  Tokyo, Seoul, Australia (Sydney part)
UTC+10: Australia (Sydney, Melbourne)
UTC+11: Solomon Islands, Australia (east coast)
UTC+12: Fiji, New Zealand
```

Note: Add 1 hour during Daylight Saving Time if applicable

## Common Display Libraries

```cpp
// TM1637 (4-digit 7-segment)
#include "TM1637Display.h"
TM1637Display display(CLK, DIO);

// SSD1306 OLED (128x64)
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(128, 64, &Wire);

// MAX7219 LED Matrix
#include "LedControl.h"
LedControl lc = LedControl(DIN, CLK, CS, NUM_DEVICES);

// LCD 16x2
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
```

## Memory Usage on Arduino

- Library: ~200 bytes code + 500 bytes for SunsetCalculator instance
- Sunset calculation: ~5-10ms execution time
- Safe to call every second without performance issues

## Testing Before Arduino

Test the library in desktop mode first:

```bash
g++ -I include -I src -o test_sunset examples/sunset_display.h \
    src/sunset_calc.cpp include/sunset_calc.h
./test_sunset
```

This will show sunset/sunrise times for today in your location.

## Troubleshooting Checklist

- [ ] Did you copy all 3 files? (sunset_calc.h/cpp, constants.h)
- [ ] Is `#include "sunset_calc.h"` in your sketch?
- [ ] Is `using namespace sunset_calc;` declared?
- [ ] Is WiFi connected and NTP time synced?
- [ ] Is timezone offset correct (signed: -5 not 5)?
- [ ] Is latitude/longitude in decimal degrees (not minutes)?

## Typical Use Pattern

```cpp
void setup() {
    // Initialize displays
    // Connect to WiFi
    // Sync time with NTP
}

void loop() {
    // Get current time
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);

    // Display NTP time
    displayTime(display1, timeinfo->tm_hour, timeinfo->tm_min);

    // Calculate and display time until sunset
    int year = timeinfo->tm_year + 1900;
    int month = timeinfo->tm_mon + 1;
    int day = timeinfo->tm_mday;

    double sunset = calc.getSunset(year, month, day, LAT, LON, TZ);
    double now_decimal = timeinfo->tm_hour + timeinfo->tm_min / 60.0;
    double time_left = sunset - now_decimal;

    if (time_left < 0) time_left += 24.0;

    int h = (int)time_left;
    int m = (int)((time_left - h) * 60);
    displayTime(display2, h, m);

    delay(1000);  // Update once per second
}
```

---

**Need help?** See `docs/SUNSET_CALC_LIBRARY.md` for complete documentation.
