#ifndef TAYLOR_HPP
#define TAYLOR_HPP

#include <vector>

namespace taylor {

/**
 * @brief Enumeração segura para representar as funções suportadas.
 * Soluciona o "code smell" de "Primitive Obsession".
 */
enum class Function {
    Exp,
    Sin,
    Cos,
    Ln
};

/**
 * @brief Estrutura para armazenar o resultado da aproximação de Taylor.
 */
struct TaylorResult {
    double value;
    int order;
    double center;
    Function function_type;
};

/**
 * @brief Classe que representa um Polinômio de Taylor.
 * Encapsula os coeficientes, o centro e a ordem, resolvendo a
 * "Primitive Obsession" de usar um std::vector<double> solto.
 */
class TaylorPolynomial {
public:
    /**
     * @brief Construtor da classe.
     * @param function_type O tipo da função (ex: Function::Sin).
     * @param x0 O centro da expansão.
     * @param order A ordem do polinômio (deve ser não-negativa).
     */
    TaylorPolynomial(Function function_type, double x0, int order);

    /**
     * @brief Avalia o polinômio em um ponto 'x'.
     * @param x O ponto de avaliação.
     * @return O valor do polinômio P(x).
     */
    double evaluate(double x) const;

    // Getters para acessar os dados encapsulados de forma segura.
    int get_order() const { return order_; }
    double get_center() const { return center_; }
    Function get_function_type() const { return function_type_; }
    const std::vector<double>& get_coefficients() const { return coefficients_; }

private:
    std::vector<double> coefficients_;
    int order_;
    double center_;
    Function function_type_;
};

/**
 * @brief Função de alto nível para aproximar o valor de uma função.
 * Agora retorna o resultado diretamente, usando a nova classe.
 *
 * @param function_type O tipo de função a ser aproximada.
 * @param x O ponto onde a aproximação será calculada.
 * @param x0 O centro da expansão de Taylor.
 * @param order A ordem do polinômio a ser usado.
 * @return Um objeto TaylorResult contendo o valor aproximado e os detalhes.
 */
TaylorResult approximate_taylor(Function function_type, double x, double x0, int order);

} // namespace taylor

#endif // TAYLOR_HPP
