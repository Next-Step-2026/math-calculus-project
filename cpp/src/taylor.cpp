#include "taylor.hpp"
#include <cmath>
#include <functional>
#include <map>
#include <stdexcept>
#include <vector>

namespace taylor {

// --- Lógica de Cálculo (agora interna e usada pelo construtor) ---
namespace {

// Função auxiliar para cálculos trigonométricos.
void coeffs_trig(std::vector<double>& coeffs, const double derivs[4], std::size_t order) {
    double inv_fact = 1.0;
    for (std::size_t k = 0; k <= order; ++k) {
        if (k > 0) {
            inv_fact /= static_cast<double>(k);
        }
        coeffs[k] = derivs[k % 4] * inv_fact;
    }
}

void coeffs_exp(std::vector<double>& coeffs, double x0, std::size_t order) {
    double term = std::exp(x0);
    coeffs[0] = term;
    for (std::size_t k = 1; k <= order; ++k) {
        term /= static_cast<double>(k);
        coeffs[k] = term;
    }
}

void coeffs_sin(std::vector<double>& coeffs, double x0, std::size_t order) {
    const double s0 = std::sin(x0);
    const double c0 = std::cos(x0);
    const double derivs[4] = {s0, c0, -s0, -c0};
    coeffs_trig(coeffs, derivs, order);
}

void coeffs_cos(std::vector<double>& coeffs, double x0, std::size_t order) {
    const double s0 = std::sin(x0);
    const double c0 = std::cos(x0);
    const double derivs[4] = {c0, -s0, -c0, s0};
    coeffs_trig(coeffs, derivs, order);
}

void coeffs_ln(std::vector<double>& coeffs, double x0, std::size_t order) {
    if (x0 <= 0.0) {
        throw std::domain_error("ln(x) requires expansion center x0 > 0");
    }
    coeffs[0] = std::log(x0);
    if (order >= 1) {
        double term = 1.0;
        for (std::size_t k = 1; k <= order; ++k) {
            term *= (k > 1 ? -(k - 1) : 1.0) / x0;
            coeffs[k] = term / static_cast<double>(k);
        }
    }
}

} // namespace anônimo

// --- Implementação da Classe TaylorPolynomial ---

TaylorPolynomial::TaylorPolynomial(Function function_type, double x0, int order)
    : order_(order), center_(x0), function_type_(function_type) {
    if (order < 0) {
        throw std::invalid_argument("Taylor polynomial order must be non-negative");
    }

    const auto u_order = static_cast<std::size_t>(order);
    coefficients_.resize(u_order + 1);

    using CoeffsFunc = std::function<void(std::vector<double>&, double, std::size_t)>;
    static const std::map<Function, CoeffsFunc> func_map = {
        {Function::Exp, coeffs_exp},
        {Function::Sin, coeffs_sin},
        {Function::Cos, coeffs_cos},
        {Function::Ln,  coeffs_ln}
    };

    auto it = func_map.find(function_type);
    if (it != func_map.end()) {
        it->second(coefficients_, x0, u_order);
    } else {
        throw std::logic_error("Unknown function type provided.");
    }
}

double TaylorPolynomial::evaluate(double x) const {
    if (coefficients_.empty()) {
        return 0.0;
    }
    const double u = x - center_;
    double result = 0.0;
    // Método de Horner
    for (std::size_t i = coefficients_.size(); i > 0; --i) {
        result = result * u + coefficients_[i - 1];
    }
    return result;
}

// --- Implementação da Função Pública de Aproximação ---

TaylorResult approximate_taylor(Function function_type, double x, double x0, int order) {
    TaylorPolynomial poly(function_type, x0, order);
    double val = poly.evaluate(x);
    return TaylorResult{val, order, x0, function_type};
}

} // namespace taylor
