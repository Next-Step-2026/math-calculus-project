#include "../include/integration.hpp"

#include <boost/contract.hpp>
#include <cmath>

IntegrationResult integrate_simpson(
    const std::function<double(double)>& f,
    const IntegrationInterval& interval,
    SimpsonSubintervals n
) {
    IntegrationResult result{0.0, n.value(), "simpson"};

    boost::contract::check c = boost::contract::function()
        .precondition([&] {
            BOOST_CONTRACT_ASSERT(static_cast<bool>(f));
            BOOST_CONTRACT_ASSERT(interval.lower() <= interval.upper());
            BOOST_CONTRACT_ASSERT(std::isfinite(interval.lower()));
            BOOST_CONTRACT_ASSERT(std::isfinite(interval.upper()));
            BOOST_CONTRACT_ASSERT(n.value() > 0);
            BOOST_CONTRACT_ASSERT(n.value() % 2 == 0);
        })
        .postcondition([&] {
            BOOST_CONTRACT_ASSERT(result.method == "simpson");
            BOOST_CONTRACT_ASSERT(result.intervals == n.value());
            BOOST_CONTRACT_ASSERT(!interval.is_degenerate() || result.value == 0.0);
            BOOST_CONTRACT_ASSERT(std::isfinite(result.value));
        });

    if (interval.is_degenerate()) {
        result.value = 0.0;
        return result;
    }

    const double h = interval.step_size(n.value());
    const double a = interval.lower();
    const double b = interval.upper();
    double sum = f(a) + f(b);

    for (int i = 1; i < n.value(); ++i) {
        const double x = a + static_cast<double>(i) * h;
        sum += (i % 2 == 1 ? 4.0 : 2.0) * f(x);
    }

    result.value = (sum * h) / 3.0;
    return result;
}

IntegrationResult integrate_trapezoidal(
    const std::function<double(double)>& f,
    const IntegrationInterval& interval,
    Subintervals n
) {
    IntegrationResult result{0.0, n.value(), "trapezoidal"};

    boost::contract::check c = boost::contract::function()
        .precondition([&] {
            BOOST_CONTRACT_ASSERT(static_cast<bool>(f));
            BOOST_CONTRACT_ASSERT(interval.lower() <= interval.upper());
            BOOST_CONTRACT_ASSERT(std::isfinite(interval.lower()));
            BOOST_CONTRACT_ASSERT(std::isfinite(interval.upper()));
            BOOST_CONTRACT_ASSERT(n.value() > 0);
        })
        .postcondition([&] {
            BOOST_CONTRACT_ASSERT(result.method == "trapezoidal");
            BOOST_CONTRACT_ASSERT(result.intervals == n.value());
            BOOST_CONTRACT_ASSERT(!interval.is_degenerate() || result.value == 0.0);
            BOOST_CONTRACT_ASSERT(std::isfinite(result.value));
        });

    if (interval.is_degenerate()) {
        result.value = 0.0;
        return result;
    }

    const double h = interval.step_size(n.value());
    const double a = interval.lower();
    const double b = interval.upper();
    double sum = 0.5 * (f(a) + f(b));

    for (int i = 1; i < n.value(); ++i) {
        const double x = a + static_cast<double>(i) * h;
        sum += f(x);
    }

    result.value = sum * h;
    return result;
}