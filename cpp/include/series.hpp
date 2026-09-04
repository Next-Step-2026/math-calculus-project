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

class InitialTerm {
    double value_ = 1.0;
public:
    constexpr InitialTerm() noexcept = default;
    explicit constexpr InitialTerm(double v) noexcept : value_(v) {}
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
    InitialTerm a{};
    GeometricRatio r{};

    constexpr GeometricSeries() = default;
    GeometricSeries(double initial_term, double ratio)
        : a(InitialTerm(initial_term)), r(GeometricRatio(ratio)) {}
    constexpr GeometricSeries(InitialTerm initial_term, GeometricRatio ratio)
        : a(initial_term), r(ratio) {}
};

struct PSeries {
    PSeriesExponent p{};

    constexpr PSeries() = default;
    explicit PSeries(double exp) : p(PSeriesExponent(exp)) {}
    constexpr explicit PSeries(PSeriesExponent exp) : p(exp) {}
};
enum class SeriesStatus {
    Converged,
    Diverged,
    MaxIterationsReached
};

class SeriesValue {
    double value_ = 0.0;
public:
    constexpr SeriesValue() noexcept = default;
    explicit constexpr SeriesValue(double value) noexcept : value_(value) {}
    [[nodiscard]] constexpr double value() const noexcept { return value_; }
    constexpr operator double() const noexcept { return value_; }
};

class IterationCount {
    int value_ = 0;
public:
    constexpr IterationCount() noexcept = default;
    explicit constexpr IterationCount(int value) : value_(value) {
        if (value < 0) {
            throw std::invalid_argument("Iteration count must be non-negative.");
        }
    }
    [[nodiscard]] constexpr int value() const noexcept { return value_; }
    constexpr operator int() const noexcept { return value_; }
};

struct SeriesResult {
    SeriesStatus status = SeriesStatus::Diverged;
    SeriesValue sum{};
    IterationCount iterations{};

    constexpr SeriesResult() = default;
    constexpr SeriesResult(SeriesStatus s, SeriesValue sm, IterationCount it)
        : status(s), sum(sm), iterations(it) {}
    constexpr SeriesResult(bool conv, double sm, int it)
        : status(conv ? SeriesStatus::Converged : SeriesStatus::MaxIterationsReached),
          sum(SeriesValue(sm)),
          iterations(IterationCount(it)) {}

    [[nodiscard]] constexpr bool converged() const noexcept {
        return status == SeriesStatus::Converged;
    }
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
