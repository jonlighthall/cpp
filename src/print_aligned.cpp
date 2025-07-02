#include <iostream>
#include <iomanip>
#include <vector>

void printAlignedNumbers(const std::vector<double>& numbers, const std::vector<int>& dp) {
    // Find the maximum width needed for the integer part
    int maxIntWidth = 0;
    for (double num : numbers) {
        int intPart = static_cast<int>(num);
        int width = std::to_string(intPart).length();
        if (width > maxIntWidth) maxIntWidth = width;
    }

    int maxDecimalPlaces = 5;

    auto formatNumber = [&](double value, int prec, int maxIntegerWidth, int maxDecimals) -> std::string {

        // int width = maxIntegerWidth + maxDecimals + 1; // +1 for the decimal point


        std::ostringstream oss;
        oss << std::fixed << std::setprecision(prec) << value;
        std::string numStr = oss.str();

        // Find position of decimal point
        size_t dotPos = numStr.find('.');

        // Calculate padding for integer part
        int intWidth = (dotPos != std::string::npos) ? dotPos : numStr.length();

        int padLeft = maxIntegerWidth - intWidth;

        return std::string(padLeft, ' ') + numStr;
    };

    // Print each number with aligned decimal points
    for (size_t i = 0; i < numbers.size(); ++i) {
        // // Convert number to string with required precision
        // std::ostringstream oss;
        // oss << std::fixed << std::setprecision(dp[i]) << numbers[i];
        // std::string numStr = oss.str();

        // // Find position of decimal point
        // size_t dotPos = numStr.find('.');

        // // Calculate padding for integer part
        // int intWidth = (dotPos != std::string::npos) ? dotPos : numStr.length();


        // int padLeft = maxIntWidth - intWidth;

        // // Print spaces for left padding, then the number
        // std::cout << std::string(padLeft, ' ') << numStr << std::endl;

        std::cout << formatNumber(numbers[i], dp[i], maxIntWidth, maxDecimalPlaces) << std::endl;
    }
}

int main() {
    std::vector<double> numbers = {123.456, 78.9, 0.12345, 4567.89};
    std::vector<int> dp = {3, 1, 5, 2};

    printAlignedNumbers(numbers, dp);

    return 0;
}