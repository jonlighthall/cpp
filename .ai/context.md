# Context

**Purpose:** Facts, decisions, and history for AI agents working on this project.

---

## Author

**Name:** Jon Lighthall
**GitHub:** jonlighthall
**Domain:** Embedded systems, astronomical calculations, C++ learning

**Primary tools:**
- C++ (GCC/G++)
- VS Code
- Linux (WSL and native)
- Arduino/embedded (Wemos D1 Mini)

**Writing preferences:**
- Impersonal voice for expository writing
- Mathematical "we" acceptable ("we substitute...", "we obtain...")
- Avoid editorial "we" ("we believe...", "we recommend...")
- Heavy commenting for educational purposes

---

## Repository Identity

**Type**: Learning/Educational Repository
**Owner**: Personal project (jonlighthall)
**Primary Language**: C++
**Development Stage**: Active learning environment

### Core Purpose
This is an **experimental learning repository** for C++ development. It is intentionally kept simple and educational rather than production-ready. Programs here are learning exercises, examples, and prototypes.

## Repository Relationships

### Sister Repository: diff_utils
- **URL**: https://github.com/jonlighthall/diff_utils
- **Relationship**: Graduation target for mature programs
- **Pattern**: Programs start in `cpp/`, move to dedicated repos when they exceed ~2,000 lines or become production-ready

### Graduated Programs
1. **uband_diff** (2024) → Migrated to diff_utils
   - Was: ~2,150 lines in cpp folder
   - Now: ~5,400 lines in diff_utils with enhancements
   - Reason: Grew beyond learning exercise into production tool
   - Cleanup: All related infrastructure (difference_analyzer, file_reader, format_tracker, line_parser, column_analyzer, tests) removed from cpp repo

### Active Programs

**Educational/Research:**
- **ephemeris** (~600 lines) - Algorithm comparison showing NOAA/USNO/Laskar calculations
  - Full debug output demonstrating every calculation step
  - Shows alternative algorithms and their differences
  - Research tool for validating calculation methodology
  - **Extracted from sunset.cpp** (Jan 2026): Separated pedagogical code into dedicated program

**Production/Practical:**
- **sunset** (~200 lines) - Simple evening commute planner
  - Uses library defaults (NOAA) for efficient calculation
  - Shows civil twilight ending time and "leave by" calculation
  - Includes twilight event table for reference
  - **Refactored from sunset.cpp** (Jan 2026): Removed all debug/comparison code
- **sunrise** (~200 lines) - Morning commute and dawn twilight planner
  - Uses library defaults (NOAA) for efficient calculation
  - Dual table: morning arrival + evening departure
- **twilight** (~200 lines) - Evening twilight event table
  - Uses library defaults
  - Prints multiple twilight types for comparison with online resources

**Learning Examples:**
- **hello** (~90 lines) - Basic example
- **modular_test** (~218 lines) - Math operations demo
- **print_aligned** (~60 lines) - Formatting example

## Development Philosophy

### Design Principles
1. **Educational First**: Code clarity over optimization
2. **Progressive Complexity**: Start simple, add features incrementally
3. **Keep or Graduate**: Programs either stay simple or move to dedicated repos
4. **No Premature Abstraction**: Don't over-engineer learning code

### Build System
- **Generic Makefile**: Links ALL object files to ALL executables
- **Intentional Bloat**: Simplicity for learning over efficiency
- **Rationale**: Easier for beginners to add new programs without makefile expertise

Note: The makefile links shared table objects (morning/twilight) into all binaries. This is intentional and keeps setup lightweight.

### Coding Style
- Mix of procedural and OOP (learning both paradigms)
- Direct implementations before abstractions
- Heavy commenting for educational purposes
- Debug output often left in code for learning

## Recent Significant Changes

### Program Split: Pedagogical vs Production (January 2026)
**What Happened**: Separated educational algorithm comparison code from practical commute planner

