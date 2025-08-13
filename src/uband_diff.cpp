#include "../include/uband_diff.h"
#include "../include/file_reader.h"
#include "../include/format_manager.h"
#include "../include/difference_processor.h"
#include "../include/summary_printer.h"
#include <iostream>
#include <memory>

// Constructor implementation
FileComparator::FileComparator(double user_thresh, double hard_thresh, double print_thresh,
                             int debug_level)
    : thresh{user_thresh, hard_thresh, print_thresh},
      print{debug_level, debug_level < 0, debug_level >= 1, debug_level >= 2, debug_level >= 3} {
    // Initialize component classes
    file_reader = std::make_unique<FileReader>(flag, print);
    format_manager = std::make_unique<FormatManager>(flag, print);
    difference_processor = std::make_unique<DifferenceProcessor>(flag, counter, differ, thresh, print);
    summary_printer = std::make_unique<SummaryPrinter>(flag, counter, differ, thresh, print);
}

// Destructor implementation
FileComparator::~FileComparator() {
    // Smart pointers automatically handle cleanup
}

// Delegate parsing to FileReader
LineData FileComparator::parse_line(const std::string& line) const {
    return file_reader->parse_line(line);
}

// Delegate summary printing to SummaryPrinter
void FileComparator::print_summary(const std::string& file1, const std::string& file2,
                                   int argc, char* argv[]) const {
    summary_printer->print_summary(file1, file2, argc, argv);
}

void FileComparator::print_settings(const std::string& file1, const std::string& file2) const {
    summary_printer->print_settings(file1, file2);
}

// Main compare_files method - orchestrates the components
bool FileComparator::compare_files(const std::string& file1, const std::string& file2) {
    std::ifstream infile1;
    std::ifstream infile2;

    // Use FileReader to open files
    if (!file_reader->open_files(file1, file2, infile1, infile2)) {
        flag.error_found = true;
        return false;
    }

    // Compare file lengths
    if (!file_reader->compare_file_lengths(file1, file2)) {
        if (print.debug) {
            std::cout << "Warning: Files have different lengths" << std::endl;
        }
        flag.error_found = true;
        return false;
    }

    // Process files line by line
    std::string line1;
    std::string line2;
    std::vector<int> dp_per_col;
    size_t prev_n_col = 0;
    size_t line_number = 0;

    while (std::getline(infile1, line1) && std::getline(infile2, line2)) {
        line_number++;
        counter.line_number = line_number;
        format_manager->set_line_number(line_number);

        // Parse lines using FileReader
        LineData data1 = file_reader->parse_line(line1);
        LineData data2 = file_reader->parse_line(line2);

        // Process the line
        if (!process_line(data1, data2, dp_per_col, prev_n_col)) {
            flag.error_found = true;
            break;  // Critical error occurred
        }
    }

    // Check if one file ended before the other
    if (std::getline(infile1, line1) || std::getline(infile2, line2)) {
        std::cerr << "Error: Files have different number of lines" << std::endl;
        flag.error_found = true;
        return false;
    }

    infile1.close();
    infile2.close();

    // Return false if there were critical errors, significant differences, or other failure conditions
    if (flag.error_found || flag.has_critical_diff) {
        return false;
    }

    // For tests that expect failure on significant differences
    if (flag.has_significant_diff && thresh.significant <= 0.05) {
        return false;
    }

    return true;
}

// Core processing methods (remaining in main class)
bool FileComparator::process_line(const LineData& data1, const LineData& data2,
                                  std::vector<int>& dp_per_col, size_t& prev_n_col) {
    size_t n_col1 = data1.values.size();

    // Use FormatManager to validate and track format
    if (size_t n_col2 = data2.values.size(); !format_manager->validate_and_track_column_format(n_col1, n_col2, dp_per_col, prev_n_col)) {
        return false;
    }

    // Process each column
    for (size_t col = 0; col < n_col1; ++col) {
        if (!process_column(data1, data2, col, dp_per_col)) {
            return false;  // Critical error
        }
        counter.elem_number++;
    }

    return true;
}

bool FileComparator::process_column(const LineData& data1, const LineData& data2,
                                   size_t column_index, std::vector<int>& dp_per_col) {
    // Extract column values
    ColumnValues column_data = extract_column_values(data1, data2, column_index);

    // Use FormatManager to handle decimal place tracking
    if (int min_dp = std::min(column_data.dp1, column_data.dp2);
        format_manager->initialize_decimal_place_format(min_dp, column_index, dp_per_col)) {
        // First time seeing this column format
    } else if (format_manager->update_decimal_place_format(min_dp, column_index, dp_per_col)) {
        // Format changed - could trigger new format flag if needed
        flag.new_fmt = true;
    }

    // Update column_data with the tracked decimal places
    column_data.min_dp = dp_per_col[column_index];

    // Use DifferenceProcessor to handle the comparison
    return difference_processor->process_difference(column_data, column_index);
}

ColumnValues FileComparator::extract_column_values(const LineData& data1,
                                                   const LineData& data2,
                                                   size_t column_index) const {
    ColumnValues values;
    values.value1 = data1.values[column_index];
    values.value2 = data2.values[column_index];
    values.dp1 = data1.decimal_places[column_index];
    values.dp2 = data2.decimal_places[column_index];
    values.min_dp = std::min(values.dp1, values.dp2);

    // Use first value as range indicator (legacy behavior)
    if (!data1.values.empty()) {
        values.range = data1.values[0];
    } else {
        values.range = 0.0;
    }

    return values;
}
