#pragma once

// Struct estruturado para evitar o code smell de "Primitive Obsession"
struct ResultadoSerie {
    bool convergeBool;
    double soma;
    int iteracoes;
};

// Assinatura da função de cálculo de séries geométricas
ResultadoSerie computarSerie(
    double termo_inicial, 
    double razao, 
    double termo_minimo = 1e-7, 
    int indice_maximo = 10000
);
