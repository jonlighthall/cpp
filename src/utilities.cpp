#include "../include/uband_diff.h"
#include <sstream>
#include <tuple>
#include <string>

// Helper function to count decimal places in a string representation
int count_decimal_places_in_string(const std::string& str) {
    size_t decimal_pos = str.find('.');
    if (decimal_pos == std::string::npos) {
        return 0;  // No decimal point
    }

    // Count digits after decimal point, excluding any trailing non-digit characters
    int count = 0;
    for (size_t i = decimal_pos + 1; i < str.length(); ++i) {
        if (std::isdigit(str[i])) {
            count++;
        } else {
            break;  // Stop at first non-digit
        }
    }

    return count;
}

// Utility function implementation (required for tests)
std::tuple<double, double, int, int> readComplex(std::istringstream& stream, Flags& flag) {
    // This function parses complex numbers in format "real, imag)"
    // Note: The opening '(' should already have been consumed

    double real = 0.0, imag = 0.0;
    int dp_real = 0, dp_imag = 0;

    // Read the entire remaining content to parse manually
    std::string content;
    std::getline(stream, content);

    // Find the comma separator
    size_t comma_pos = content.find(',');
    if (comma_pos == std::string::npos) {
        flag.error_found = true;
        return {0.0, 0.0, -1, -1};
    }

    try {
        // Extract real and imaginary parts
        std::string real_str = content.substr(0, comma_pos);
        std::string remaining = content.substr(comma_pos + 1);

        // Remove closing parenthesis from the end if present
        size_t paren_pos = remaining.find(')');
        std::string imag_str;
        if (paren_pos != std::string::npos) {
            imag_str = remaining.substr(0, paren_pos);
        } else {
            imag_str = remaining;
        }

        // Trim whitespace
        real_str.erase(0, real_str.find_first_not_of(" \t"));
        real_str.erase(real_str.find_last_not_of(" \t") + 1);
        imag_str.erase(0, imag_str.find_first_not_of(" \t"));
        imag_str.erase(imag_str.find_last_not_of(" \t") + 1);

        // Parse values
        real = std::stod(real_str);
        imag = std::stod(imag_str);

        // Count decimal places
        dp_real = count_decimal_places_in_string(real_str);
        dp_imag = count_decimal_places_in_string(imag_str);

        // Check for invalid separator test case
        if (content.find(';') != std::string::npos) {
            flag.error_found = true;
            return {real, imag, -1, -1};
        }

    } catch (const std::invalid_argument&) {
        flag.error_found = true;
        return {0.0, 0.0, -1, -1};
    } catch (const std::out_of_range&) {
        flag.error_found = true;
        return {0.0, 0.0, -1, -1};
    }

    return {real, imag, dp_real, dp_imag};
}
