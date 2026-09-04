#include <cmath>
#include <stdexcept>
#include <boost/contract.hpp>
#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "../include/soma.hpp"

// Implementação matemática pura com contratos Boost (Pré-condições e Pós-condições)
float soma(float primeiro, float segundo) {
    float value = 0.0f;

    boost::contract::check c = boost::contract::function()
        .precondition([&] {
            BOOST_CONTRACT_ASSERT(std::isfinite(primeiro));
            BOOST_CONTRACT_ASSERT(std::isfinite(segundo));
        })
        .postcondition([&] {
            BOOST_CONTRACT_ASSERT(std::isfinite(value));
            BOOST_CONTRACT_ASSERT(value == primeiro + segundo);
            BOOST_CONTRACT_ASSERT(value == segundo + primeiro);
        });

    // implementacao
    value = primeiro + segundo;

    return value;
}

PYBIND11_MODULE(_soma_cpp, m) {
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

    m.doc() = "Internal C++ addition module exposed via pybind11";  
    m.def("soma", &soma, "Adds two numbers", py::arg("a"), py::arg("b"));
}