#include "../include/format_manager.h"
#include "../include/uband_diff.h"
#include <iostream>
#include <cmath>

FormatManager::FormatManager(const Flags& flags, const PrintLevel& print)
    : flag(flags), print(print), this_fmt_line(0), this_fmt_column(0),
      last_fmt_line(0), this_line_ncols(0) {}

bool FormatManager::validate_and_track_column_format(size_t n_col1, size_t n_col2,
                                                    std::vector<int>& dp_per_col,
                                                    size_t& prev_n_col) {
    // Check if both files have the same number of columns
    if (n_col1 != n_col2) {
        std::cerr << "Error: Line " << this_fmt_line
                  << " has different number of columns in each file:" << std::endl;
        std::cerr << "  File 1: " << n_col1 << " columns" << std::endl;
        std::cerr << "  File 2: " << n_col2 << " columns" << std::endl;
        return false;
    }

    this_line_ncols = n_col1;

    // Initialize or validate decimal places vector
    if (prev_n_col == 0) {
        // First line - initialize the vector
        dp_per_col.resize(this_line_ncols, -1);
        prev_n_col = this_line_ncols;
    } else if (this_line_ncols != prev_n_col) {
        // Number of columns changed
        if (print.debug) {
            std::cout << "Column count changed from " << prev_n_col
                      << " to " << this_line_ncols << " at line " << this_fmt_line << std::endl;
        }
        dp_per_col.resize(this_line_ncols, -1);
        prev_n_col = this_line_ncols;
    }

    return true;
}

bool FormatManager::validate_decimal_column_size(const std::vector<int>& dp_per_col,
                                                size_t column_index) const {
    if (column_index >= dp_per_col.size()) {
        std::cerr << "Error: Column index " << column_index
                  << " exceeds decimal places vector size " << dp_per_col.size() << std::endl;
        return false;
    }
    return true;
}

bool FormatManager::initialize_decimal_place_format(const int min_dp,
                                                   const size_t column_index,
                                                   std::vector<int>& dp_per_col) {
    if (!validate_decimal_column_size(dp_per_col, column_index)) {
        return false;
    }

    if (dp_per_col[column_index] == -1) {
        // First time seeing this column
        dp_per_col[column_index] = min_dp;
        if (print.debug2) {
            std::cout << "Initialized column " << column_index
                      << " decimal places to " << min_dp << std::endl;
        }
        return true;
    }
    return false;  // Already initialized
}

bool FormatManager::update_decimal_place_format(const int min_dp,
                                               const size_t column_index,
                                               std::vector<int>& dp_per_col) {
    if (!validate_decimal_column_size(dp_per_col, column_index)) {
        return false;
    }

    if (dp_per_col[column_index] != -1 && min_dp != dp_per_col[column_index]) {
        if (print.debug) {
            std::cout << "Format change detected at line " << this_fmt_line
                      << ", column " << column_index << ": "
                      << dp_per_col[column_index] << " -> " << min_dp << std::endl;
        }

        // Update format tracking
        this_fmt_column = column_index;
        last_fmt_line = this_fmt_line;

        // Update the decimal places for this column
        dp_per_col[column_index] = min_dp;
        return true;  // Format changed
    }
    return false;  // No change
}

double FormatManager::calculate_threshold(int decimal_places) {
    if (decimal_places <= 0) {
        return 1.0;  // Integer values
    }
    return std::pow(10.0, -decimal_places) / 2.0;
}

void FormatManager::set_line_number(size_t line_number) {
    this_fmt_line = line_number;
}

size_t FormatManager::get_line_number() const {
    return this_fmt_line;
}

size_t FormatManager::get_column_number() const {
    return this_fmt_column;
}

size_t FormatManager::get_last_format_line() const {
    return last_fmt_line;
}

size_t FormatManager::get_line_column_count() const {
    return this_line_ncols;
}
