#pragma once

#include <cmath>
#include <stdexcept>

// ============================================================================
// PROPERTY-CONSTRAINED VALUE OBJECTS (Strong Types com Auto-Validação)
// ============================================================================

class Tolerance {
    double value_ = 1e-7;
public:
    constexpr Tolerance() noexcept = default;
    explicit Tolerance(double v) : value_(v) {
        if (v <= 0.0) {
            throw std::invalid_argument("Tolerance (eps) must be strictly positive (> 0).");
        }
    }
    [[nodiscard]] constexpr double value() const noexcept { return value_; }
    constexpr operator double() const noexcept { return value_; }
};

class MaxIterations {
    int value_ = 10000;
public:
    constexpr MaxIterations() noexcept = default;
    explicit MaxIterations(int v) : value_(v) {
        if (v < 1) {
            throw std::invalid_argument("Maximum iterations (n_max) must be at least 1.");
        }
    }
    [[nodiscard]] constexpr int value() const noexcept { return value_; }
    constexpr operator int() const noexcept { return value_; }
};

class GeometricRatio {
    double value_ = 0.5;
public:
    constexpr GeometricRatio() noexcept = default;
    explicit GeometricRatio(double v) : value_(v) {
        if (std::abs(v) >= 1.0) {
            throw std::runtime_error("Geometric series diverges for |r| >= 1.0.");
        }
    }
    [[nodiscard]] constexpr double value() const noexcept { return value_; }
    constexpr operator double() const noexcept { return value_; }
};

class PSeriesExponent {
    double value_ = 2.0;
public:
    constexpr PSeriesExponent() noexcept = default;
    explicit PSeriesExponent(double v) : value_(v) {
        if (v <= 1.0) {
            throw std::runtime_error("p-series diverges for p <= 1.0.");
        }
    }
    [[nodiscard]] constexpr double value() const noexcept { return value_; }
    constexpr operator double() const noexcept { return value_; }
};

// ============================================================================
// DOMAIN VALUE OBJECTS (Agrupamentos Semânticos de Séries)
// ============================================================================

struct ConvergenceCriteria {
    Tolerance eps{};
    MaxIterations n_max{};

    constexpr ConvergenceCriteria() = default;
    ConvergenceCriteria(double e, int n) : eps(Tolerance(e)), n_max(MaxIterations(n)) {}

    void validate() const;
};

struct GeometricSeries {
    double a = 1.0;
    GeometricRatio r{};

    constexpr GeometricSeries() = default;
    GeometricSeries(double initial_term, double ratio)
        : a(initial_term), r(GeometricRatio(ratio)) {}
    constexpr GeometricSeries(double initial_term, GeometricRatio ratio)
        : a(initial_term), r(ratio) {}
};

struct PSeries {
    PSeriesExponent p{};

    constexpr PSeries() = default;
    explicit PSeries(double exp) : p(PSeriesExponent(exp)) {}
    constexpr explicit PSeries(PSeriesExponent exp) : p(exp) {}
};
struct SeriesResult {
    bool converged;
    double sum;
    int iterations;
};

// Funções primárias do núcleo operando estritamente sobre tipos de domínio:
void validate_series_parameters(
    const GeometricSeries& series,
    const ConvergenceCriteria& criteria = {}
);

SeriesResult compute_series(
    const GeometricSeries& series,
    const ConvergenceCriteria& criteria = {}
);

void validate_p_series_parameters(
    const PSeries& series,
    const ConvergenceCriteria& criteria = {}
);

SeriesResult compute_p_series(
    const PSeries& series,
    const ConvergenceCriteria& criteria = {}
);

// Inline convenience forwarders (não poluem o AST de series.cpp com primitivos):
inline void validate_series_parameters(double a, double r, const ConvergenceCriteria& criteria = {}) {
    validate_series_parameters(GeometricSeries{a, r}, criteria);
}
inline void validate_series_parameters(double a, double r, double eps, int n_max) {
    validate_series_parameters(GeometricSeries{a, r}, ConvergenceCriteria{eps, n_max});
}
inline SeriesResult compute_series(double a, double r, const ConvergenceCriteria& criteria = {}) {
    return compute_series(GeometricSeries{a, r}, criteria);
}
inline SeriesResult compute_series(double a, double r, double eps, int n_max) {
    return compute_series(GeometricSeries{a, r}, ConvergenceCriteria{eps, n_max});
}

inline void validate_p_series_parameters(double p, const ConvergenceCriteria& criteria = {}) {
    validate_p_series_parameters(PSeries{p}, criteria);
}
inline void validate_p_series_parameters(double p, double eps, int n_max) {
    validate_p_series_parameters(PSeries{p}, ConvergenceCriteria{eps, n_max});
}
inline SeriesResult compute_p_series(double p, const ConvergenceCriteria& criteria = {}) {
    return compute_p_series(PSeries{p}, criteria);
}
inline SeriesResult compute_p_series(double p, double eps, int n_max) {
    return compute_p_series(PSeries{p}, ConvergenceCriteria{eps, n_max});
}
