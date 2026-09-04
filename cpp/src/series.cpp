#include "series.hpp"

#include <boost/contract.hpp>
#include <cmath>

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

template <typename PreconditionFn, typename PostconditionFn, typename TermFn>
SeriesResult compute_series_generic(
    const ConvergenceCriteria& criteria,
    PreconditionFn&& precondition,
    PostconditionFn&& postcondition,
    TermFn&& get_term
) {
    SeriesResult result{SeriesStatus::Diverged, SeriesValue(0.0), IterationCount(0)};

    boost::contract::check c = boost::contract::function()
        .precondition(std::forward<PreconditionFn>(precondition))
        .postcondition([&] {
            BOOST_CONTRACT_ASSERT(std::isfinite(result.sum.value()));
            BOOST_CONTRACT_ASSERT(result.iterations.value() >= 1);
            BOOST_CONTRACT_ASSERT(result.iterations.value() <= criteria.n_max.value());
            // Run function-specific postconditions
            postcondition(result);
        });

    double total_sum = 0.0;
    int iterations = 0;
    SeriesStatus status = SeriesStatus::MaxIterationsReached;

    for (int k = 1; k <= criteria.n_max.value(); ++k) {
        const double term = get_term(k);
        total_sum += term;
        ++iterations;

        if (has_converged(term, criteria.eps.value())) {
            status = SeriesStatus::Converged;
            break;
        }
    }

    result = SeriesResult{status, SeriesValue(total_sum), IterationCount(iterations)};
    return result;
}

SeriesResult compute_series(const GeometricSeries& series, const ConvergenceCriteria& criteria) {
    double current_term = series.a.value();

    return compute_series_generic(
        criteria,
        [&] {
            BOOST_CONTRACT_ASSERT(std::isfinite(series.a.value()));
            BOOST_CONTRACT_ASSERT(std::isfinite(series.r.value()));
            BOOST_CONTRACT_ASSERT(std::abs(series.r.value()) < 1.0);
            BOOST_CONTRACT_ASSERT(criteria.eps.value() > 0.0);
            BOOST_CONTRACT_ASSERT(std::isfinite(criteria.eps.value()));
            BOOST_CONTRACT_ASSERT(criteria.n_max.value() >= 1);
        },
        [&](const SeriesResult& res) {
            BOOST_CONTRACT_ASSERT(series.a.value() != 0.0 || res.sum.value() == 0.0);
        },
        [&](int k) {
            if (k == 1) return series.a.value();
            current_term = next_geometric_term(current_term, series.r.value());
            return current_term;
        }
    );
}

SeriesResult compute_p_series(const PSeries& series, const ConvergenceCriteria& criteria) {
    return compute_series_generic(
        criteria,
        [&] {
            BOOST_CONTRACT_ASSERT(std::isfinite(series.p.value()));
            BOOST_CONTRACT_ASSERT(series.p.value() > 1.0);
            BOOST_CONTRACT_ASSERT(criteria.eps.value() > 0.0);
            BOOST_CONTRACT_ASSERT(std::isfinite(criteria.eps.value()));
            BOOST_CONTRACT_ASSERT(criteria.n_max.value() >= 1);
        },
        [&](const SeriesResult& res) {
            BOOST_CONTRACT_ASSERT(res.sum.value() >= 1.0);
        },
        [&](int k) {
            return p_series_term(k, series.p.value());
        }
    );
}