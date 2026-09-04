#ifndef TAYLOR_HPP
#define TAYLOR_HPP

#include <stdexcept>
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
 * @brief Value Object para a ordem do Polinômio de Taylor (0 <= order <= 20).
 */
class TaylorOrder {
    int value_ = 0;
public:
    constexpr TaylorOrder() noexcept = default;
    explicit TaylorOrder(int n) {
        if (n < 0 || n > 20) {
            throw std::invalid_argument("Taylor polynomial order must be between 0 and 20");
        }
        value_ = n;
    }
    [[nodiscard]] constexpr int value() const noexcept { return value_; }
    constexpr operator int() const noexcept { return value_; }
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
    TaylorPolynomial(Function function_type, double x0, TaylorOrder order);
    TaylorPolynomial(Function function_type, double x0, int order);
    /**
     * @brief Construtor da classe.
     * @param function_type O tipo da função (ex: Function::Sin).
     * @param x0 O centro da expansão.
     * @param order A ordem do polinômio (deve ser não-negativa).
     */

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
TaylorResult approximate_taylor(Function function_type, double x, double x0, TaylorOrder order);
inline TaylorResult approximate_taylor(Function function_type, double x, double x0, int order) {
    return approximate_taylor(function_type, x, x0, TaylorOrder(order));
}
/**
 * @brief Avalia polinomio via algoritmo de Horner
 */
double evaluate_polynomial(const std::vector<double>& coefficients, double x, double center = 0.0);

} // namespace taylor

#endif // TAYLOR_HPP
