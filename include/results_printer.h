#ifndef RESULTS_PRINTER_H
#define RESULTS_PRINTER_H

#include <string>
#include <cstddef>

// Forward declarations
struct ColumnValues;
struct CountStats;
struct DiffStats;
struct Flags;
struct PrintLevel;
struct SummaryParams;
struct Thresholds;

/**
 * @brief Handles all output formatting and printing operations
 *
 * This class is responsible for:
 * - Formatting and printing difference tables
 * - Generating summary reports
 * - Managing output colors and formatting
 * - Printing statistics and debug information
 */
class ResultsPrinter {
public:
    ResultsPrinter(const PrintLevel& print_settings,
                   const Thresholds& thresholds);
    ~ResultsPrinter() = default;

    // Main printing interfaces
    void print_table(const ColumnValues& column_data,
                     size_t column_index,
                     double line_threshold,
                     double diff_rounded,
                     const CountStats& counter,
                     const DiffStats& differ,
                     Flags& flags) const;

    void print_summary(const std::string& file1, const std::string& file2,
                       int argc, char* argv[],
                       const CountStats& counter,
                       const DiffStats& differ,
                       const Flags& flags) const;

    void print_settings(const std::string& file1,
                        const std::string& file2) const;

    // Error and warning messages
    void print_hard_threshold_error(double rounded1, double rounded2,
                                    double diff_rounded, size_t column_index,
                                    const CountStats& counter,
                                    const Thresholds& thresh,
                                    Flags& flags) const;

    void print_format_info(const ColumnValues& column_data,
                           size_t column_index,
                           size_t line_number) const;

    // Number formatting utilities
    std::string format_number(double value, int precision,
                              int max_integer_width,
                              int max_decimals) const;

private:
    const PrintLevel& print;
    const Thresholds& thresh;

    // Summary helper functions
    void print_diff_like_summary(const SummaryParams& params,
                                 const CountStats& counter,
                                 const DiffStats& differ,
                                 const Flags& flags) const;

    void print_rounded_summary(const SummaryParams& params,
                               const CountStats& counter,
                               const DiffStats& differ,
                               const Flags& flags) const;

    void print_significant_summary(const SummaryParams& params,
                                   const CountStats& counter,
                                   const DiffStats& differ,
                                   const Flags& flags) const;

    // Utility functions for printing
    std::string get_count_color(size_t count, const CountStats& counter) const;
    std::string format_boolean_status(bool value, bool showStatus,
                                      bool reversed, bool soft = false) const;

    // Statistics and status printing
    void print_arguments_and_files(const std::string& file1,
                                   const std::string& file2,
                                   int argc, char* argv[]) const;

    void print_statistics(const std::string& file1,
                          const CountStats& counter,
                          const Flags& flags) const;

    void print_flag_status(const Flags& flags,
                           const CountStats& counter) const;

    void print_counter_info(const CountStats& counter) const;

    // Detailed summary helper functions
    void print_identical_files_message(const SummaryParams& params) const;
    void print_exact_matches_info(const SummaryParams& params,
                                  const CountStats& counter) const;
    void print_non_zero_differences_info(const SummaryParams& params,
                                         const CountStats& counter) const;
    void print_difference_counts(const SummaryParams& params,
                                 const CountStats& counter) const;
    void print_maximum_difference_analysis(const SummaryParams& params,
                                           const CountStats& counter,
                                           const DiffStats& differ) const;

    // Significant summary helper functions
    void print_significant_differences_count(const SummaryParams& params,
                                             const CountStats& counter) const;
    void print_significant_percentage(const CountStats& counter,
                                      Flags& flags) const;
    void print_insignificant_differences_count(const SummaryParams& params,
                                               const CountStats& counter) const;
    void print_maximum_significant_difference_analysis(const SummaryParams& params,
                                                       const DiffStats& differ) const;
    void print_maximum_significant_difference_details(const DiffStats& differ,
                                                      Flags& flags) const;
    void print_max_diff_threshold_comparison_above(const DiffStats& differ) const;
    void print_max_diff_threshold_comparison_below(const DiffStats& differ) const;
    void print_file_comparison_result(const SummaryParams& params,
                                      const DiffStats& differ) const;
    void print_significant_differences_printing_status(const SummaryParams& params,
                                                        const CountStats& counter) const;
    void print_count_with_percent(const SummaryParams& params,
                                  const std::string& label,
                                  size_t count,
                                  const CountStats& counter,
                                  const std::string& color = "") const;

    // File length utility (moved from FileReader for this specific use)
    size_t get_file_length(const std::string& file) const;
};

#endif // RESULTS_PRINTER_H
