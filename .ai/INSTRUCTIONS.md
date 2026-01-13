# Instructions

**Purpose:** Procedures and standing orders for AI agents working on this project.

---

## Quick Start

1. Read this file for procedures
2. Read `CONTEXT.md` for project facts and decisions
3. Follow suggestion guidelines below

---

## Context Maintenance (Standing Order)

When the user provides substantial clarifying information, **integrate it into the appropriate `.ai/` file without being asked**.

### Where to put new information:

| Type of Information | Destination |
|---------------------|-------------|
| Project-wide decisions, facts, history | `CONTEXT.md` |
| Project-wide procedures, standing orders | `INSTRUCTIONS.md` |

### When to update:

**DO update when:**
- User provides ≥2-3 sentences of explanatory context
- User answers clarifying questions about the project
- User makes a decision that should persist across sessions
- User corrects a misconception (especially if AI-generated)

**DON'T update for:**
- Routine edits, minor corrections
- Conversational exchanges
- Information already documented

---

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

---

## Program Architecture

### Educational vs Production Split

Programs are split by **purpose**, not complexity:

1. **Educational/Research Program**
   - `ephemeris.cpp` - Algorithm comparison (NOAA/USNO/Laskar)
   - Full debug output showing every calculation step
   - Research tool for validating algorithm differences

2. **Production/Practical Programs**
   - `sunset.cpp` - Evening commute planner
   - `sunrise.cpp` - Morning/evening dual schedule
   - `twilight.cpp` - Evening twilight table
   - Use library defaults (NOAA), clean output

3. **Learning Examples**
   - `hello.cpp`, `modular_test.cpp`, `print_aligned.cpp`
   - Keep simple - these are teaching programs

### Dual Implementation Pattern

**Library** (`sunset_calc.h/.cpp`):
- Authoritative, production-quality calculations
- Arduino-compatible (no iostream)
- Functions: `getSunset()`, `getSunrise()`, `getZenith()`
- Algorithm selection via enums

**Educational** (`ephemeris.cpp`):
- Full walkthrough of calculations
- Extensive debug output
- Shows all algorithm variants for comparison
- Calls library functions for authoritative results

**Rule:** New features go in the library. Pedagogical code lives in `ephemeris.cpp`.

---

## Algorithm Defaults

**Default = NOAA** for all production code:
- Matches major online calculators
- Well-documented, appropriate precision
- Use for practical programs

**Academic variants** (USNO, LASKAR) for:
- Research/validation in ephemeris.cpp
- Educational comparison only

---

## Terminology Standards

### Twilight References
- Formal: "civil twilight ending" / "civil twilight beginning"
- Casual: "before dark" / "civil twilight ends"
- Both sunrise and sunset programs use civil twilight as practical reference

### Angle Conventions
- Latitude: positive north, negative south (-90 to +90°)
- Longitude: positive east, negative west (-180 to +180°)
- Timezone: positive east of UTC (e.g., -5 for EST)
- Sun angles: positive above horizon, negative below

---

## Quality Standards

### Before Editing:
1. Verify you have sufficient context
2. Check terminology against `CONTEXT.md`
3. Consider Arduino portability for library code

### After Editing:
1. Verify compilation succeeds
2. Update `CONTEXT.md` if you made decisions that should persist
3. Check for errors introduced

### When Uncertain:
- Ask clarifying questions before making changes
- Document assumptions in `CONTEXT.md`
- Prefer minimal changes over extensive rewrites

---

## What NOT To Do

### Code Suggestions
- **Don't** suggest refactoring learning examples into production patterns
- **Don't** recommend complex abstractions for simple programs
- **Don't** propose iostream for sunset_calc library
- **Don't** push professional patterns in educational code

### Meta-Content
- **No meta-commentary:** Don't create summary markdown files after edits
- **No hyper-literal headers:** Integrate naturally, don't create "Clarifying Statement" sections
- **No AI self-narration:** Don't describe what you're doing in documents

### Common Misunderstandings
1. "This makefile is wrong" - It's intentionally simple for learning
2. "sunset.cpp should use the library" - Both approaches coexist intentionally
3. "Too much debug output" - Educational code benefits from verbosity
4. "These should be classes" - Not all learning code needs OOP

---

## Shared Utilities

Reuse these headers for new table renderers:
- `include/colors.h` - ANSI color codes
- `include/solar_utils.h` - Solar calculation helpers
- `include/format_utils.h` - Output formatting
- `include/text_utils.h` - String utilities
- `include/table_layout.h` - Table rendering

Existing renderers:
- Morning: `src/morning_table.cpp` + `include/morning_table.h`
- Evening: `src/twilight_table.cpp` + `include/twilight_table.h`

---

## Questions to Ask

Before suggesting major changes:
1. "Is this program intended to graduate to a production repo?"
2. "Should this remain simple for learning purposes?"
3. "Does this need to work on Arduino/embedded?"
4. "Is the current approach demonstrating a specific concept?"
5. "Would this change obscure the educational intent?"

---

*For detailed project history and decisions, see `CONTEXT.md`.*
