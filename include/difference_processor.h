#ifndef DIFFERENCE_PROCESSOR_H
#define DIFFERENCE_PROCESSOR_H

#include "uband_diff.h"

/**
 * @class DifferenceProcessor
 * @brief Processes numerical differences and tracks statistics
 *
 * This class is responsible for:
 * - Calculating differences between values
 * - Applying thresholds and rounding
 * - Tracking difference statistics
 * - Managing critical threshold detection
 */
class DifferenceProcessor {
public:
    DifferenceProcessor(const Flags& flags, CountStats& counter, DiffStats& differ,
                       const Thresholds& thresh, const PrintLevel& print);

    // Main processing methods
    bool process_difference(const ColumnValues& column_data, size_t column_index);

private:
    const Flags& flag;
    CountStats& counter;
    DiffStats& differ;
    const Thresholds& thresh;
    const PrintLevel& print;

    // Helper methods
    void process_raw_values(const ColumnValues& column_data);
    void process_rounded_values(const ColumnValues& column_data,
                               double rounded_diff, int minimum_deci);
    double round_to_decimal_places(double value, int decimal_places);
    bool is_within_ignore_threshold(double value) const;
    bool is_above_marginal_threshold(double value) const;
};

#endif // DIFFERENCE_PROCESSOR_H
