
import mathlab
import pytest


def test_trapezoidal_basic():
    # Integral de f(x) = x de 0 a 2 é 2.0
    result = mathlab.integrate_trapezoidal(lambda x: x, 0.0, 2.0, 100)
    assert result == pytest.approx(2.0, rel=1e-4)

def test_simpson_basic():
    # Integral de f(x) = x^2 de 0 a 3 é 9.0
    result = mathlab.integrate_simpson(lambda x: x**2, 0.0, 3.0, 100)
    assert result == pytest.approx(9.0, rel=1e-4)

def test_mathlab_same_limits():
    # Limites iguais devem retornar 0.0
    res1 = mathlab.integrate_trapezoidal(lambda x: x**3, 5.0, 5.0, 10)
    res2 = mathlab.integrate_simpson(lambda x: x**3, 5.0, 5.0, 10)
    assert res1 == 0.0
    assert res2 == 0.0

def test_invalid_n_throws_exception():
    # Testa se std::invalid_argument é convertido em ValueError no Python
    with pytest.raises(ValueError, match="Number of intervals"):
        mathlab.integrate_trapezoidal(lambda x: x, 0.0, 1.0, 0)

    with pytest.raises(ValueError, match="Number of intervals"):
        mathlab.integrate_simpson(lambda x: x, 0.0, 1.0, -5)

def test_simpson_odd_n_throws_exception():
    # Regra de Simpson exige n par
    with pytest.raises(ValueError, match="requires an even number"):
        mathlab.integrate_simpson(lambda x: x, 0.0, 1.0, 3)


def test_integration_strategy_simpson():
    interval = mathlab.IntegrationInterval(0.0, 3.0)
    result = mathlab.integrate(lambda x: x**2, interval, 100, method="simpson")
    assert result == pytest.approx(9.0, rel=1e-4)


def test_integration_strategy_trapezoidal():
    interval = mathlab.IntegrationInterval(0.0, 2.0)
    result = mathlab.integrate(lambda x: x, interval, 100, method="trapezoidal")
    assert result == pytest.approx(2.0, rel=1e-4)


def test_integration_strategy_invalid_method():
    interval = mathlab.IntegrationInterval(0.0, 1.0)
    with pytest.raises(ValueError, match="method must be 'simpson' or 'trapezoidal'"):
        mathlab.integrate(lambda x: x, interval, 10, method="monte_carlo")


    with pytest.raises(ValueError, match="must be less than or equal to"):
        mathlab.integrate_trapezoidal(lambda x: x, 2.0, 1.0, 10)

    with pytest.raises(ValueError, match="must be less than or equal to"):
        mathlab.integrate_simpson(lambda x: x, 2.0, 1.0, 10)
