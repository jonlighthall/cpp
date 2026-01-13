# AI Context - cpp Repository

> **Purpose**: This file provides context for AI assistants to better understand this repository's purpose, structure, and development patterns. Information here may not be obvious from code inspection alone.

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

## AI Assistant Guidelines

### When Suggesting Code Changes

**DO**:
- Suggest improvements that enhance learning
- Explain why something works the way it does
- Offer alternatives with trade-offs explained
- Respect the educational nature of examples
- Consider Arduino/embedded portability for sunset_calc library
 - Reuse shared headers (colors/solar_utils/format/text/table_layout) for any new tables
- Default to NOAA algorithms unless explicitly researching alternatives

**DON'T**:
- Suggest refactoring learning examples into production patterns
- Recommend complex abstractions for simple programs
- Propose moving all programs to classes/modules
- Suggest optimizations that obscure learning intent
- Recommend splitting hello.cpp into multiple files
- Push for professional software engineering practices in educational code
 - Merge evening and morning views unless a flag-based UI adds clear value

### When Creating New Code
- Match existing patterns in the program being modified
- Keep it simple unless complexity teaches something
- Add comments explaining "why" not just "what"
- Consider whether it belongs in this repo or should graduate

### When Analyzing Code
- Remember sunset.cpp has debug output by design
- Generic makefile linking is intentional, not a mistake
- Simple patterns in hello/modular_test are for learning
- sunset_calc library must remain Arduino-compatible (minimal dependencies)

### Terminology & Messaging Standards
**Twilight references:**
- Formal: "civil twilight ending" / "civil twilight beginning"
- Casual (status messages): "civil twilight ends" / "before dark"
- Consistent across programs: Both sunrise and sunset use civil twilight as practical reference

**Angle conventions:**
- Latitude: positive north, negative south (-90 to +90°)
- Longitude: positive east, negative west (-180 to +180°)
- Timezone: positive east of UTC (e.g., -5 for EST, +1 for CET)
- Sun angles: positive above horizon, negative below

**Status message style:**
- Production programs (sunrise/twilight): concise, practical
- Educational program (sunset): verbose with calculation steps

## Code Organization Patterns

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
└── tests/          # Test programs (if any)
```

Renderer modules:
- Morning: `src/morning_table.cpp` (API in `include/morning_table.h`)
- Evening: `src/twilight_table.cpp` (API in `include/twilight_table.h`)

### File Naming Conventions
- `main/*.cpp` - Executable programs
- `src/*.cpp` - Library implementations
- `include/*.h` - Public interfaces
- No .hpp distinction (keep it simple)

### Constants and Configuration
- `include/config.h` - Runtime configuration (location, debug levels, algorithm choice)
- `include/constants.h` - Astronomical/mathematical constants
- Namespace usage: `astro::`, `config::`, `sunset_calc::`

## Common Pitfalls for AI Assistants

### Misunderstandings to Avoid
1. **"This makefile is wrong"** - It's intentionally simple/bloated for learning
2. **"sunset.cpp should use the library"** - Keeping both shows two approaches
3. **"Too much debug output"** - Educational code benefits from verbosity
4. **"These should be classes"** - Not all learning code needs OOP
5. **"Missing error handling"** - Learning examples can be permissive
6. **"sunset_calc should use iostream"** - No! Must stay Arduino-compatible

### Context Confusion
- **cpp repo** = learning/experiments
- **diff_utils repo** = production code
- Don't suggest production patterns for cpp programs unless they're graduating
- Don't suggest cpp-style simplicity for diff_utils programs

## Testing and Validation

### Current Approach
- Manual testing with known good values
- Compare against NOAA solar calculator for sunset calculations
- Validated against USNO algorithms
- No formal test framework (learning repo)

### When to Suggest Tests
- For graduated programs moving to production
- For libraries intended for reuse (like sunset_calc)
- Not for simple examples (hello, print_aligned)

## Future Considerations

### Potential Graduates
Watch these programs - they may graduate if they grow significantly:
- **sunset** - Currently 910 lines; already has extracted library; main program may stay as example
- **None currently at risk** - Other programs are intentionally small examples

### Areas for Learning
The repository owner is learning:
- C++ standard library features
- Build systems (make, cmake potential future)
- Embedded development (Arduino)
- Astronomical algorithms
- Cross-platform development

## References and External Context

### Sunset Calculator Sources
- NOAA Solar Calculator: https://www.esrl.noaa.gov/gmd/grad/solcalc/
- USNO: https://aa.usno.navy.mil/faq/sun_approx
- James Still's article: https://squarewidget.com/solar-coordinates/
- Meeus, Jean (1991). Astronomical Algorithms

### Development Environment
- Primary: VS Code with C++ extensions
- Platform: Linux (WSL and native)
- Compiler: GCC/G++ with -Wall -g flags
- Git for version control

## Questions AI Assistants Should Ask

Before suggesting major changes, consider asking:
1. "Is this program intended to graduate to a production repo?"
2. "Should this remain simple for learning purposes?"
3. "Does this need to work on Arduino/embedded systems?"
4. "Is the current approach demonstrating a specific learning concept?"
5. "Would this change obscure the educational intent?"

## Version History Context

This context file created: January 10, 2026
Last major repo changes: January 10, 2026 (uband_diff cleanup, sunset_calc extraction)

---

**Note to AI Assistants**: This file is specifically for you! It contains context that experienced developers might infer but isn't explicit in code. When in doubt about suggestions, refer back to the "Repository Identity" and "Development Philosophy" sections.
