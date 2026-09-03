#pragma once

#include <functional>
#include <string>

// Estrutura para encapsular o resultado da integração numérica
struct IntegrationResult {
    double value;
    int intervals;
    std::string method;
};

// Integração pela regra dos trapézios composta
IntegrationResult integrate_trapezoidal(const std::function<double(double)>& f, double a, double b, int n);

// Integração pela regra de Simpson 1/3 composta (requer n par)
IntegrationResult integrate_simpson(const std::function<double(double)>& f, double a, double b, int n);

// Fachada/Estratégia unificada de integração
IntegrationResult integrate(
    const std::function<double(double)>& f,
    double a,
    double b,
    int n,
    const std::string& method = "simpson"
);
