#ifndef FORMAT_MANAGER_H
#define FORMAT_MANAGER_H

#include "uband_diff.h"
#include <vector>

/**
 * @class FormatManager
 * @brief Manages format validation and decimal place tracking
 *
 * This class is responsible for:
 * - Column count validation
 * - Decimal place format tracking
 * - Format change detection
 */
class FormatManager {
public:
    FormatManager(const Flags& flags, const PrintLevel& print);

    // Format validation
    bool validate_and_track_column_format(size_t n_col1, size_t n_col2,
                                         std::vector<int>& dp_per_col,
                                         size_t& prev_n_col);
    bool validate_decimal_column_size(const std::vector<int>& dp_per_col,
                                     size_t column_index) const;

    // Decimal places management
    bool initialize_decimal_place_format(int min_dp, size_t column_index,
                                        std::vector<int>& dp_per_col);
    bool update_decimal_place_format(int min_dp, size_t column_index,
                                    std::vector<int>& dp_per_col);

    // Threshold calculation
    double calculate_threshold(int decimal_places);

    // Line tracking
    void set_line_number(size_t line_number);
    size_t get_line_number() const;
    size_t get_column_number() const;
    size_t get_last_format_line() const;
    size_t get_line_column_count() const;

private:
    const Flags& flag;
    const PrintLevel& print;
    size_t this_fmt_line;
    size_t this_fmt_column;
    size_t last_fmt_line;
    size_t this_line_ncols;
};

#endif // FORMAT_MANAGER_H
