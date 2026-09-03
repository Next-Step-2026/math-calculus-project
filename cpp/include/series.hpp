#pragma once

// Struct estruturado para evitar o code smell de "Primitive Obsession"
struct ResultadoSerie {
    bool convergeBool;
    double soma;
    int iteracoes;
};

// Verifica os parametros da serie
void verify_parameters(
    double termo_inicial, 
    double razao, 
    double termo_minimo, 
    int indice_maximo
);

// Assinatura da função de cálculo de séries geométricas
ResultadoSerie computarSerie(
    double termo_inicial, 
    double razao, 
    double termo_minimo = 1e-7, 
    int indice_maximo = 10000
);
