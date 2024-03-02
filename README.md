# cpp

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

1. you will get the error  SonarLint is unable to analyze C and C++ files(s) because there is no configured compilation database
   * button -> click on Configure compile commands
      * makes `.vscode/settings.json`
      * then error
   * Ctrl-Shift-P SonarLint Configure the compilation...
2. No compilation databases were found in the workspace
   * link-> How to generate compile commands
3. Generate a Compilation Database
   1. CMake
      settings
   2. Makefile Tools
      add the line
      `"makefile.compileCommandsPath": ".vscode/compile_commands.json"`
      to `.vscode/settings.json`
