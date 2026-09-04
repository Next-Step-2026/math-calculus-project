#include "../include/maximum.hpp"
#include <cassert>
#include <cmath>

namespace mathlab {

double safe_maximum(double a, double b) {
    // Runtime Preconditions
    contracts::Expects(std::isfinite(a), "Parameter 'a' must be finite");
    contracts::Expects(std::isfinite(b), "Parameter 'b' must be finite");

    double result = (a > b) ? a : b;

    // Runtime Postconditions
    contracts::Ensures(result >= a, "Postcondition: result >= a");
    contracts::Ensures(result >= b, "Postcondition: result >= b");
    contracts::Ensures(result == a || result == b, "Postcondition: result in {a, b}");

    return result;
}

int safe_maximum(int a, int b) {
    int result = (a > b) ? a : b;

    contracts::Ensures(result >= a, "Postcondition: result >= a");
    contracts::Ensures(result >= b, "Postcondition: result >= b");
    contracts::Ensures(result == a || result == b, "Postcondition: result in {a, b}");

    return result;
}

double safe_maximum_bounded_array(const double* arr, int size) {
    // Preconditions
    contracts::Expects(arr != nullptr, "Array pointer cannot be null");
    contracts::Expects(size > 0 && size <= 10, "Size must be in range [1, 10]");

    double max_val = arr[0];
    contracts::Expects(std::isfinite(max_val), "Array elements must be finite");

    for (int i = 1; i < size; ++i) {
        contracts::Expects(std::isfinite(arr[i]), "Array elements must be finite");
        if (arr[i] > max_val) {
            max_val = arr[i];
        }
    }

    // Postconditions
    for (int i = 0; i < size; ++i) {
        contracts::Ensures(max_val >= arr[i], "Postcondition: max_val >= arr[i]");
    }

    return max_val;
}

} // namespace mathlab

// ============================================================================
// VERIFICATION HARNESS (ESBMC / CTest)
// Executable via:
//   cd cpp && esbmc src/maximum.cpp
// or via CMake CTest
// ============================================================================
int main() {
#if defined(__ESBMC__) || defined(__ESBMC)
    
    double a;
    double b;

    // Assumptions (Preconditions constraining symbolic search space)
    CONTRACT_ASSUME(std::isfinite(a) && a >= -1000.0 && a <= 1000.0);
    CONTRACT_ASSUME(std::isfinite(b) && b >= -1000.0 && b <= 1000.0);

    double max_res = mathlab::safe_maximum(a, b);

    // Formal Postcondition Assertions proved by SMT solver
    CONTRACT_ASSERT(max_res >= a);
    CONTRACT_ASSERT(max_res >= b);
    CONTRACT_ASSERT(max_res == a || max_res == b);

    double arr[3];
    CONTRACT_ASSUME(std::isfinite(arr[0]) && arr[0] >= -100.0 && arr[0] <= 100.0);
    CONTRACT_ASSUME(std::isfinite(arr[1]) && arr[1] >= -100.0 && arr[1] <= 100.0);
    CONTRACT_ASSUME(std::isfinite(arr[2]) && arr[2] >= -100.0 && arr[2] <= 100.0);

    double arr_max = mathlab::safe_maximum_bounded_array(arr, 3);

    CONTRACT_ASSERT(arr_max >= arr[0]);
    CONTRACT_ASSERT(arr_max >= arr[1]);
    CONTRACT_ASSERT(arr_max >= arr[2]);
    CONTRACT_ASSERT(arr_max == arr[0] || arr_max == arr[1] || arr_max == arr[2]);

#else
    // Scalar tests
    assert(mathlab::safe_maximum(2.0, 5.0) == 5.0);
    assert(mathlab::safe_maximum(10.0, 1.0) == 10.0);
    assert(mathlab::safe_maximum(-4.0, -1.0) == -1.0);
    assert(mathlab::safe_maximum(7.0, 7.0) == 7.0);

    assert(mathlab::safe_maximum(3, 8) == 8);
    assert(mathlab::safe_maximum(9, 2) == 9);

    // Array tests
    double test_arr[4] = { 1.5, -2.0, 8.75, 4.2 };
    assert(mathlab::safe_maximum_bounded_array(test_arr, 4) == 8.75);

    double single_elem[1] = { 42.0 };
    assert(mathlab::safe_maximum_bounded_array(single_elem, 1) == 42.0);

    // Precondition error verification
    bool caught = false;
    try {
        mathlab::safe_maximum_bounded_array(nullptr, 3);
    } catch (const contracts::PreconditionError&) {
        caught = true;
    }
    assert(caught);
#endif

    return 0;
}
