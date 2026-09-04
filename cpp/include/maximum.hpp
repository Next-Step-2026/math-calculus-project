#pragma once

#include "contracts.hpp"

namespace mathlab {

/**
 * @brief Computes maximum of two double values under contract.
 */
double safe_maximum(double a, double b);

/**
 * @brief Computes maximum of two int values under contract.
 */
int safe_maximum(int a, int b);

/**
 * @brief Computes maximum over a bounded contiguous array.
 */
double safe_maximum_bounded_array(const double* arr, int size);

} // namespace mathlab
