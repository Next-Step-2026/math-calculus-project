#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "include/integration.hpp"
#include "include/series.hpp"
#include "include/taylor.hpp"

namespace py = pybind11;

PYBIND11_MODULE(_mathcore, m) {
    m.doc() = "Modulo nativo C++ de calculo numerico (mathlab)";

    // --- Series Domain ---
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
        &compute_series,
        py::arg("a"),
        py::arg("r"),
        py::arg("eps") = 1e-7,
        py::arg("n_max") = 10000,
        "Calcula soma de serie geometrica sum(a * r^k)"
    );

    m.def(
        "compute_p_series",
        &compute_p_series,
        py::arg("p"),
        py::arg("eps") = 1e-7,
        py::arg("n_max") = 10000,
        "Calcula e verifica convergencia de p-serie sum(1 / k^p)"
    );

    // --- Integration Domain ---
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

    m.def(
        "integrate",
        &integrate,
        py::arg("f"),
        py::arg("a"),
        py::arg("b"),
        py::arg("n"),
        py::arg("method") = "simpson",
        "Estrategia unificada de integracao numerica ('simpson' ou 'trapezoidal')"
    );

    // --- Taylor Domain ---
    py::class_<TaylorResult>(m, "TaylorResult")
        .def_readonly("value", &TaylorResult::value)
        .def_readonly("order", &TaylorResult::order)
        .def_readonly("center", &TaylorResult::center)
        .def_readonly("function_name", &TaylorResult::function_name)
        .def("__repr__", [](const TaylorResult& r) {
            return "<TaylorResult function='" + r.function_name +
                   "' order=" + std::to_string(r.order) +
                   " center=" + std::to_string(r.center) +
                   " value=" + std::to_string(r.value) + ">";
        });

    m.def(
        "evaluate_polynomial",
        &evaluate_polynomial,
        py::arg("coefficients"),
        py::arg("x"),
        py::arg("center") = 0.0,
        "Avalia polinomio via algoritmo de Horner"
    );

    m.def(
        "taylor_coefficients",
        &taylor_coefficients,
        py::arg("function_name"),
        py::arg("x0"),
        py::arg("order"),
        "Calcula coeficientes de Taylor para funcoes analiticas ('exp', 'sin', 'cos', 'ln')"
    );

    m.def(
        "approximate_taylor",
        &approximate_taylor,
        py::arg("function_name"),
        py::arg("x"),
        py::arg("x0"),
        py::arg("order"),
        "Aproxima funcao analitica via polinomio de Taylor"
    );
}
