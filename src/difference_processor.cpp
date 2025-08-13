#include "../include/difference_processor.h"
#include "../include/uband_diff.h"
#include <iostream>
#include <cmath>
#include <algorithm>

DifferenceProcessor::DifferenceProcessor(const Flags& flags, CountStats& counter,
                                       DiffStats& differ, const Thresholds& thresh,
                                       const PrintLevel& print)
    : flag(flags), counter(counter), differ(differ), thresh(thresh), print(print) {}

bool DifferenceProcessor::process_difference(const ColumnValues& column_data,
                                           size_t column_index) {
    // Process raw (unrounded) values first
    process_raw_values(column_data);

    // Calculate rounded difference using minimum decimal places
    double rounded1 = round_to_decimal_places(column_data.value1, column_data.min_dp);
    double rounded2 = round_to_decimal_places(column_data.value2, column_data.min_dp);
    double rounded_diff = std::abs(rounded1 - rounded2);

    // Process rounded values
    process_rounded_values(column_data, rounded_diff, column_data.min_dp);

    // Check for critical threshold violation
    if (rounded_diff > thresh.critical) {
        counter.diff_critical++;
        const_cast<Flags&>(flag).has_critical_diff = true;
        const_cast<Flags&>(flag).error_found = true;

        if (print.debug) {
            std::cerr << "Critical difference found: " << rounded_diff
                      << " > " << thresh.critical << std::endl;
        }
        return false;  // Critical error
    }

    return true;
}

void DifferenceProcessor::process_raw_values(const ColumnValues& column_data) {
    double raw_diff = std::abs(column_data.value1 - column_data.value2);

    // Track maximum raw difference
    if (raw_diff > differ.max_non_zero) {
        differ.max_non_zero = raw_diff;
        differ.ndp_non_zero = std::max(column_data.dp1, column_data.dp2);
    }

    // Count non-zero differences (strict format-dependent comparison)
    if (raw_diff > 0.0 || column_data.dp1 != column_data.dp2) {
        counter.diff_non_zero++;
        const_cast<Flags&>(flag).has_non_zero_diff = true;
        const_cast<Flags&>(flag).files_are_same = false;
    }
}

void DifferenceProcessor::process_rounded_values(const ColumnValues& column_data,
                                                double rounded_diff,
                                                int minimum_deci) {
    // Calculate format-independent threshold
    double format_threshold = std::pow(10.0, -minimum_deci) / 2.0;

    // Check if difference exceeds format precision
    if (rounded_diff > format_threshold) {
        counter.diff_non_trivial++;
        const_cast<Flags&>(flag).has_non_trivial_diff = true;
        const_cast<Flags&>(flag).files_have_same_values = false;

        // Track maximum non-trivial difference
        if (rounded_diff > differ.max_non_trivial) {
            differ.max_non_trivial = rounded_diff;
            differ.ndp_non_trivial = minimum_deci;
        }
    }

    // Check user-defined significant threshold
    if (rounded_diff > thresh.significant) {
        counter.diff_significant++;
        const_cast<Flags&>(flag).has_significant_diff = true;
        const_cast<Flags&>(flag).files_are_close_enough = false;

        // Track maximum significant difference
        if (rounded_diff > differ.max_significant) {
            differ.max_significant = rounded_diff;
            differ.ndp_significant = minimum_deci;
        }
    }

    // Check marginal threshold
    if (rounded_diff > thresh.marginal) {
        counter.diff_marginal++;
        const_cast<Flags&>(flag).has_marginal_diff = true;
    }

    // Check print threshold
    if (rounded_diff > thresh.print) {
        counter.diff_print++;
        const_cast<Flags&>(flag).has_printed_diff = true;
    }

    // Track maximum difference overall
    if (rounded_diff > differ.max_significant) {
        differ.ndp_max = minimum_deci;
    }
}

double DifferenceProcessor::round_to_decimal_places(double value, int decimal_places) {
    if (decimal_places <= 0) {
        return std::round(value);
    }

    double multiplier = std::pow(10.0, decimal_places);
    return std::round(value * multiplier) / multiplier;
}

bool DifferenceProcessor::is_within_ignore_threshold(double value) const {
    return value < thresh.ignore;
}

bool DifferenceProcessor::is_above_marginal_threshold(double value) const {
    return value > thresh.marginal;
}
