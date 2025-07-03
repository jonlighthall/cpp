#include <iostream>
#include "math_ops.h"

int main() {
    int x = 10, y = 5;

    std::cout << "Addition: " << add(x, y) << std::endl;
    std::cout << "Subtraction: " << subtract(x, y) << std::endl;

    return 0;
}