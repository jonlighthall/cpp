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
- **sunset** (~910 lines) - Solar position calculator; evening UI remains here
- **sunrise** (~200 lines) - Morning commute and dawn twilight planner
- **twilight** (~200 lines) - Evening twilight table renderer
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
**sunrise**: New CLI focuses on morning commute planning. Uses the library for sunrise, then prints a dawn table with golden hour start/end and twilight begin events.

**twilight**: Evening-only table shows sunset, golden hour end, and twilight end events with ETAs.

**Design Decision**: Keep morning and evening views separate for clarity. A future `--morning` flag in `twilight` is possible but not yet implemented.

### Repository Cleanup (January 2026)
**Removed**: 14 files related to uband_diff after confirming complete migration
**Updated**: README with "Repository Status" section documenting graduated programs

## AI Assistant Guidelines

### When Suggesting Code Changes

**DO**:
- Suggest improvements that enhance learning
- Explain why something works the way it does
- Offer alternatives with trade-offs explained
- Respect the educational nature of examples
- Consider Arduino/embedded portability for sunset_calc library
 - Reuse shared headers (colors/solar_utils/format/text/table_layout) for any new tables

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
