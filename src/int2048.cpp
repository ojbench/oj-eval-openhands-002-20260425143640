#include "include/int2048.h"
#include <algorithm>
#include <cassert>

namespace sjtu {

// Private constants
static const int BASE = 1000000000;  // 10^9
static const int BASE_DIGITS = 9;    // Number of digits per base element

// Helper function to remove leading zeros
static void remove_leading_zeros(std::vector<int>& digits) {
    while (!digits.empty() && digits.back() == 0) {
        digits.pop_back();
    }
}

// Helper function to compare absolute values
static int compare_absolute(const std::vector<int>& a, const std::vector<int>& b) {
    if (a.size() != b.size()) {
        return a.size() < b.size() ? -1 : 1;
    }
    for (int i = a.size() - 1; i >= 0; --i) {
        if (a[i] != b[i]) {
            return a[i] < b[i] ? -1 : 1;
        }
    }
    return 0;
}

// Helper function for absolute addition
static std::vector<int> add_absolute(const std::vector<int>& a, const std::vector<int>& b) {
    std::vector<int> result;
    int carry = 0;
    int max_size = std::max(a.size(), b.size());
    
    for (int i = 0; i < max_size || carry; ++i) {
        int sum = carry;
        if (i < a.size()) sum += a[i];
        if (i < b.size()) sum += b[i];
        result.push_back(sum % BASE);
        carry = sum / BASE;
    }
    
    return result;
}

// Helper function for absolute subtraction (assumes |a| >= |b|)
static std::vector<int> subtract_absolute(const std::vector<int>& a, const std::vector<int>& b) {
    std::vector<int> result;
    int borrow = 0;
    
    for (int i = 0; i < a.size(); ++i) {
        int diff = a[i] - borrow;
        if (i < b.size()) diff -= b[i];
        
        if (diff < 0) {
            diff += BASE;
            borrow = 1;
        } else {
            borrow = 0;
        }
        
        result.push_back(diff);
    }
    
    remove_leading_zeros(result);
    return result;
}

// Helper function for absolute multiplication
static std::vector<int> multiply_absolute(const std::vector<int>& a, const std::vector<int>& b) {
    if (a.empty() || b.empty()) {
        return std::vector<int>();
    }
    
    std::vector<int> result(a.size() + b.size(), 0);
    
    for (int i = 0; i < a.size(); ++i) {
        long long carry = 0;
        for (int j = 0; j < b.size() || carry; ++j) {
            long long product = result[i + j] + carry;
            if (j < b.size()) {
                product += (long long)a[i] * b[j];
            }
            result[i + j] = product % BASE;
            carry = product / BASE;
        }
    }
    
    remove_leading_zeros(result);
    return result;
}

// Helper function to compare absolute value with a base element
static bool compare_absolute_with_base(const std::vector<int>& a, int b) {
    if (a.empty()) return b == 0;
    if (a.size() > 1) return true;
    return a[0] >= b;
}

// Helper function for absolute division by single digit
static std::vector<int> divide_absolute_by_base(const std::vector<int>& a, int b, int& remainder) {
    std::vector<int> result;
    remainder = 0;
    
    for (int i = a.size() - 1; i >= 0; --i) {
        long long current = (long long)remainder * BASE + a[i];
        result.push_back(current / b);
        remainder = current % b;
    }
    
    std::reverse(result.begin(), result.end());
    remove_leading_zeros(result);
    return result;
}

// Helper function for absolute division using binary search
static std::vector<int> divide_absolute(const std::vector<int>& a, const std::vector<int>& b, std::vector<int>& remainder) {
    if (b.empty()) {
        // Division by zero - undefined behavior as per requirements
        return std::vector<int>();
    }
    
    if (a.size() < b.size() || (a.size() == b.size() && compare_absolute(a, b) < 0)) {
        remainder = a;
        return std::vector<int>();
    }
    
    // Binary search for quotient
    std::vector<int> low, high, mid, result;
    low.push_back(0);
    high = a;
    
    // Find upper bound by doubling
    while (true) {
        std::vector<int> product = multiply_absolute(high, b);
        if (compare_absolute(product, a) > 0) {
            break;
        }
        low = high;
        // Double high
        int carry = 0;
        for (int i = 0; i < high.size() || carry; ++i) {
            if (i < high.size()) {
                carry += high[i] * 2;
                high[i] = carry % BASE;
            } else {
                high.push_back(carry % BASE);
            }
            carry /= BASE;
        }
    }
    
    // Binary search
    while (compare_absolute(low, high) <= 0) {
        // mid = (low + high) / 2
        std::vector<int> sum = add_absolute(low, high);
        int rem;
        mid = divide_absolute_by_base(sum, 2, rem);
        
        std::vector<int> product = multiply_absolute(mid, b);
        int cmp = compare_absolute(product, a);
        
        if (cmp <= 0) {
            result = mid;
            low = add_absolute(mid, std::vector<int>(1, 1));
        } else {
            high = subtract_absolute(mid, std::vector<int>(1, 1));
        }
    }
    
    remainder = subtract_absolute(a, multiply_absolute(result, b));
    return result;
}

// Constructors
int2048::int2048() : sign(true) {}

int2048::int2048(long long value) : sign(value >= 0) {
    if (value == 0) {
        digits.clear();
        return;
    }
    
    unsigned long long abs_value = sign ? value : -value;
    while (abs_value > 0) {
        digits.push_back(abs_value % BASE);
        abs_value /= BASE;
    }
}

int2048::int2048(const std::string& str) {
    read(str);
}

int2048::int2048(const int2048& other) : sign(other.sign), digits(other.digits) {}

// Basic operations
void int2048::read(const std::string& str) {
    digits.clear();
    sign = true;
    
    int start = 0;
    if (str[0] == '-') {
        sign = false;
        start = 1;
    } else if (str[0] == '+') {
        start = 1;
    }
    
    // Process digits from right to left
    for (int i = str.length() - 1; i >= start; i -= BASE_DIGITS) {
        int block_start = std::max(start, i - BASE_DIGITS + 1);
        std::string block = str.substr(block_start, i - block_start + 1);
        digits.push_back(std::stoi(block));
    }
    
    remove_leading_zeros(digits);
    if (digits.empty()) {
        sign = true;  // Zero is always positive
    }
}

void int2048::print() {
    if (digits.empty()) {
        std::cout << "0";
        return;
    }
    
    if (!sign) {
        std::cout << "-";
    }
    
    std::cout << digits.back();
    for (int i = digits.size() - 2; i >= 0; --i) {
        std::cout.width(BASE_DIGITS);
        std::cout.fill('0');
        std::cout << digits[i];
    }
}

int2048& int2048::add(const int2048& other) {
    if (sign == other.sign) {
        // Same sign: add absolute values
        digits = add_absolute(digits, other.digits);
    } else {
        // Different signs: subtract smaller absolute value from larger
        int cmp = compare_absolute(digits, other.digits);
        if (cmp >= 0) {
            digits = subtract_absolute(digits, other.digits);
            // sign remains the same
        } else {
            digits = subtract_absolute(other.digits, digits);
            sign = !sign;
        }
    }
    
    if (digits.empty()) {
        sign = true;  // Zero is always positive
    }
    
    return *this;
}

int2048 add(int2048 a, const int2048& b) {
    return a.add(b);
}

int2048& int2048::minus(const int2048& other) {
    int2048 neg_other = other;
    neg_other.sign = !other.sign;
    return add(neg_other);
}

int2048 minus(int2048 a, const int2048& b) {
    return a.minus(b);
}

// Comparison operators
bool operator==(const int2048& a, const int2048& b) {
    if (a.sign != b.sign) {
        return false;
    }
    return compare_absolute(a.digits, b.digits) == 0;
}

bool operator!=(const int2048& a, const int2048& b) {
    return !(a == b);
}

bool operator<(const int2048& a, const int2048& b) {
    if (a.sign != b.sign) {
        return !a.sign && b.sign;  // negative < positive
    }
    if (!a.sign && !b.sign) {
        // Both negative: compare absolute values in reverse
        return compare_absolute(a.digits, b.digits) > 0;
    }
    // Both positive: compare absolute values normally
    return compare_absolute(a.digits, b.digits) < 0;
}

bool operator>(const int2048& a, const int2048& b) {
    return b < a;
}

bool operator<=(const int2048& a, const int2048& b) {
    return !(a > b);
}

bool operator>=(const int2048& a, const int2048& b) {
    return !(a < b);
}

// Unary operators
int2048 int2048::operator+() const {
    return *this;
}

int2048 int2048::operator-() const {
    int2048 result = *this;
    if (!result.digits.empty()) {
        result.sign = !result.sign;
    }
    return result;
}

// Assignment operator
int2048& int2048::operator=(const int2048& other) {
    if (this != &other) {
        sign = other.sign;
        digits = other.digits;
    }
    return *this;
}

// Compound assignment operators
int2048& int2048::operator+=(const int2048& other) {
    return add(other);
}

int2048& int2048::operator-=(const int2048& other) {
    return minus(other);
}

int2048& int2048::operator*=(const int2048& other) {
    if (digits.empty() || other.digits.empty()) {
        digits.clear();
        sign = true;
        return *this;
    }
    
    digits = multiply_absolute(digits, other.digits);
    sign = (sign == other.sign);
    
    if (digits.empty()) {
        sign = true;
    }
    
    return *this;
}

int2048& int2048::operator/=(const int2048& other) {
    if (other.digits.empty()) {
        // Division by zero - undefined behavior as per requirements
        return *this;
    }
    
    if (digits.empty()) {
        return *this;
    }
    
    std::vector<int> remainder;
    digits = divide_absolute(digits, other.digits, remainder);
    
    // Determine the sign of the result
    bool result_sign = (sign == other.sign);
    
    // Handle floor division for negative numbers
    if (!result_sign && !remainder.empty()) {
        // If result should be negative and there's a remainder, we need to add 1
        // to implement floor division (round toward negative infinity)
        // Example: -10 / 3 = -3.333... → floor = -4, so we need |quotient| + 1
        digits = add_absolute(digits, std::vector<int>(1, 1));
    }
    
    sign = result_sign;
    
    if (digits.empty()) {
        sign = true;
    }
    
    return *this;
}

int2048& int2048::operator%=(const int2048& other) {
    if (other.digits.empty()) {
        // Division by zero - undefined behavior as per requirements
        return *this;
    }
    
    if (digits.empty()) {
        return *this;
    }
    
    std::vector<int> quotient, remainder;
    quotient = divide_absolute(digits, other.digits, remainder);
    digits = remainder;
    
    // Handle modulo for negative numbers: x % y = x - (x / y) * y
    // Since we implemented floor division, this should work correctly
    if (digits.empty()) {
        sign = true;
    }
    
    return *this;
}

// Binary arithmetic operators
int2048 operator+(int2048 a, const int2048& b) {
    return a += b;
}

int2048 operator-(int2048 a, const int2048& b) {
    return a -= b;
}

int2048 operator*(int2048 a, const int2048& b) {
    return a *= b;
}

int2048 operator/(int2048 a, const int2048& b) {
    return a /= b;
}

int2048 operator%(int2048 a, const int2048& b) {
    return a %= b;
}

// Stream operators
std::istream& operator>>(std::istream& is, sjtu::int2048& num) {
    std::string str;
    is >> str;
    num.read(str);
    return is;
}

std::ostream& operator<<(std::ostream& os, const sjtu::int2048& num) {
    if (num.digits.empty()) {
        os << "0";
        return os;
    }
    
    if (!num.sign) {
        os << "-";
    }
    
    os << num.digits.back();
    for (int i = num.digits.size() - 2; i >= 0; --i) {
        os.width(9);
        os.fill('0');
        os << num.digits[i];
    }
    
    return os;
}

} // namespace sjtu
