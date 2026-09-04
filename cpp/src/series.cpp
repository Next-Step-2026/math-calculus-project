#include "series.hpp"
#include <cmath>
#include <stdexcept>


void ConvergenceCriteria::validate() const {
    if (eps <= 0.0) {
        throw std::invalid_argument("Tolerance (eps) must be strictly positive (> 0).");
    }
    if (n_max < 1) {
        throw std::invalid_argument("Maximum iterations (n_max) must be at least 1.");
    }
}

namespace {

inline bool has_converged(double term, double eps) noexcept {
    return std::abs(term) < eps;
}

inline double next_geometric_term(double term, double r) noexcept {
    return term * r;
}

// EXTRACT FUNCTION: Cohesive helper for term k of p-series (1 / k^p)
inline double p_series_term(int k, double p) noexcept {
    return 1.0 / std::pow(static_cast<double>(k), p);
}

// DECOMPOSE CONDITIONAL: Named domain predicates for divergence rules
inline bool is_geometric_series_divergent(double r) noexcept {
    return std::abs(r) >= 1.0;
}

inline bool is_p_series_divergent(double p) noexcept {
    return p <= 1.0;
}

using DivergencePredicate = bool (*)(double) noexcept;

struct DivergenceRule {
    DivergencePredicate is_divergent = nullptr;
    const char* error_message = nullptr;

    void validate(double param) const {
        if (is_divergent != nullptr && is_divergent(param)) {
            throw std::runtime_error(error_message);
        }
    }
};

// CURRYING PURAMENTE UNARIO (1 -> 1 -> 1):
// 1. make_series_validator: vincula objeto de dominio DivergenceRule (1 arg)
// 2. lambda retornado: delega para criteria.validate() (1 arg)
// 3. lambda final: delega para rule.validate(param) (1 arg)
auto make_series_validator(const DivergenceRule& rule) {
    return [rule](const ConvergenceCriteria& criteria) {
        criteria.validate();
        return [rule](double param) {
            rule.validate(param);
        };
    };
}

const auto geometric_validator = make_series_validator({
    &is_geometric_series_divergent,
    "Geometric series diverges for |r| >= 1.0."
});

const auto p_series_validator = make_series_validator({
    &is_p_series_divergent,
    "p-series diverges for p <= 1.0."
});

} // namespace

void validate_series_parameters(const GeometricSeries& series, const ConvergenceCriteria& criteria) {
    geometric_validator(criteria)(series.r);
}

SeriesResult compute_series(const GeometricSeries& series, const ConvergenceCriteria& criteria) {
    validate_series_parameters(series, criteria);

    double current_term = series.a;
    double total_sum = 0.0;
    int iterations = 0;
    bool converged = false;

    for (int k = 0; k < criteria.n_max; ++k) {
        total_sum += current_term;
        ++iterations;

        if (has_converged(current_term, criteria.eps)) {
            converged = true;
            break;
        }

        current_term = next_geometric_term(current_term, series.r);
    }

    return SeriesResult{converged, total_sum, iterations};
}

void validate_p_series_parameters(const PSeries& series, const ConvergenceCriteria& criteria) {
    p_series_validator(criteria)(series.p);
}

SeriesResult compute_p_series(const PSeries& series, const ConvergenceCriteria& criteria) {
    validate_p_series_parameters(series, criteria);

    double total_sum = 0.0;
    int iterations = 0;
    bool converged = false;

    for (int k = 1; k <= criteria.n_max; ++k) {
        const double term = p_series_term(k, series.p);
        total_sum += term;
        ++iterations;

        if (has_converged(term, criteria.eps)) {
            converged = true;
            break;
        }
    }

    return SeriesResult{converged, total_sum, iterations};
}
