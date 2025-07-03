#include <iomanip>
#include <iostream>
#include <vector>

void printAlignedNumbers(const std::vector<double>& numbers,
                         const std::vector<int>& dp) {
  // Find the maximum width needed for the integer part
  int maxIntWidth = 0;
  for (double num : numbers) {
    auto intPart = static_cast<int>(num);
    auto width = static_cast<int>(std::to_string(intPart).length());
    if (width > maxIntWidth) maxIntWidth = width;
  }

  int maxDecimalPlaces = 4;

  auto formatNumber = [&](double value, int prec, int maxIntegerWidth,
                          int maxDecimals) {
    // convert the number to a string with the specified precision
    std::ostringstream oss;

    int iprec = prec;  // Use the provided precision
    if (prec > maxDecimals) {
      iprec = maxDecimals;  // Ensure precision is within bounds
    }

    oss << std::fixed << std::setprecision(iprec) << value;
    std::string numStr = oss.str();

    // Find position of decimal point
    size_t dotPos = numStr.find('.');

    // Calculate padding for integer part
    int intWidth = (dotPos != std::string::npos) ? static_cast<int>(dotPos) : static_cast<int>(numStr.length());

    int padLeft = maxIntegerWidth - intWidth;

    int padRight = maxDecimals - iprec;
    if (padRight < 0) padRight = 0;  // Ensure no negative padding

    return std::string(padLeft, ' ') + numStr + std::string(padRight, ' ');
  };

  // Print each number with aligned decimal points
  for (size_t i = 0; i < numbers.size(); ++i) {
    std::cout << i + 1 << " | "
              << formatNumber(numbers[i], dp[i], maxIntWidth, maxDecimalPlaces)
              << " |" << std::endl;
  }
}

int main() {
  std::vector<double> numbers = {123.456, 78.9, 0.12345, 4567.89};
  std::vector<int> dp = {3, 1, 5, 2};

  printAlignedNumbers(numbers, dp);

  return 0;
}