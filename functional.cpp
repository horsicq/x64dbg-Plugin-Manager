#include <iostream>

// Recursive function to calculate the factorial of a number
unsigned long long factorial(int n) {
    if (n == 0) {
        return 1;
    }
    return n * factorial(n - 1);
}

int main() {
    int num;

    std::cout << "Enter a positive integer: ";
    std::cin >> num;

    if (num < 0) {
        std::cout << "Factorial is not defined for negative numbers." << std::endl;
    } else {
        unsigned long long result = factorial(num);
        std::cout << "Factorial of " << num << " is: " << result << std::endl;
    }

    return 0;
}
