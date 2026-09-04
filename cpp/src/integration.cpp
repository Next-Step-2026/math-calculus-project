#include "../include/integration.hpp"

#include <stdexcept>


namespace {

void validate_integrand(const std::function<double(double)>& f) {
    if (!f) {
        throw std::invalid_argument("Integrand function cannot be null");
    }
}

} // namespace

IntegrationResult integrate_simpson(
    const std::function<double(double)>& f,
    const IntegrationInterval& interval,
    SimpsonSubintervals n
) {
    validate_integrand(f);

    if (interval.is_degenerate()) {
        return IntegrationResult{0.0, n.value(), "simpson"};
    }

    const double h = interval.step_size(n.value());
    const double a = interval.lower();
    const double b = interval.upper();
    double sum = f(a) + f(b);

    for (int i = 1; i < n.value(); ++i) {
        const double x = a + static_cast<double>(i) * h;
        sum += (i % 2 == 1 ? 4.0 : 2.0) * f(x);
    }

    double val = (sum * h) / 3.0;
    return IntegrationResult{val, n.value(), "simpson"};
}

IntegrationResult integrate_trapezoidal(
    const std::function<double(double)>& f,
    const IntegrationInterval& interval,
    Subintervals n
) {
    validate_integrand(f);

    if (interval.is_degenerate()) {
        return IntegrationResult{0.0, n.value(), "trapezoidal"};
    }

    const double h = interval.step_size(n.value());
    const double a = interval.lower();
    const double b = interval.upper();
    double sum = 0.5 * (f(a) + f(b));

    for (int i = 1; i < n.value(); ++i) {
        const double x = a + static_cast<double>(i) * h;
        sum += f(x);
    }

    double val = sum * h;
    return IntegrationResult{val, n.value(), "trapezoidal"};
}