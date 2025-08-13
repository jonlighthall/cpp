#include "../include/summary_printer.h"
#include "../include/uband_diff.h"
#include <iostream>
#include <iomanip>

SummaryPrinter::SummaryPrinter(const Flags& flags, const CountStats& counter,
                              const DiffStats& differ, const Thresholds& thresh,
                              const PrintLevel& print)
    : flag(flags), counter(counter), differ(differ), thresh(thresh), print(print) {}

void SummaryPrinter::print_summary(const std::string& file1, const std::string& file2,
                                   int argc, char* argv[]) const {
    // This implementation will delegate to the existing comprehensive implementation
    // For now, we'll create a minimal version and move the full implementation later

    print_arguments_and_files(file1, file2, argc, argv);
    print_statistics(file1);

    if (print.debug) {
        print_flag_status();
        print_counter_info();
    }

    SummaryParams params{file1, file2, 12};  // Default format width
    print_detailed_summary(params);
}

void SummaryPrinter::print_settings(const std::string& file1, const std::string& file2) const {
    std::cout << "Settings for comparison:" << std::endl;
    std::cout << "  File 1: " << file1 << std::endl;
    std::cout << "  File 2: " << file2 << std::endl;
    std::cout << "  Significant threshold: " << thresh.significant << std::endl;
    std::cout << "  Critical threshold: " << thresh.critical << std::endl;
    std::cout << "  Print threshold: " << thresh.print << std::endl;
    std::cout << "  Debug level: " << print.level << std::endl;
}

// Minimal implementations that can be expanded later
void SummaryPrinter::print_arguments_and_files(const std::string& file1, const std::string& file2,
                                              int argc, char* argv[]) const {
    std::cout << "Comparing files: " << file1 << " and " << file2 << std::endl;
    if (print.debug && argc > 0) {
        std::cout << "Command line arguments: ";
        for (int i = 0; i < argc; ++i) {
            std::cout << argv[i] << " ";
        }
        std::cout << std::endl;
    }
}

void SummaryPrinter::print_statistics(const std::string& file1) const {
    std::cout << "Statistics:" << std::endl;
    std::cout << "  Lines processed: " << counter.line_number << std::endl;
    std::cout << "  Elements processed: " << counter.elem_number << std::endl;
}

void SummaryPrinter::print_flag_status() const {
    std::cout << "Flag Status:" << std::endl;
    std::cout << "  Files are same: " << format_boolean_status(flag.files_are_same, true, false) << std::endl;
    std::cout << "  Error found: " << format_boolean_status(flag.error_found, true, true) << std::endl;
}

void SummaryPrinter::print_counter_info() const {
    std::cout << "Counter Information:" << std::endl;
    std::cout << "  Non-zero differences: " << counter.diff_non_zero << std::endl;
    std::cout << "  Non-trivial differences: " << counter.diff_non_trivial << std::endl;
    std::cout << "  Significant differences: " << counter.diff_significant << std::endl;
}

void SummaryPrinter::print_detailed_summary(const SummaryParams& params) const {
    std::cout << "Detailed Summary:" << std::endl;

    if (flag.files_are_same) {
        std::cout << "Files are identical." << std::endl;
    } else if (flag.files_have_same_values) {
        std::cout << "Files have same values within precision." << std::endl;
    } else if (flag.files_are_close_enough) {
        std::cout << "Files are close enough within threshold." << std::endl;
    } else {
        std::cout << "Files have significant differences." << std::endl;
    }
}

std::string SummaryPrinter::format_boolean_status(bool value, bool showStatus, bool reversed,
                                                  bool soft) const {
    if (reversed) {
        value = !value;
    }

    if (value) {
        return soft ? "YES" : "TRUE";
    } else {
        return soft ? "NO" : "FALSE";
    }
}

// Placeholder implementations for table printing
void SummaryPrinter::print_table(const ColumnValues& column_data, size_t column_index,
                                 double line_threshold, double diff_rounded) {
    if (print.debug) {
        std::cout << "Table entry - Column " << column_index
                  << ": " << column_data.value1 << " vs " << column_data.value2
                  << " (diff: " << diff_rounded << ")" << std::endl;
    }
}

std::string SummaryPrinter::format_number(double value, int prec, int max_integer_width,
                                         int max_decimals) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(prec) << value;
    return oss.str();
}

void SummaryPrinter::print_hard_threshold_error(double rounded1, double rounded2,
                                               double diff_rounded, size_t column_index) const {
    std::cerr << "Critical threshold exceeded at column " << column_index
              << ": " << rounded1 << " vs " << rounded2
              << " (difference: " << diff_rounded << ")" << std::endl;
}

void SummaryPrinter::print_format_info(const ColumnValues& column_data, size_t column_index) const {
    if (print.debug) {
        std::cout << "Format info - Column " << column_index
                  << ": dp1=" << column_data.dp1 << ", dp2=" << column_data.dp2 << std::endl;
    }
}

// Placeholder implementations for the summary sections
void SummaryPrinter::print_diff_like_summary(const SummaryParams& params) const {
    std::cout << "Diff-like summary: " << params.file1 << " vs " << params.file2 << std::endl;
}

void SummaryPrinter::print_rounded_summary(const SummaryParams& params) const {
    std::cout << "Rounded summary: " << params.file1 << " vs " << params.file2 << std::endl;
}

void SummaryPrinter::print_significant_summary(const SummaryParams& params) const {
    std::cout << "Significant summary: " << params.file1 << " vs " << params.file2 << std::endl;
}
