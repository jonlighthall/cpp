/*
  ------------------------------------------------------------------------------
  Google C++ Style Guide
  IBM 80-column punch card format
  ..,....1....,....2....,....3....,....4....,....5....,....6....,....7....,....8
  345678901234567890123456789012345678901234567890123456789012345678901234567890
  ------------------------------------------------------------------------------
  Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor
  incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis
  nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo
  consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse
  cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non
  proident, sunt in culpa qui officia deserunt mollit anim id est laborum.
  ------------------------------------------------------------------------------
*/
#include <fstream>
#include <iostream>
#include <string>

int main() {
    std::ifstream file("readme.cc");
    if (!file) {
        std::cerr << "Failed to open file.\n";
        return 1;
    }
    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << '\n';
    }
    return 0;
}