**Created**:
- `main/ephemeris.cpp` (~600 lines) - Educational algorithm comparison
  - Shows NOAA, USNO, Laskar calculations with full debug output
  - Demonstrates every calculation step for learning
  - Research tool for validating algorithm differences
  - **Extracted from sunset.cpp**: All `_Debug` functions and comparison logic

**Refactored**:
- `main/sunset.cpp` (~200 lines) - Simple evening commute planner
  - Removed all pedagogical debug code
  - Uses library defaults (NOAA) for efficient calculation
  - Shows civil twilight ending time and "leave by" calculation
  - Includes twilight event table for reference
  - Clean, simple output focused on practical commute planning

**Design Pattern**:
- Separation of concerns: Educational vs production use
- Programs split by PURPOSE, not COMPLEXITY
- Makefile automatically picks up new `ephemeris.cpp` executable
- Both programs use same library (`sunset_calc`) as single source of truth

**Benefit**: Users can choose:
- Run `ephemeris` to understand *how* calculations work
- Run `sunset` to get practical commute times
- Both use identical library for authoritative results

### Sunset Calculator Hardening (January 2026)
**What Happened**: Added input validation and error handling to sunset_calc library

**Added**:
- `SunsetCalculator::validateInputs()` - Validates all input parameters
- Input range checking (year: 1900-2100, month: 1-12, day: 1-31, latitude: -90 to 90, longitude: -180 to 180, timezone: -12 to 14)
- Error return codes (returns 24.0 for invalid sunset, -1.0 for invalid sunrise)
- Main program now checks for calculation failures and reports errors

