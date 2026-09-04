#include "taylor.hpp"

#include <boost/contract.hpp>
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
    boost::contract::check c = boost::contract::function()
        .precondition([&] {
            BOOST_CONTRACT_ASSERT(coeffs.size() >= order + 1);
            BOOST_CONTRACT_ASSERT(derivs != nullptr);
            BOOST_CONTRACT_ASSERT(std::isfinite(derivs[0]));
            BOOST_CONTRACT_ASSERT(std::isfinite(derivs[1]));
            BOOST_CONTRACT_ASSERT(std::isfinite(derivs[2]));
            BOOST_CONTRACT_ASSERT(std::isfinite(derivs[3]));
        })
        .postcondition([&] {
            for (std::size_t k = 0; k <= order; ++k) {
                BOOST_CONTRACT_ASSERT(std::isfinite(coeffs[k]));
            }
        });

    double inv_fact = 1.0;
    for (std::size_t k = 0; k <= order; ++k) {
        if (k > 0) {
            inv_fact /= static_cast<double>(k);
        }
        coeffs[k] = derivs[k % 4] * inv_fact;
    }
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
    boost::contract::check c = boost::contract::function()
        .precondition([&] {
            BOOST_CONTRACT_ASSERT(coeffs.size() >= order + 1);
            BOOST_CONTRACT_ASSERT(std::isfinite(x0));
        })
        .postcondition([&] {
            for (std::size_t k = 0; k <= order; ++k) {
                BOOST_CONTRACT_ASSERT(std::isfinite(coeffs[k]));
            }
        });

    const double s0 = std::sin(x0);
    const double c0 = std::cos(x0);
    const double derivs[4] = {s0, c0, -s0, -c0};
    coeffs_trig(coeffs, derivs, order);
}

void coeffs_cos(std::vector<double>& coeffs, double x0, std::size_t order) {
    boost::contract::check c = boost::contract::function()
        .precondition([&] {
            BOOST_CONTRACT_ASSERT(coeffs.size() >= order + 1);
            BOOST_CONTRACT_ASSERT(std::isfinite(x0));
        })
        .postcondition([&] {
            for (std::size_t k = 0; k <= order; ++k) {
                BOOST_CONTRACT_ASSERT(std::isfinite(coeffs[k]));
            }
        });

    const double s0 = std::sin(x0);
    const double c0 = std::cos(x0);
    const double derivs[4] = {c0, -s0, -c0, s0};
    coeffs_trig(coeffs, derivs, order);
}

void coeffs_ln(std::vector<double>& coeffs, double x0, std::size_t order) {
    boost::contract::check c = boost::contract::function()
        .precondition([&] {
            BOOST_CONTRACT_ASSERT(coeffs.size() >= order + 1);
            BOOST_CONTRACT_ASSERT(std::isfinite(x0));
            BOOST_CONTRACT_ASSERT(x0 > 0.0);
        })
        .postcondition([&] {
            for (std::size_t k = 0; k <= order; ++k) {
                BOOST_CONTRACT_ASSERT(std::isfinite(coeffs[k]));
            }
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

} // namespace anônimo

// --- Implementação da Classe TaylorPolynomial ---

TaylorPolynomial::TaylorPolynomial(Function function_type, double x0, TaylorOrder order)
    : order_(order.value()), center_(x0), function_type_(function_type) {
    boost::contract::check c = boost::contract::function()
        .precondition([&] {
            BOOST_CONTRACT_ASSERT(std::isfinite(x0));
            BOOST_CONTRACT_ASSERT(order.value() >= 0);
            BOOST_CONTRACT_ASSERT(order.value() <= 20);
            BOOST_CONTRACT_ASSERT(function_type != Function::Ln || x0 > 0.0);
        })
        .postcondition([&] {
            BOOST_CONTRACT_ASSERT(order_ == order.value());
            BOOST_CONTRACT_ASSERT(center_ == x0);
            BOOST_CONTRACT_ASSERT(function_type_ == function_type);
            BOOST_CONTRACT_ASSERT(coefficients_.size() == static_cast<std::size_t>(order.value() + 1));
            for (double coeff : coefficients_) {
                BOOST_CONTRACT_ASSERT(std::isfinite(coeff));
            }
        });

    const auto u_order = static_cast<std::size_t>(order.value());
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

TaylorPolynomial::TaylorPolynomial(Function function_type, double x0, int order)
    : TaylorPolynomial(function_type, x0, TaylorOrder(order)) {}


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
    // Algoritmo de Horner
    for (std::size_t i = coefficients.size(); i > 0; --i) {
        result = result * u + coefficients[i - 1];
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
            BOOST_CONTRACT_ASSERT(coefficients_.empty() || x != center_ || result == coefficients_[0]);
        });

    result = evaluate_polynomial(coefficients_, x, center_);
    return result;
}

// --- Implementação da Função Pública de Aproximação ---

TaylorResult approximate_taylor(Function function_type, double x, double x0, TaylorOrder order) {
    TaylorResult result{0.0, order.value(), x0, function_type};

    boost::contract::check c = boost::contract::function()
        .precondition([&] {
            BOOST_CONTRACT_ASSERT(std::isfinite(x));
            BOOST_CONTRACT_ASSERT(std::isfinite(x0));
            BOOST_CONTRACT_ASSERT(order.value() >= 0);
            BOOST_CONTRACT_ASSERT(order.value() <= 20);
            BOOST_CONTRACT_ASSERT(function_type != Function::Ln || (x > 0.0 && x0 > 0.0));
        })
        .postcondition([&] {
            BOOST_CONTRACT_ASSERT(result.order == order.value());
            BOOST_CONTRACT_ASSERT(result.center == x0);
            BOOST_CONTRACT_ASSERT(result.function_type == function_type);
            BOOST_CONTRACT_ASSERT(std::isfinite(result.value));
        });

    TaylorPolynomial poly(function_type, x0, order);
    result.value = poly.evaluate(x);
    return result;
}

} // namespace taylor

