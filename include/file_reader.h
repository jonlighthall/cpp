#ifndef FILE_READER_H
#define FILE_READER_H

#include "uband_diff.h"
#include <fstream>
#include <string>
#include <vector>

/**
 * @class FileReader
 * @brief Handles file operations and line parsing for numerical file comparison
 *
 * This class is responsible for:
 * - Opening and reading files
 * - Parsing lines into numerical data
 * - File length validation
 */
class FileReader {
public:
    FileReader(const Flags& flags, const PrintLevel& print);

    // File operations
    bool open_files(const std::string& file1, const std::string& file2,
                   std::ifstream& infile1, std::ifstream& infile2) const;
    size_t get_file_length(const std::string& file) const;
    bool compare_file_lengths(const std::string& file1, const std::string& file2) const;

    // Line parsing
    LineData parse_line(const std::string& line) const;

private:
    const Flags& flag;
    const PrintLevel& print;

    // Helper methods
    int count_decimal_places(const std::string& number_str) const;
    bool parse_complex_number(const std::string& complex_str, LineData& data) const;
};

#endif // FILE_READER_H
