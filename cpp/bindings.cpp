#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "include/integration.hpp"
#include "include/series.hpp"
#include "include/taylor.hpp"

namespace py = pybind11;

namespace {

void bind_series(py::module_& m) {
    py::class_<ConvergenceCriteria>(m, "ConvergenceCriteria")
        .def(py::init<double, int>(), py::arg("eps") = 1e-7, py::arg("n_max") = 10000)
        .def_readwrite("eps", &ConvergenceCriteria::eps)
        .def_readwrite("n_max", &ConvergenceCriteria::n_max);

    py::class_<SeriesResult>(m, "SeriesResult")
        .def_readonly("converged", &SeriesResult::converged)
        .def_readonly("sum", &SeriesResult::sum)
        .def_readonly("iterations", &SeriesResult::iterations)
        .def("__repr__", [](const SeriesResult& r) {
            return "<SeriesResult converged=" + std::string(r.converged ? "True" : "False") +
                   " sum=" + std::to_string(r.sum) +
                   " iterations=" + std::to_string(r.iterations) + ">";
        });

    m.def(
        "compute_series",
        py::overload_cast<double, double, double, int>(&compute_series),
        py::arg("a"),
        py::arg("r"),
        py::arg("eps") = 1e-7,
        py::arg("n_max") = 10000,
        "Calcula soma de serie geometrica sum(a * r^k)"
    );

    m.def(
        "compute_p_series",
        py::overload_cast<double, double, int>(&compute_p_series),
        py::arg("p"),
        py::arg("eps") = 1e-7,
        py::arg("n_max") = 10000,
        "Calcula e verifica convergencia de p-serie sum(1 / k^p)"
    );
}

void bind_integration(py::module_& m) {
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
        &integrate_trapezoidal,
        py::arg("f"),
        py::arg("a"),
        py::arg("b"),
        py::arg("n"),
        "Integracao numerica pela regra composta dos trapezios"
    );

    m.def(
        "integrate_simpson",
        &integrate_simpson,
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
    m.doc() = "Modulo nativo C++ de calculo numerico (mathlab)";
    bind_series(m);
    bind_integration(m);
    bind_taylor(m);
}
