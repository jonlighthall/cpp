/**
 * Arduino Sketch Example: Two 4-Digit 7-Segment Display Clock
 *
 * This example shows how to adapt the sunset calculator for an Arduino Wemos
 * project with two 4-digit 7-segment displays:
 *   Display 1: Shows current NTP time (HH:MM)
 *   Display 2: Shows time until sunset (HH:MM)
 *
 * Hardware:
 *   - Arduino Wemos D1 Mini (or compatible)
 *   - Two TM1637 4-digit 7-segment displays
 *   - WiFi for NTP time synchronization
 *
 * Libraries needed:
 *   - TM1637 (for display control)
 *   - WiFi (built-in)
 *   - Time (for NTP sync)
 *
 * Setup:
 *   1. Copy sunset_calc.h and sunset_calc.cpp to project folder
 *   2. Copy constants.h to project folder
 *   3. Install TM1637 library: Sketch > Include Library > Manage Libraries
 *   4. Update WiFi credentials and timezone as needed
 */

// #include <Arduino.h>
// #include <WiFi.h>
// #include <time.h>
// #include "TM1637Display.h"
// #include "sunset_calc.h"

// using namespace sunset_calc;

// ============================================================================
// Configuration
// ============================================================================

// WiFi credentials
// const char* ssid = "YOUR_SSID";
// const char* password = "YOUR_PASSWORD";

// Display pins (adjust for your wiring)
// const int CLK1 = D5;   // Display 1 clock (NTP time)
// const int DIO1 = D6;   // Display 1 data
// const int CLK2 = D7;   // Display 2 clock (time to sunset)
// const int DIO2 = D8;   // Display 2 data

// Location (set to your location)
// const double LATITUDE = 40.7128;    // New York
// const double LONGITUDE = -74.0060;  // New York
// const int TIMEZONE = -5;            // EST

// ============================================================================
// Global Objects
// ============================================================================

// TM1637Display display1(CLK1, DIO1);  // NTP time display
// TM1637Display display2(CLK2, DIO2);  // Time to sunset display
// SunsetCalculator sunsetCalc;

// ============================================================================
// Setup Function
// ============================================================================

// void setup() {
//   Serial.begin(115200);
//   delay(100);

//   // Initialize displays
//   display1.setBrightness(0x0f);  // Max brightness
//   display2.setBrightness(0x0f);

//   Serial.println("\n\nStarting Sunset Display Clock...");

//   // Connect to WiFi
//   connectToWiFi();

//   // Synchronize time with NTP
//   syncTimeWithNTP();
// }

// ============================================================================
// Main Loop
// ============================================================================

// void loop() {
//   // Get current date and time (from NTP)
//   time_t now = time(nullptr);
//   struct tm* timeinfo = localtime(&now);

//   int year = timeinfo->tm_year + 1900;
//   int month = timeinfo->tm_mon + 1;
//   int day = timeinfo->tm_mday;
//   int hour = timeinfo->tm_hour;
//   int minute = timeinfo->tm_min;

//   // Display 1: Show current NTP time (HH:MM)
//   displayTime(display1, hour, minute);

//   // Display 2: Calculate and show time until sunset
//   double sunsetTime = sunsetCalc.getSunset(year, month, day, LATITUDE,
//                                             LONGITUDE, TIMEZONE);
//   double currentTimeDecimal = hour + minute / 60.0;
//   double timeUntilSunset = sunsetTime - currentTimeDecimal;

//   // Handle case where sunset has passed
//   if (timeUntilSunset < 0) {
//     timeUntilSunset += 24.0;  // Next day's sunset
//   }

//   // Convert decimal hours to HH:MM
//   int hoursUntilSunset = (int)timeUntilSunset;
//   int minutesUntilSunset = (int)((timeUntilSunset - hoursUntilSunset) *
//   60);

//   displayTime(display2, hoursUntilSunset, minutesUntilSunset);

//   // Update display every 10 seconds
//   delay(10000);
// }

// ============================================================================
// Helper Functions
// ============================================================================

// /**
//  * Display hours and minutes on a 4-digit 7-segment display
//  * Format: HH:MM (with colon separator)
//  */
// void displayTime(TM1637Display& display, int hours, int minutes) {
//   // Create display data: HH:MM
//   uint8_t data[] = {
//       display.encodeDigit(hours / 10),      // First digit of hour
//       display.encodeDigit(hours % 10),      // Second digit of hour
//       display.encodeDigit(minutes / 10),    // First digit of minute
//       display.encodeDigit(minutes % 10)     // Second digit of minute
//   };

//   // Display with colon between hour and minute
//   display.setSegments(data, sizeof(data), 0);
//   // Alternative: use data[1] with 0x80 to show colon
//   data[1] |= 0x80;  // Set colon bit
//   display.setSegments(data, sizeof(data), 0);
// }

// /**
//  * Connect to WiFi network
//  */
// void connectToWiFi() {
//   Serial.print("Connecting to WiFi: ");
//   Serial.println(ssid);

//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);

//   int attempts = 0;
//   while (WiFi.status() != WL_CONNECTED && attempts < 20) {
//     delay(500);
//     Serial.print(".");
//     attempts++;
//   }

//   if (WiFi.status() == WL_CONNECTED) {
//     Serial.println("\nWiFi connected!");
//     Serial.print("IP address: ");
//     Serial.println(WiFi.localIP());
//   } else {
//     Serial.println("\nFailed to connect to WiFi");
//   }
// }

// /**
//  * Synchronize time with NTP (Network Time Protocol)
//  * Uses pool.ntp.org as time source
//  */
// void syncTimeWithNTP() {
//   Serial.println("Syncing time with NTP...");

//   // Configure time with NTP server
//   // Set timezone: UTC offset in seconds (EST = -5 hours = -18000 seconds)
//   configTime(TIMEZONE * 3600, 0, "pool.ntp.org", "time.nist.gov");

//   // Wait for time to be set
//   Serial.print("Waiting for NTP time sync: ");
//   time_t now = time(nullptr);
//   int attempts = 0;

//   while (now < 24 * 3600 && attempts < 20) {
//     delay(500);
//     Serial.print(".");
//     now = time(nullptr);
//     attempts++;
//   }

//   Serial.println();
//   Serial.print("Current time: ");
//   Serial.println(ctime(&now));
// }

// ============================================================================
// Notes for Deployment
// ============================================================================

/*
BEFORE uploading to Arduino:

1. Uncomment all the code above
2. Update WiFi credentials (ssid, password)
3. Update location coordinates (LATITUDE, LONGITUDE, TIMEZONE)
4. Verify pin assignments match your wiring

File organization:
  sketch/
    ├── sunset_wemos.ino (this file)
    ├── sunset_calc.h
    ├── sunset_calc.cpp
    ├── constants.h
    └── libraries/
        └── (TM1637Display library)

Memory notes:
  - SunsetCalculator uses ~500 bytes of RAM
  - sunset_calc.cpp uses ~200 bytes of ROM for functions
  - Total code size is minimal and fits easily on Arduino

Performance:
  - getSunset() calculation takes ~1-2ms
  - Safe to call every loop iteration
  - Typically called once per minute or once per second

Modifications for different displays:
  - SSD1306 OLED: Replace TM1637Display with Adafruit_SSD1306
  - MAX7219 LED matrix: Use LedControl library
  - LCD 16x2: Use LiquidCrystal library
  - Still use sunset_calc.h for calculations in all cases
*/
