#include <pybind11/pybind11.h>
namespace py = pybind11;

#include "../include/soma.hpp"

// Implementação matemática pura (Sem nenhuma dependência de Python ou pybind11)
float soma(float primeiro, float segundo) {

    // implementacao
    float value = primeiro + segundo;

    return value;
}

PYBIND11_MODULE(_soma_cpp, m) {
    m.doc() = "Internal C++ addition module exposed via pybind11";
    m.def("soma", &soma, "Adds two numbers", py::arg("a"), py::arg("b"));
}