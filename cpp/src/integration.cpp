#include "../include/integration.hpp"

#include <stdexcept>


void validate_trapezoidal_parameters(const std::function<double(double)>& f, int n){
    if (!f) {
        throw std::invalid_argument("Integrand function cannot be null");
    }
    if (n <= 0) {
        throw std::invalid_argument("Number of intervals (n) must be strictly positive");
    }
}

void validate_simpson_parameters(const std::function<double(double)>& f, int n){
     if (!f) {
        throw std::invalid_argument("Integrand function cannot be null");
    }
    if (n <= 0) {
        throw std::invalid_argument("Number of intervals (n) must be strictly positive");
    }
    if (n % 2 != 0) {
        throw std::invalid_argument("Simpson's rule requires an even number of intervals (n % 2 == 0)");
    }
}

IntegrationResult integrate_simpson(const std::function<double(double)>& f, double a, double b, int n) {
   
    validate_simpson_parameters(f, n);

    if (a == b) {
        return IntegrationResult{0,n,"simpson"};
    }

    const double h = (b - a) / static_cast<double>(n);
    double sum = f(a) + f(b);

    for (int i = 1; i < n; ++i) {
        const double x = a + static_cast<double>(i) * h;
        if (i % 2 == 1) {
            sum += 4.0 * f(x);
        } else {
            sum += 2.0 * f(x);
        }
    }
    
    double val = (sum * h) / 3.0;
    return IntegrationResult{val,n,"simpson"};
}

IntegrationResult integrate_trapezoidal(const std::function<double(double)>& f, double a, double b, int n) {
    validate_trapezoidal_parameters(f, n);

    if (a == b) {
        return IntegrationResult{0,n,"trapezoidal"};
    }

    const double h = (b - a) / static_cast<double>(n);
    double sum = 0.5 * (f(a) + f(b));

    for (int i = 1; i < n; ++i) {
        const double x = a + static_cast<double>(i) * h;
        sum += f(x);
    }

    double val = sum * h;
    return IntegrationResult{val,n,"trapezoidal"};
}