**Known Limitations** (documented):
- Does not handle polar regions (latitude near ±90°) where sun is above/below horizon continuously
- Timezone must be integer hours (doesn't support +5:30, +9:45, etc.)
- No daylight saving time adjustment
- Month-specific day validation not implemented (e.g., doesn't reject Feb 30)
- Date normalization needs improvement when calculations cross midnight

**Design Philosophy**: Educational code can be strict about inputs while remaining simple

### Sunset Calculator Library Extraction (January 2026)
**What Happened**: Extracted core astronomical calculations from `sunset.cpp` into reusable library

**Created**:
- `include/sunset_calc.h` - Portable calculation API
- `src/sunset_calc.cpp` - Implementation (~350 lines)
- `examples/sunset_wemos_sketch.ino` - Arduino example
- `examples/sunset_display.h` - Desktop wrapper
- `docs/SUNSET_CALC_LIBRARY.md` - Integration guide
- `examples/QUICK_REFERENCE.md` - Quick start
 - Shared utilities for table renderers:
    - `include/colors.h`, `include/solar_utils.h`, `include/format_utils.h`, `include/text_utils.h`, `include/table_layout.h`
    - Evening renderer: `src/twilight_table.cpp`
    - Morning renderer: `src/morning_table.cpp`

**Original Program**: `main/sunset.cpp` remains unchanged (~910 lines with UI/formatting)

**Design Decision**: Library enables Arduino/embedded use while keeping original program as learning example with full debug output

**Key Insight**: This demonstrates separation of concerns without requiring refactor of original code - both can coexist

### New Programs (January 2026)
**sunrise**: New CLI focuses on morning commute planning. Uses the library for sunrise, then prints a dawn table with golden hour start/end and twilight begin events. **Status messages reference civil twilight ending** (6° below horizon, legally dark for driving) rather than sunset for practical commute planning.

**twilight**: Evening-only table shows sunset, golden hour end, and twilight end events with ETAs. Also uses civil twilight as primary reference.

**Design Decision**: Keep morning and evening views separate for clarity. A future `--morning` flag in `twilight` is possible but not yet implemented.

### Morning Table Enhancement (January 2026)
**What Happened**: Enhanced morning_table to show both morning arrival and evening departure timing

**Design**:
- **Two-section display**: Morning arrival uses single commute time, evening departure uses 2× commute + 8.5-hour workday
- **Practical focus**: Status messages reference "getting back home by civil twilight" rather than sunrise
- **Civil twilight as reference**: Both sunrise and sunset programs use civil twilight ending (6° below horizon) as the practical "legally dark" threshold for commute planning

**Rationale**: Civil twilight ending (~20 minutes after sunset) is when it becomes legally dark for driving, making it a more relevant reference for evening commute planning than astronomical sunset.

### Algorithm Source Attribution (January 2026)
**What Happened**: Added source-specific enums for functions with multiple authoritative implementations

**Pattern Established**:
- Generic `Algorithm` enum (NOAA, USNO, LASKAR) for functions with polynomial order variants
- Function-specific enums (e.g., `LongitudeAscendingNodeFormulation`) when algorithms come from different authoritative sources

**Example: longitudeAscendingNode**:
- NOAA uses linear approximation: `125.04 - 1934.136 * t`
- Reda & Andreas (2008) NREL/TP-560-34302 uses cubic: `125.04452 - 1934.136261 * t + 0.0020708 * t² + t³/450000`
- These are from different authoritative sources, not just polynomial variants

**Moved to Public API**: `longitudeAscendingNode()` now available in public interface for advanced users needing nutation calculations

### Repository Cleanup (January 2026)
**Removed**: 14 files related to uband_diff after confirming complete migration
**Updated**: README with "Repository Status" section documenting graduated programs

## Design Philosophy & Priorities

### Algorithm Selection Strategy
**Default = NOAA** (authoritative, widely adopted, "good enough")
- Primary API uses NOAA algorithms as defaults
- Well-documented, matches major online calculators
- Appropriate precision for practical applications

**Academic Variants = Research/Comparison**
- Reda & Andreas (2008): Higher precision for scientific work
- Laskar (1986): Maximum polynomial precision
- Available but not default; used for validation and educational comparison

### Platform Constraints
**Arduino Target**: Wemos D1 Mini
- Library must fit in limited memory
- Keep dependencies minimal (only `<cmath>` and `constants.h`)
- Avoid heavy standard library features (no `iostream` in library code)

### Known Limitations (Future Work)
- **Fractional timezones**: Not intentionally excluded, but personal use unlikely to need +5:30 offsets
- **DST handling**: Should be added (check if implemented)
- **Polar regions**: Edge cases (continuous day/night) not a priority
- **Formal uncertainty analysis**: "Good enough" precision acceptable for now

---

## Code Organization

### Directory Structure
```
cpp/
├── main/           # Main programs (entry points)
├── src/            # Reusable implementations
├── include/        # Header files
├── bin/            # Compiled binaries (generated)
├── obj/            # Object files (generated)
├── examples/       # Example code and documentation
├── docs/           # Technical documentation
└── .ai/            # AI agent context (this folder)
```

Renderer modules:
- Morning: `src/morning_table.cpp` (API in `include/morning_table.h`)
- Evening: `src/twilight_table.cpp` (API in `include/twilight_table.h`)

### File Naming Conventions
- `main/*.cpp` - Executable programs
- `src/*.cpp` - Library implementations
- `include/*.h` - Public interfaces
- No .hpp distinction (keep it simple)

### Configuration Files
- `include/config_location.h` - Location settings (lat/lon/tz/altitude)
- `include/config_commute.h` - Commute settings (used by sunrise/sunset)
- `include/config_ephemeris.h` - Debug/algorithm settings (ephemeris only)
- `include/constants.h` - Astronomical/mathematical constants
- Namespace usage: `config::`, `sunset_calc::`

---

## Testing and Validation

### Current Approach
- Manual testing with known good values
- Compare against NOAA solar calculator
- Validated against USNO algorithms
- No formal test framework (learning repo)

---

## References

### Sunset Calculator Sources
- NOAA Solar Calculator: https://www.esrl.noaa.gov/gmd/grad/solcalc/
- USNO: https://aa.usno.navy.mil/faq/sun_approx
- James Still's article: https://squarewidget.com/solar-coordinates/
- Meeus, Jean (1991). *Astronomical Algorithms*

---

*For procedures and standing orders, see `instructions.md`.*
