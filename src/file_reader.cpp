#include "../include/file_reader.h"
#include "../include/uband_diff.h"
#include <iostream>
#include <sstream>

FileReader::FileReader(const Flags& flags, const PrintLevel& print)
    : flag(flags), print(print) {}

bool FileReader::open_files(const std::string& file1, const std::string& file2,
                           std::ifstream& infile1, std::ifstream& infile2) const {
    infile1.open(file1);
    if (!infile1.is_open()) {
        std::cerr << "Error: Could not open file " << file1 << std::endl;
        return false;
    }

    infile2.open(file2);
    if (!infile2.is_open()) {
        std::cerr << "Error: Could not open file " << file2 << std::endl;
        infile1.close();
        return false;
    }

    return true;
}

size_t FileReader::get_file_length(const std::string& file) const {
    std::ifstream infile(file);
    if (!infile.is_open()) {
        std::cerr << "Error: Could not open file " << file << " to get length" << std::endl;
        return 0;
    }

    size_t line_count = 0;
    std::string line;
    while (std::getline(infile, line)) {
        line_count++;
    }

    infile.close();
    return line_count;
}

bool FileReader::compare_file_lengths(const std::string& file1,
                                     const std::string& file2) const {
    size_t length1 = get_file_length(file1);

    if (size_t length2 = get_file_length(file2); length1 != length2) {
        std::cerr << "Warning: Files have different number of lines:" << std::endl;
        std::cerr << "  " << file1 << ": " << length1 << " lines" << std::endl;
        std::cerr << "  " << file2 << ": " << length2 << " lines" << std::endl;
        return false;
    }

    return true;
}

LineData FileReader::parse_line(const std::string& line) const {
    LineData data;
    std::istringstream stream(line);

    // Parse the line to extract numbers and their decimal places
    std::string token;
    while (stream >> token) {
        if (!token.empty()) {
            // Check if this is a complex number starting with '('
            if (token[0] == '(' || (token.find('(') != std::string::npos)) {
                // Handle complex number parsing
                std::string complex_str = token;

                // If the token doesn't end with ')', read more tokens until we find the closing paren
                while (complex_str.find(')') == std::string::npos && stream >> token) {
                    complex_str += " " + token;
                }

                // Parse the complex number
                if (parse_complex_number(complex_str, data)) {
                    continue; // Successfully parsed complex number
                }
            }

            // Try to parse as regular number
            try {
                double value = std::stod(token);
                int decimal_places = count_decimal_places(token);

                data.values.push_back(value);
                data.decimal_places.push_back(decimal_places);
            } catch (const std::invalid_argument&) {
                if (print.debug) {
                    std::cerr << "Warning: Could not parse token '" << token
                              << "' as number" << std::endl;
                }
            }
        }
    }

    return data;
}

int FileReader::count_decimal_places(const std::string& number_str) const {
    size_t decimal_pos = number_str.find('.');
    if (decimal_pos == std::string::npos) {
        return 0;  // No decimal point found
    }

    // Count digits after decimal point, ignoring scientific notation
    size_t e_pos = number_str.find_first_of("eE", decimal_pos);
    if (e_pos != std::string::npos) {
        return static_cast<int>(e_pos - decimal_pos - 1);
    } else {
        return static_cast<int>(number_str.length() - decimal_pos - 1);
    }
}

bool FileReader::parse_complex_number(const std::string& complex_str, LineData& data) const {
    // Parse complex number in format "(real, imag)"
    size_t start_paren = complex_str.find('(');
    size_t end_paren = complex_str.find(')', start_paren);

    if (start_paren == std::string::npos || end_paren == std::string::npos) {
        return false; // Invalid format
    }

    // Extract the content between parentheses
    std::string content = complex_str.substr(start_paren + 1, end_paren - start_paren - 1);

    // Find the comma separator
    size_t comma_pos = content.find(',');
    if (comma_pos == std::string::npos) {
        return false; // No comma found
    }

    try {
        // Extract real and imaginary parts
        std::string real_str = content.substr(0, comma_pos);
        std::string imag_str = content.substr(comma_pos + 1);

        // Trim whitespace
        real_str.erase(0, real_str.find_first_not_of(" \t"));
        real_str.erase(real_str.find_last_not_of(" \t") + 1);
        imag_str.erase(0, imag_str.find_first_not_of(" \t"));
        imag_str.erase(imag_str.find_last_not_of(" \t") + 1);

        // Parse values
        double real_val = std::stod(real_str);
        double imag_val = std::stod(imag_str);

        // Count decimal places
        int real_dp = count_decimal_places(real_str);
        int imag_dp = count_decimal_places(imag_str);

        // Add to data
        data.values.push_back(real_val);
        data.values.push_back(imag_val);
        data.decimal_places.push_back(real_dp);
        data.decimal_places.push_back(imag_dp);

        return true;

    } catch (const std::invalid_argument& e) {
        if (print.debug) {
            std::cerr << "Warning: Could not parse complex number '" << complex_str
                      << "' - " << e.what() << std::endl;
        }
        return false;
    }
}
