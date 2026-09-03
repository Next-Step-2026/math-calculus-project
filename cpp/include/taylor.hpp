#pragma once

#include <string>
#include <vector>

// Estrutura para encapsular o resultado da aproximação por Taylor
struct TaylorResult {
    double value;
    int order;
    double center;
    std::string function_name;
};

// Avaliação de polinômio na forma sum(c[k] * (x - center)^k, k=0..n) usando algoritmo de Horner
double evaluate_polynomial(const std::vector<double>& coefficients, double x, double center = 0.0);

// Gera os coeficientes c_k = f^(k)(x0) / k! para funções analíticas elementares
std::vector<double> taylor_coefficients(const std::string& function_name, double x0, int order);

// Aproximação analítica por polinômio de Taylor
TaylorResult approximate_taylor(const std::string& function_name, double x, double x0, int order);
