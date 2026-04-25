

#include "src/include/int2048.h"
#include <iostream>

using namespace sjtu;

int main() {
    std::cout << "Debugging division..." << std::endl;
    
    int2048 a("-10");
    int2048 b("3");
    
    std::cout << "a = " << a << std::endl;
    std::cout << "b = " << b << std::endl;
    
    int2048 c = a / b;
    std::cout << "a / b = " << c << std::endl;
    
    // Test the expected result
    std::cout << "Expected: -4" << std::endl;
    
    // Test more cases
    int2048 d("10");
    int2048 e("-3");
    int2048 f = d / e;
    std::cout << "10 / -3 = " << f << " (expected: -4)" << std::endl;
    
    int2048 g("-10");
    int2048 h("-3");
    int2048 i = g / h;
    std::cout << "-10 / -3 = " << i << " (expected: 3)" << std::endl;
    
    return 0;
}

