#ifndef SUMMARY_PRINTER_H
#define SUMMARY_PRINTER_H

#include "uband_diff.h"
#include <string>

/**
 * @class SummaryPrinter
 * @brief Handles all output formatting and printing operations
 *
 * This class is responsible for:
 * - Printing difference tables
 * - Generating summary reports
 * - Formatting numbers and colors
 * - Error reporting
 */
class SummaryPrinter {
public:
    SummaryPrinter(const Flags& flags, const CountStats& counter,
                  const DiffStats& differ, const Thresholds& thresh,
                  const PrintLevel& print);

    // Table printing
    void print_table(const ColumnValues& column_data, size_t column_index,
                    double line_threshold, double diff_rounded);
    std::string format_number(double value, int prec, int max_integer_width,
                             int max_decimals) const;

    // Error reporting
    void print_hard_threshold_error(double rounded1, double rounded2,
                                   double diff_rounded, size_t column_index) const;
    void print_format_info(const ColumnValues& column_data, size_t column_index) const;

    // Summary printing
    void print_summary(const std::string& file1, const std::string& file2,
                      int argc, char* argv[]) const;
    void print_settings(const std::string& file1, const std::string& file2) const;

    // Main summary sections
    void print_diff_like_summary(const SummaryParams& params) const;
    void print_rounded_summary(const SummaryParams& params) const;
    void print_significant_summary(const SummaryParams& params) const;

private:
    const Flags& flag;
    const CountStats& counter;
    const DiffStats& differ;
    const Thresholds& thresh;
    const PrintLevel& print;

    // Helper methods for summary printing
    void print_arguments_and_files(const std::string& file1, const std::string& file2,
                                  int argc, char* argv[]) const;
    void print_statistics(const std::string& file1) const;
    void print_flag_status() const;
    void print_counter_info() const;
    void print_detailed_summary(const SummaryParams& params) const;

    // Diff-like summary helpers
    void print_identical_files_message(const SummaryParams& params) const;
    void print_exact_matches_info(const SummaryParams& params) const;
    void print_non_zero_differences_info(const SummaryParams& params) const;
    void print_difference_counts(const SummaryParams& params) const;
    void print_maximum_difference_analysis() const;
    std::string get_count_color(size_t count) const;

    // Rounded summary helpers
    void print_equivalent_files_message(const SummaryParams& params) const;
    void print_rounded_summary_details(const SummaryParams& params) const;
    void print_trivial_differences_info(const SummaryParams& params) const;
    void print_non_trivial_differences_info(const SummaryParams& params) const;
    void print_files_different_message(const SummaryParams& params) const;
    std::string get_trivial_diff_color(size_t zero_diff) const;
    std::string get_non_trivial_diff_color() const;
    void print_maximum_rounded_difference() const;
    void print_rounded_difference_counts(const SummaryParams& params) const;
    void print_printed_differences_info(const SummaryParams& params) const;
    void print_not_printed_differences_info(const SummaryParams& params) const;
    void print_rounded_threshold_comparison() const;
    void print_max_diff_above_threshold() const;
    void print_max_diff_below_or_equal_threshold() const;
    void print_max_diff_equal_threshold() const;
    void print_max_diff_less_than_threshold() const;

    // Significant summary helpers
    void print_significant_differences_count(const SummaryParams& params) const;
    void print_significant_percentage() const;
    void print_insignificant_differences_count(const SummaryParams& params) const;
    void print_maximum_significant_difference_analysis() const;
    void print_maximum_significant_difference_details() const;
    void print_max_diff_threshold_comparison_above() const;
    void print_max_diff_threshold_comparison_below() const;
    void print_file_comparison_result(const SummaryParams& params) const;
    void print_significant_differences_printing_status(const SummaryParams& params) const;
    void print_count_with_percent(const SummaryParams& params, const std::string& label,
                                 size_t count, const std::string& color = "") const;

    // Utility helpers
    std::string format_boolean_status(bool value, bool showStatus, bool reversed,
                                     bool soft = false) const;
    void print_additional_diff_info(const SummaryParams& params) const;
    void print_critical_threshold_info() const;
};

#endif // SUMMARY_PRINTER_H
