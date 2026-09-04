#include "taylor.hpp"

#include <array>
#include <boost/contract.hpp>
#include <cmath>
#include <stdexcept>
#include <vector>

namespace taylor {

// --- Lógica de Cálculo (agora interna e usada pelo construtor) ---
namespace {

using DerivativeSet = std::array<double, 4>;

void assert_finite_coefficients(const std::vector<double>& coeffs, std::size_t order) {
    BOOST_CONTRACT_ASSERT(coeffs.size() >= order + 1);
    for (std::size_t k = 0; k <= order; ++k) {
        BOOST_CONTRACT_ASSERT(std::isfinite(coeffs[k]));
    }
}

// Função auxiliar para cálculos trigonométricos.
void coeffs_trig(std::vector<double>& coeffs, const DerivativeSet& derivs, std::size_t order) {
    boost::contract::check c = boost::contract::function()
        .precondition([&] {
            BOOST_CONTRACT_ASSERT(coeffs.size() >= order + 1);
            for (std::size_t i = 0; i < derivs.size(); ++i) {
                BOOST_CONTRACT_ASSERT(std::isfinite(derivs[i]));
            }
        })
        .postcondition([&] {
            assert_finite_coefficients(coeffs, order);
        });

    double inv_fact = 1.0;
    for (std::size_t k = 0; k <= order; ++k) {
        if (k > 0) {
            inv_fact /= static_cast<double>(k);
        }
        coeffs[k] = derivs[k % derivs.size()] * inv_fact;
    }
}

void coeffs_trig_from_derivatives(std::vector<double>& coeffs, double x0, std::size_t order,
                                 const DerivativeSet& derivs) {
    boost::contract::check c = boost::contract::function()
        .precondition([&] {
            BOOST_CONTRACT_ASSERT(coeffs.size() >= order + 1);
            BOOST_CONTRACT_ASSERT(std::isfinite(x0));
            for (std::size_t i = 0; i < derivs.size(); ++i) {
                BOOST_CONTRACT_ASSERT(std::isfinite(derivs[i]));
            }
        })
        .postcondition([&] {
            assert_finite_coefficients(coeffs, order);
        });

    coeffs_trig(coeffs, derivs, order);
}

void coeffs_exp(std::vector<double>& coeffs, double x0, std::size_t order) {
    boost::contract::check c = boost::contract::function()
        .precondition([&] {
            BOOST_CONTRACT_ASSERT(coeffs.size() >= order + 1);
            BOOST_CONTRACT_ASSERT(std::isfinite(x0));
        })
        .postcondition([&] {
            BOOST_CONTRACT_ASSERT(coeffs[0] > 0.0);
            for (std::size_t k = 0; k <= order; ++k) {
                BOOST_CONTRACT_ASSERT(std::isfinite(coeffs[k]));
            }
        });

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
    const DerivativeSet derivs = {s0, c0, -s0, -c0};
    coeffs_trig_from_derivatives(coeffs, x0, order, derivs);
}

void coeffs_cos(std::vector<double>& coeffs, double x0, std::size_t order) {
    const double s0 = std::sin(x0);
    const double c0 = std::cos(x0);
    const DerivativeSet derivs = {c0, -s0, -c0, s0};
    coeffs_trig_from_derivatives(coeffs, x0, order, derivs);
}

void coeffs_ln(std::vector<double>& coeffs, double x0, std::size_t order) {
    boost::contract::check c = boost::contract::function()
        .precondition([&] {
            BOOST_CONTRACT_ASSERT(coeffs.size() >= order + 1);
            BOOST_CONTRACT_ASSERT(std::isfinite(x0));
            BOOST_CONTRACT_ASSERT(x0 > 0.0);
        })
        .postcondition([&] {
            assert_finite_coefficients(coeffs, order);
        });

    if (x0 <= 0.0) {
        throw std::domain_error("ln(x) requires expansion center x0 > 0");
    }
    coeffs[0] = std::log(x0);
    double inv_x0_pow = 1.0 / x0;
    double sign = 1.0;
    for (std::size_t k = 1; k <= order; ++k) {
        coeffs[k] = (sign * inv_x0_pow) / static_cast<double>(k);
        inv_x0_pow /= x0;
        sign = -sign;
    }
}

void build_coefficients(Function function_type, std::vector<double>& coeffs, double x0, std::size_t order) {
    switch (function_type) {
        case Function::Exp:
            coeffs_exp(coeffs, x0, order);
            return;
        case Function::Sin:
            coeffs_sin(coeffs, x0, order);
            return;
        case Function::Cos:
            coeffs_cos(coeffs, x0, order);
            return;
        case Function::Ln:
            coeffs_ln(coeffs, x0, order);
            return;
    }

    throw std::logic_error("Unknown function type provided.");
}

} // namespace anônimo

// --- Implementação da Classe TaylorPolynomial ---

TaylorPolynomial::TaylorPolynomial(Function function_type, TaylorCenter x0, TaylorOrder order)
    : order_(order), center_(x0), function_type_(function_type) {
    boost::contract::check c = boost::contract::function()
        .precondition([&] {
            BOOST_CONTRACT_ASSERT(std::isfinite(x0.value()));
            BOOST_CONTRACT_ASSERT(order.value() >= 0);
            BOOST_CONTRACT_ASSERT(order.value() <= 20);
            BOOST_CONTRACT_ASSERT(function_type != Function::Ln || x0.value() > 0.0);
        })
        .postcondition([&] {
            BOOST_CONTRACT_ASSERT(order_ == order);
            BOOST_CONTRACT_ASSERT(center_ == x0);
            BOOST_CONTRACT_ASSERT(function_type_ == function_type);
            BOOST_CONTRACT_ASSERT(coefficients_.size() == static_cast<std::size_t>(order.value() + 1));
            for (double coeff : coefficients_) {
                BOOST_CONTRACT_ASSERT(std::isfinite(coeff));
            }
        });

    const auto u_order = static_cast<std::size_t>(order.value());
    coefficients_.resize(u_order + 1);
    build_coefficients(function_type, coefficients_, x0.value(), u_order);
}

TaylorPolynomial::TaylorPolynomial(Function function_type, double x0, TaylorOrder order)
    : TaylorPolynomial(function_type, TaylorCenter(x0), order) {}

TaylorPolynomial::TaylorPolynomial(Function function_type, double x0, int order)
    : TaylorPolynomial(function_type, TaylorCenter(x0), TaylorOrder(order)) {}


double evaluate_polynomial(const std::vector<double>& coefficients, double x, double center) {
    double result = 0.0;

    boost::contract::check c = boost::contract::function()
        .precondition([&] {
            BOOST_CONTRACT_ASSERT(std::isfinite(x));
            BOOST_CONTRACT_ASSERT(std::isfinite(center));
            for (double coeff : coefficients) {
                BOOST_CONTRACT_ASSERT(std::isfinite(coeff));
            }
        })
        .postcondition([&] {
            BOOST_CONTRACT_ASSERT(std::isfinite(result));
            BOOST_CONTRACT_ASSERT(!coefficients.empty() || result == 0.0);
            BOOST_CONTRACT_ASSERT(coefficients.empty() || x != center || result == coefficients[0]);
        });

    if (coefficients.empty()) {
        result = 0.0;
        return result;
    }
    const double u = x - center;
    result = 0.0;
    // Algoritmo de Horner com FMA para reduzir arredondamento intermediário
    // e explorar a instrução de máquina vfmadd em CPUs modernos.
    for (std::size_t i = coefficients.size(); i > 0; --i) {
        result = std::fma(result, u, coefficients[i - 1]);
    }
    return result;
}

double TaylorPolynomial::evaluate(double x) const {
    double result = 0.0;

    boost::contract::check c = boost::contract::function()
        .precondition([&] {
            BOOST_CONTRACT_ASSERT(std::isfinite(x));
            BOOST_CONTRACT_ASSERT(function_type_ != Function::Ln || x > 0.0);
        })
        .postcondition([&] {
            BOOST_CONTRACT_ASSERT(std::isfinite(result));
            BOOST_CONTRACT_ASSERT(coefficients_.empty() || x != center_.value() || result == coefficients_[0]);
        });

    result = evaluate_polynomial(coefficients_, x, center_.value());
    return result;
}

// --- Implementação da Função Pública de Aproximação ---

TaylorResult approximate_taylor(Function function_type, double x, TaylorCenter x0, TaylorOrder order) {
    TaylorResult result{0.0, order, x0, function_type};

    boost::contract::check c = boost::contract::function()
        .precondition([&] {
            BOOST_CONTRACT_ASSERT(std::isfinite(x));
            BOOST_CONTRACT_ASSERT(std::isfinite(x0.value()));
            BOOST_CONTRACT_ASSERT(order.value() >= 0);
            BOOST_CONTRACT_ASSERT(order.value() <= 20);
            BOOST_CONTRACT_ASSERT(function_type != Function::Ln || (x > 0.0 && x0.value() > 0.0));
        })
        .postcondition([&] {
            BOOST_CONTRACT_ASSERT(result.order == order);
            BOOST_CONTRACT_ASSERT(result.center == x0);
            BOOST_CONTRACT_ASSERT(result.function_type == function_type);
            BOOST_CONTRACT_ASSERT(std::isfinite(result.value));
        });

    TaylorPolynomial poly(function_type, x0, order);
    result.value = poly.evaluate(x);
    return result;
}

TaylorResult approximate_taylor(Function function_type, double x, double x0, TaylorOrder order) {
    return approximate_taylor(function_type, x, TaylorCenter(x0), order);
}

} // namespace taylor

