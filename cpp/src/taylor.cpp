#include "../include/taylor.hpp"

#include <cmath>
#include <stdexcept>
#include <vector>

double evaluate_polynomial(const std::vector<double>& coefficients, double x, double center) {
    if (coefficients.empty()) {
        return 0.0;
    }

    const double u = x - center;
    const auto n = coefficients.size();
    double result = coefficients[n - 1];

    for (std::size_t i = n - 1; i > 0; --i) {
        result = result * u + coefficients[i - 1];
    }

    return result;
}

std::vector<double> taylor_coefficients(const std::string& function_name, double x0, int order) {
    if (order < 0) {
        throw std::invalid_argument("Taylor polynomial order must be non-negative");
    }

    std::vector<double> coeffs(static_cast<std::size_t>(order + 1), 0.0);

    if (function_name == "exp") {
        const double exp_x0 = std::exp(x0);
        coeffs[0] = exp_x0;
        for (int k = 1; k <= order; ++k) {
            coeffs[static_cast<std::size_t>(k)] = coeffs[static_cast<std::size_t>(k - 1)] / static_cast<double>(k);
        }
        return coeffs;
    }

    if (function_name == "sin") {
        const double s0 = std::sin(x0);
        const double c0 = std::cos(x0);
        double fact = 1.0;

        for (int k = 0; k <= order; ++k) {
            if (k > 0) {
                fact *= static_cast<double>(k);
            }
            double deriv = 0.0;
            switch (k % 4) {
                case 0: deriv = s0; break;
                case 1: deriv = c0; break;
                case 2: deriv = -s0; break;
                case 3: deriv = -c0; break;
            }
            coeffs[static_cast<std::size_t>(k)] = deriv / fact;
        }
        return coeffs;
    }

    if (function_name == "cos") {
        const double s0 = std::sin(x0);
        const double c0 = std::cos(x0);
        double fact = 1.0;

        for (int k = 0; k <= order; ++k) {
            if (k > 0) {
                fact *= static_cast<double>(k);
            }
            double deriv = 0.0;
            switch (k % 4) {
                case 0: deriv = c0; break;
                case 1: deriv = -s0; break;
                case 2: deriv = -c0; break;
                case 3: deriv = s0; break;
            }
            coeffs[static_cast<std::size_t>(k)] = deriv / fact;
        }
        return coeffs;
    }

    if (function_name == "ln") {
        if (x0 <= 0.0) {
            throw std::domain_error("Natural logarithm ln(x) requires expansion center x0 > 0");
        }
        coeffs[0] = std::log(x0);
        double x0_pow = x0;
        double sign = 1.0;
        for (int k = 1; k <= order; ++k) {
            coeffs[static_cast<std::size_t>(k)] = (sign) / (static_cast<double>(k) * x0_pow);
            x0_pow *= x0;
            sign = -sign;
        }
        return coeffs;
    }

    throw std::invalid_argument(
        "Unknown function: '" + function_name + "'. Supported: 'exp', 'sin', 'cos', 'ln'"
    );
}

TaylorResult approximate_taylor(const std::string& function_name, double x, double x0, int order) {
    auto coeffs = taylor_coefficients(function_name, x0, order);
    double val = evaluate_polynomial(coeffs, x, x0);
    return TaylorResult{val, order, x0, function_name};
}
