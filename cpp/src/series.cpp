#include "series.hpp"
#include <cmath>
#include <stdexcept>


void verify_parameters(
    double termo_inicial,
    double razao,
    double termo_minimo,
    int indice_maximo
) {
    if (termo_minimo <= 0.0) {
        throw std::invalid_argument("A tolerancia (termo_minimo) deve ser estritamente positiva (> 0).");
    }
    if (indice_maximo < 1) {
        throw std::invalid_argument("O numero maximo de iteracoes (indice_maximo) deve ser no minimo 1.");
    }
    if (std::abs(razao) >= 1.0) {
        throw std::runtime_error("A serie geometrica diverge estritamente para |razao| >= 1.0.");
    }
}

ResultadoSerie computarSerie(
    double termo_inicial, 
    double razao, 
    double termo_minimo, 
    int indice_maximo
) {
    verify_parameters(termo_inicial, razao, termo_minimo, indice_maximo);

    // 2. Acumulação termo a termo da série geométrica (a_k = a_{k-1} * r)
    double termo_atual = termo_inicial;
    double soma_acumulada = 0.0;
    int iteracoes_feitas = 0;
    bool convergiu = false;

    for (int k = 0; k < indice_maximo; ++k) {
        soma_acumulada += termo_atual;
        iteracoes_feitas++;

        // Critério de parada por tolerância residual
        if (std::abs(termo_atual) < termo_minimo) {
            convergiu = true;
            break;
        }

        termo_atual *= razao;
    }

    return ResultadoSerie{convergiu, soma_acumulada, iteracoes_feitas};
}
