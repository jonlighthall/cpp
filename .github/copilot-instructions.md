# GitHub Copilot Instructions for cpp Repository

This repository is a **learning/educational C++ environment**. Suggestions should prioritize clarity and teaching over production patterns.

## Repository Context

- **Purpose**: Learning C++ through examples and experiments
- **Code Style**: Simple, well-commented, educational
- **Build**: Generic makefile (intentionally simple)
- **Graduation Pattern**: Programs move to dedicated repos when mature (>2K lines)

## Recent History

- **uband_diff**: Graduated to diff_utils repo (removed from cpp/)
- **sunset_calc**: Library extracted from sunset.cpp for Arduino reuse
- **ephemeris.cpp**: New educational program showing algorithm comparisons (NOAA/USNO/Laskar)
- **sunset.cpp**: Refactored to simple evening commute planner (clean, no debug verbosity)
- **sunrise**: Morning commute planner + dawn table
- **twilight**: Evening-only table renderer using shared utilities

## Suggestion Guidelines

### Prefer:
- Simple, educational approaches
- Explanations of trade-offs
- Patterns that teach concepts
- Arduino-compatible code for sunset_calc library

### Avoid:
- Over-engineering learning examples
- Production patterns for simple demos
- Refactoring tiny programs (hello, modular_test)
- Complex abstractions that obscure learning
- iostream in sunset_calc (must stay Arduino-compatible)

## Key Programs

**ephemeris.cpp**: Educational program showing algorithm comparisons (NOAA/USNO/Laskar with full debug output)
**sunset.cpp**: Simple evening commute planner using library defaults
**sunrise.cpp**: Morning/evening dual commute table planner
**twilight.cpp**: Evening twilight table renderer
**sunset_calc**: Portable library (desktop + Arduino) - minimize dependencies
**hello/modular_test/print_aligned**: Keep simple - these are learning examples

### Program Architecture: Educational vs Production

Programs are split by purpose:

1. **Educational/Research Program**
   - **`ephemeris.cpp`** - Algorithm comparison showing NOAA/USNO/Laskar variants
   - Full debug output showing every calculation step
   - Demonstrates pedagogical methodology alongside authoritative results
   - Research tool for validating algorithm differences

2. **Production/Practical Programs**
   - **`sunset.cpp`** - Evening commute planner (simple, clean output)
   - **`sunrise.cpp`** - Morning/evening dual schedule planner
   - **`twilight.cpp`** - Evening twilight reference table
   - Use library defaults (NOAA) for reliable, efficient calculations
   - No debug verbosity; clear commute planning output

3. **Learning Examples**
   - **`hello.cpp`**, **`modular_test.cpp`**, **`print_aligned.cpp`** - Simple teaching programs

### Dual Implementation Approach

The sunset calculator uses **two separate implementations** to serve different purposes:

1. **Library Implementation** (`sunset_calc.h/.cpp`)
   - The authoritative, production-quality ephemeris calculator
   - Used by practical programs (`sunset.cpp`, `sunrise.cpp`, `twilight.cpp`)
   - Minimal dependencies, Arduino-compatible
   - Functions: `getSunset()`, `getSunrise()`, `getZenith()`
   - **Algorithm Selection**: Supports multiple implementations via enums
     - Generic `Algorithm` enum for polynomial variants (NOAA, USNO, LASKAR)
     - Function-specific enums for source attribution (e.g., `LongitudeAscendingNodeFormulation`)

2. **Educational Program** (`ephemeris.cpp`)
   - Full educational walkthrough of every calculation step
   - Extensive debug output showing intermediate values and alternatives
   - Shows NOAA, USNO, and Laskar results for comparison
   - Calls library functions to display authoritative results
   - Named functions (no `_Debug` suffix needed - whole program is pedagogical)
   - Research/validation tool for algorithm differences

**Important**: Always use library functions for new features. Pedagogical code lives in `ephemeris.cpp`, not in production programs.

### Astronomical Calculation Philosophy

The sunset/sunrise/twilight programs aim to be **authoritative ephemeris calculators** using production-quality algorithms:
- Based on NOAA (National Oceanic and Atmospheric Administration) methods
- Implements Meeus (1991) *Astronomical Algorithms* formulas
- Uses USNO (U.S. Naval Observatory) standards where applicable
- High-precision calculations with nutation corrections
- Measured physical constants (e.g., solar radius from Mercury transits)
- Observer altitude support (extends sunset/sunrise by ~3-4 minutes per ~1600m elevation)

### Algorithm Selection for Research & Comparison

The library supports **multiple algorithm implementations** for research and validation purposes:

- **`enum class Algorithm { NOAA, USNO, LASKAR }`** - Generic algorithm selector for polynomial variants
- **NOAA** (default): Authoritative, widely adopted, "good enough" precision
- **USNO**: Linear approximations, simplest calculations
- **LASKAR**: Academic/research, highest precision (usually unnecessary)

**Design Philosophy:**
- **Default = NOAA**: Matches major online calculators, well-documented, appropriate precision
- **Academic variants**: Available for validation, comparison, and educational purposes
- **Arduino constraint**: Library must fit on Wemos D1 Mini (minimal dependencies)

Functions with algorithm variants:
- `meanLongitude(t, Algorithm algo = Algorithm::NOAA)`
- `meanAnomaly(t, Algorithm algo = Algorithm::NOAA)`
- `equationOfCenter(t, M, Algorithm algo = Algorithm::NOAA)`
- `obliquityOfEcliptic(T, Algorithm algo = Algorithm::NOAA)`

**Source-Specific Enums**: Some functions have implementations from **different authoritative sources** rather than just polynomial order variants. These use function-specific enums for semantic clarity:

- **`enum class LongitudeAscendingNodeFormulation { NOAA_LINEAR, REDA_ANDREAS_SPA }`**
  - `NOAA_LINEAR`: Simple linear approximation from NOAA (125.04 - 1934.136 * t)
  - `REDA_ANDREAS_SPA`: Cubic formula from Reda & Andreas (2008) NREL/TP-560-34302
  - Function: `longitudeAscendingNode(t, LongitudeAscendingNodeFormulation form = REDA_ANDREAS_SPA)`
  - **Now public API** for advanced users needing nutation calculations

The library API defaults to NOAA for production use. Pedagogical functions in sunset.cpp call the library with all three algorithms to show differences and support research/comparison workflows.

These are NOT simplified approximations - they implement the same algorithms used by professional astronomical software. Debug verbosity is educational, but the calculations themselves are authoritative.

## Special Notes

- Generic makefile linking all .o to all binaries is intentional
- Debug output in learning programs is by design
- Both sunset.cpp and sunset_calc library coexist to show different approaches

### Morning/Evening Commute Planning
- **sunrise** program displays dual-section table:
  - Morning arrival section: single commute time
  - Evening departure section: 2× commute + 8.5-hour workday (default)
- **Civil twilight reference**: Both sunrise and sunset programs use **civil twilight ending** (6° below horizon, ~20 min after sunset) as the practical "legally dark" threshold for commute planning, not astronomical sunset
- Status messages reference "getting back home by civil twilight" for practical relevance

### Shared Utilities
- Use `include/colors.h`, `include/solar_utils.h`, `include/format_utils.h`, `include/text_utils.h`, `include/table_layout.h` to avoid duplication across tables.
- Morning table: `include/morning_table.h` + `src/morning_table.cpp`
- Evening table: `include/twilight_table.h` + `src/twilight_table.cpp`

### UX Guidance
- Keep `twilight` evening-focused by default; use `sunrise` for morning. Add flags only if they help learning (e.g., `--morning`).

For detailed context, see `.ai/context.md`
