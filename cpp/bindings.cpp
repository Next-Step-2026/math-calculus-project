#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <boost/contract.hpp>

#include "include/integration.hpp"
#include "include/series.hpp"
#include "include/taylor.hpp"

namespace py = pybind11;

namespace {

void bind_series(py::module_& m) {
    py::class_<ConvergenceCriteria>(m, "ConvergenceCriteria")
        .def(py::init<double, int>(), py::arg("eps") = 1e-7, py::arg("n_max") = 10000)
        .def_property(
            "eps",
            [](const ConvergenceCriteria& c) { return c.eps.value(); },
            [](ConvergenceCriteria& c, double v) { c.eps = Tolerance(v); }
        )
        .def_property(
            "n_max",
            [](const ConvergenceCriteria& c) { return c.n_max.value(); },
            [](ConvergenceCriteria& c, int v) { c.n_max = MaxIterations(v); }
        );

    py::enum_<SeriesStatus>(m, "SeriesStatus")
        .value("Converged", SeriesStatus::Converged)
        .value("Diverged", SeriesStatus::Diverged)
        .value("MaxIterationsReached", SeriesStatus::MaxIterationsReached)
        .export_values();

    py::class_<GeometricSeries>(m, "GeometricSeries")
        .def(py::init<double, double>(), py::arg("a") = 1.0, py::arg("r") = 0.5)
        .def_property(
            "a",
            [](const GeometricSeries& s) { return s.a.value(); },
            [](GeometricSeries& s, double v) { s.a = InitialTerm(v); }
        )
        .def_property(
            "r",
            [](const GeometricSeries& s) { return s.r.value(); },
            [](GeometricSeries& s, double v) { s.r = GeometricRatio(v); }
        );

    py::class_<PSeries>(m, "PSeries")
        .def(py::init<double>(), py::arg("p") = 2.0)
        .def_property(
            "p",
            [](const PSeries& s) { return s.p.value(); },
            [](PSeries& s, double v) { s.p = PSeriesExponent(v); }
        );
    py::class_<SeriesResult>(m, "SeriesResult")
        .def_readonly("status", &SeriesResult::status)
        .def_property_readonly("converged", [](const SeriesResult& r) { return r.converged(); })
        .def_property_readonly("sum", [](const SeriesResult& r) { return r.sum.value(); })
        .def_property_readonly("iterations", [](const SeriesResult& r) { return r.iterations.value(); })
        .def("__repr__", [](const SeriesResult& r) {
            return "<SeriesResult converged=" + std::string(r.converged() ? "True" : "False") +
                   " sum=" + std::to_string(r.sum.value()) +
                   " iterations=" + std::to_string(r.iterations.value()) + ">";
        });

    m.def(
        "compute_series",
        [](double a, double r, double eps, int n_max) {
            return compute_series(GeometricSeries{a, r}, ConvergenceCriteria{eps, n_max});
        },
        py::arg("a"),
        py::arg("r"),
        py::arg("eps") = 1e-7,
        py::arg("n_max") = 10000,
        "Calcula soma de serie geometrica sum(a * r^k)"
    );

    m.def(
        "compute_p_series",
        [](double p, double eps, int n_max) {
            return compute_p_series(PSeries{p}, ConvergenceCriteria{eps, n_max});
        },
        py::arg("p"),
        py::arg("eps") = 1e-7,
        py::arg("n_max") = 10000,
        "Calcula e verifica convergencia de p-serie sum(1 / k^p)"
    );
}

void bind_integration(py::module_& m) {
    py::class_<IntegrationInterval>(m, "IntegrationInterval")
        .def(py::init<double, double>(), py::arg("a"), py::arg("b"))
        .def_property_readonly("lower", &IntegrationInterval::lower)
        .def_property_readonly("upper", &IntegrationInterval::upper)
        .def_property_readonly("width", &IntegrationInterval::width)
        .def_property_readonly("is_degenerate", &IntegrationInterval::is_degenerate);

    py::class_<IntegrationResult>(m, "IntegrationResult")
        .def_readonly("value", &IntegrationResult::value)
        .def_readonly("intervals", &IntegrationResult::intervals)
        .def_readonly("method", &IntegrationResult::method)
        .def("__repr__", [](const IntegrationResult& r) {
            return "<IntegrationResult value=" + std::to_string(r.value) +
                   " intervals=" + std::to_string(r.intervals) +
                   " method='" + r.method + "'>";
        });

    m.def(
        "integrate_trapezoidal",
        [](const std::function<double(double)>& f, double a, double b, int n) {
            return integrate_trapezoidal(f, a, b, n);
        },
        py::arg("f"),
        py::arg("a"),
        py::arg("b"),
        py::arg("n"),
        "Integracao numerica pela regra composta dos trapezios"
    );

    m.def(
        "integrate_simpson",
        [](const std::function<double(double)>& f, double a, double b, int n) {
            return integrate_simpson(f, a, b, n);
        },
        py::arg("f"),
        py::arg("a"),
        py::arg("b"),
        py::arg("n"),
        "Integracao numerica pela regra composta de Simpson 1/3 (n par)"
    );
}

void bind_taylor(py::module_& m) {
    py::class_<taylor::TaylorResult>(m, "TaylorResult")
        .def_readonly("value", &taylor::TaylorResult::value)
        .def_readonly("order", &taylor::TaylorResult::order)
        .def_readonly("center", &taylor::TaylorResult::center)
        .def("__repr__", [](const taylor::TaylorResult& r) {
            return "<TaylorResult order=" + std::to_string(r.order) +
                   " center=" + std::to_string(r.center) +
                   " value=" + std::to_string(r.value) + ">";
        });

    m.def(
        "approximate_taylor",
        [](const std::string& func_name, double x, double x0, int order) {
            taylor::Function f;
            if (func_name == "exp") f = taylor::Function::Exp;
            else if (func_name == "sin") f = taylor::Function::Sin;
            else if (func_name == "cos") f = taylor::Function::Cos;
            else if (func_name == "ln") f = taylor::Function::Ln;
            else throw std::invalid_argument("Unsupported function: " + func_name);
            return taylor::approximate_taylor(f, x, x0, order);
        },
        py::arg("function_name"),
        py::arg("x"),
        py::arg("x0"),
        py::arg("order"),
        "Aproxima funcao analitica via polinomio de Taylor"
    );

    m.def(
        "evaluate_polynomial",
        &taylor::evaluate_polynomial,
        py::arg("coefficients"),
        py::arg("x"),
        py::arg("center") = 0.0,
        "Avalia polinomio via algoritmo de Horner"
    );
}

} // namespace

PYBIND11_MODULE(_mathcore, m) {
    boost::contract::set_precondition_failure([](boost::contract::from) {
        try {
            throw;
        } catch (const boost::contract::assertion_failure& e) {
            throw std::invalid_argument(e.what());
        }
    });

    boost::contract::set_postcondition_failure([](boost::contract::from) {
        try {
            throw;
        } catch (const boost::contract::assertion_failure& e) {
            throw std::runtime_error(e.what());
        }
    });

    m.doc() = "Modulo nativo C++ de calculo numerico (mathlab)";
    bind_series(m);
    bind_integration(m);
    bind_taylor(m);
}

