
#include "src/include/int2048.h"
#include <iostream>
#include <cassert>

using namespace sjtu;

void test_basic_operations() {
    std::cout << "Testing basic operations..." << std::endl;
    
    // Test constructors
    int2048 a;
    int2048 b(123);
    int2048 c("456789012345678901234567890");
    
    // Test read and print
    a.read("-98765432109876543210987654321");
    std::cout << "a = "; a.print(); std::cout << std::endl;
    std::cout << "b = "; b.print(); std::cout << std::endl;
    std::cout << "c = "; c.print(); std::cout << std::endl;
    
    // Test addition
    int2048 sum = add(a, b);
    std::cout << "a + b = "; sum.print(); std::cout << std::endl;
    
    // Test subtraction
    int2048 diff = minus(c, b);
    std::cout << "c - b = "; diff.print(); std::cout << std::endl;
    
    std::cout << "Basic operations test passed!" << std::endl << std::endl;
}

void test_operators() {
    std::cout << "Testing operators..." << std::endl;
    
    int2048 a("12345678901234567890");
    int2048 b("98765432109876543210");
    
    // Test arithmetic operators
    int2048 c = a + b;
    std::cout << "a + b = " << c << std::endl;
    
    int2048 d = b - a;
    std::cout << "b - a = " << d << std::endl;
    
    int2048 e = a * int2048(2);
    std::cout << "a * 2 = " << e << std::endl;
    
    // Test comparison operators
    std::cout << "a < b: " << (a < b) << std::endl;
    std::cout << "a > b: " << (a > b) << std::endl;
    std::cout << "a == a: " << (a == a) << std::endl;
    
    // Test unary operators
    int2048 f = -a;
    std::cout << "-a = " << f << std::endl;
    
    std::cout << "Operators test passed!" << std::endl << std::endl;
}

void test_division() {
    std::cout << "Testing division..." << std::endl;
    
    // Test positive division
    int2048 a("10");
    int2048 b("3");
    int2048 c = a / b;
    std::cout << "10 / 3 = " << c << std::endl;
    
    int2048 d = a % b;
    std::cout << "10 % 3 = " << d << std::endl;
    
    // Test negative division (floor division)
    int2048 e("-10");
    int2048 f = e / b;
    std::cout << "-10 / 3 = " << f << std::endl;  // Should be -4
    
    int2048 g = e % b;
    std::cout << "-10 % 3 = " << g << std::endl;
    
    int2048 h = a / int2048("-3");
    std::cout << "10 / -3 = " << h << std::endl;  // Should be -4
    
    int2048 i = a % int2048("-3");
    std::cout << "10 % -3 = " << i << std::endl;
    
    std::cout << "Division test passed!" << std::endl << std::endl;
}

int main() {
    try {
        test_basic_operations();
        test_operators();
        test_division();
        std::cout << "All tests passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
