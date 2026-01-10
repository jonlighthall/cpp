# cpp

## Repository Status

This is a C++ learning repository containing educational examples and test programs.

### Graduated Programs
The following programs have been moved to dedicated repositories as they matured:
- **`uband_diff`** - Migrated to [diff_utils](https://github.com/jonlighthall/diff_utils) repository
  - Advanced numerical file comparison tool with extensive features
  - Includes column structure analysis, error accumulation analysis, and more
  - See [diff_utils/src/cpp](https://github.com/jonlighthall/diff_utils/tree/master/src/cpp)

### Current Programs
- **`sunset`** - Solar position and twilight calculator (evening focus)
- **`sunrise`** - Morning commute and dawn twilight planner
- **`twilight`** - Evening twilight event table (golden → astronomical)
- **`hello`** - Basic "Hello World" example
- **`modular_test`** - Testing/demonstration program
- **`print_aligned`** - Number alignment formatting example

### Reusable Libraries

#### Sunset Calculator Library
A portable astronomical calculation library extracted from the sunset program.
- **Files**: `include/sunset_calc.h`, `src/sunset_calc.cpp`
- **Purpose**: Calculate sunset/sunrise times for any location and date
- **Platforms**: Desktop C++, Arduino, embedded systems
- **Documentation**: See [docs/SUNSET_CALC_LIBRARY.md](docs/SUNSET_CALC_LIBRARY.md)
- **Quick Start**: See [examples/QUICK_REFERENCE.md](examples/QUICK_REFERENCE.md)
- **Example Sketches**: Arduino/Wemos examples in `examples/` directory

### Shared Utilities
To reduce duplication across `sunset`, `sunrise`, and `twilight`, several simple headers centralize formatting and math:
- `include/colors.h` – unified ANSI color palette for terminal tables (xterm-256)
- `include/solar_utils.h` – helpers for zenith conversion and hour-angle math
- `include/format_utils.h` – `hh:mm` and signed `±hh:mm` time formatting
- `include/text_utils.h` – string padding with degree symbol width handling
- `include/table_layout.h` – shared column widths for consistent tables

Rendering modules:
- `src/twilight_table.cpp` – evening table (events after sunset)
- `src/morning_table.cpp` – morning table (events before sunrise)

Design note: `twilight` defaults to evening events to keep the mental model simple. Use the `sunrise` program for morning golden hour and commute planning.

## WSL 1

EACCES: permission denied, rename home .vscode-server extensions ms-vscode cpptools linux

* try add polling, fail
* try chmod, fail
* try restarting

## Make

run `Makefile:Configure`
this will add settings to `.vscode/settings.json`
a launch target still needs to be set
run `Makefile:Set launch`... and select target

## SonarLint for VS Code

### Compilation Database

1. you will get the error:
   SonarLint is unable to analyze C and C++ files(s) because there is no configured compilation database
   * click on the button `Configure compile commands`
      * this will make the file `.vscode/settings.json`
      * then error
   * Ctrl-Shift-P SonarLint Configure the compilation...
3. No compilation databases were found in the workspace
   * link-> How to generate compile commands
4. Generate a Compilation Database
   * create a file named .vscode/compile_commands.json
   * settings
   * Makefile Tools
      add the line
      `"makefile.compileCommandsPath": ".vscode/compile_commands.json"`
      to `.vscode/settings.json`

### Node JS

* download latest package from <https://nodejs.org/en/download/>
  * Linux Binaries (x64)

* untar with `sudo tar -C /usr/local --strip-components=1 -xJf node-v20.11.1-linux-x64.tar.xz`
* then set execution directory to `/usr/local/bin/node`

### Connected Mode

#### Sonar Cloud

Ctrl-Shift P
SonarLint: Connect to SonarCloud
Configure

#### SonarQube

* install sonarqube
* run sonar console
* open webpage
