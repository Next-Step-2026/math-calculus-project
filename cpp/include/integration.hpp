#pragma once

#include <functional>
#include <stdexcept>
#include <string>

// ============================================================================
// VALUE OBJECTS DE INTEGRAÇÃO
// ============================================================================

class IntegrationInterval {
    double a_;
    double b_;
public:
    IntegrationInterval(double a, double b) : a_(a), b_(b) {
        if (a > b) {
            throw std::invalid_argument("Lower limit (a) must be less than or equal to upper limit (b)");
        }
    }
    [[nodiscard]] double lower() const noexcept { return a_; }
    [[nodiscard]] double upper() const noexcept { return b_; }
    [[nodiscard]] double width() const noexcept { return b_ - a_; }
    [[nodiscard]] bool is_degenerate() const noexcept { return a_ == b_; }
    [[nodiscard]] double step_size(int n) const noexcept { return width() / static_cast<double>(n); }
};

class Subintervals {
    int value_;
public:
    explicit Subintervals(int n) : value_(n) {
        if (n <= 0) {
            throw std::invalid_argument("Number of intervals (n) must be strictly positive");
        }
    }
    [[nodiscard]] int value() const noexcept { return value_; }
    operator int() const noexcept { return value_; }
};

class SimpsonSubintervals {
    int value_;
public:
    explicit SimpsonSubintervals(int n) : value_(n) {
        if (n <= 0) {
            throw std::invalid_argument("Number of intervals (n) must be strictly positive");
        }
        if (n % 2 != 0) {
            throw std::invalid_argument("Simpson's rule requires an even number of intervals (n % 2 == 0)");
        }
    }
    [[nodiscard]] int value() const noexcept { return value_; }
    operator int() const noexcept { return value_; }
};

// Estrutura para encapsular o resultado da integração numérica
struct IntegrationResult {
    double value;
    int intervals;
    std::string method;
};

// Integração operando sobre entidades ricas de domínio:
IntegrationResult integrate_trapezoidal(
    const std::function<double(double)>& f,
    const IntegrationInterval& interval,
    Subintervals n
);

IntegrationResult integrate_simpson(
    const std::function<double(double)>& f,
    const IntegrationInterval& interval,
    SimpsonSubintervals n
);

// Forwarders de conveniência inline (preservam compatibilidade total de API):
inline IntegrationResult integrate_trapezoidal(
    const std::function<double(double)>& f,
    double a,
    double b,
    int n
) {
    return integrate_trapezoidal(f, IntegrationInterval(a, b), Subintervals(n));
}

inline IntegrationResult integrate_simpson(
    const std::function<double(double)>& f,
    double a,
    double b,
    int n
) {
    return integrate_simpson(f, IntegrationInterval(a, b), SimpsonSubintervals(n));
}
