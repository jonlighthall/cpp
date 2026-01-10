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
- **sunset.cpp**: Original program kept as full example (~910 lines with debug output)

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

**sunset.cpp**: Educational program with intentional debug verbosity
**sunset_calc**: Portable library (desktop + Arduino) - minimize dependencies
**hello/modular_test/print_aligned**: Keep simple - these are learning examples

## Special Notes

- Generic makefile linking all .o to all binaries is intentional
- Debug output in learning programs is by design
- Both sunset.cpp and sunset_calc library coexist to show different approaches

For detailed context, see `.ai/context.md`
