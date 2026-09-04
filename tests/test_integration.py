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


def test_integration_trigonometric_functions():
    import math

    # Integral de sin(x) de 0 a pi = 2.0
    val_sin_simpson = mathlab.integrate_simpson(math.sin, 0.0, math.pi, 100)
    assert val_sin_simpson == pytest.approx(2.0, rel=1e-5)

    val_sin_trap = mathlab.integrate_trapezoidal(math.sin, 0.0, math.pi, 200)
    assert val_sin_trap == pytest.approx(2.0, rel=1e-4)

    # Integral de cos(x) de 0 a pi/2 = 1.0
    val_cos_simpson = mathlab.integrate_simpson(math.cos, 0.0, math.pi / 2.0, 100)
    assert val_cos_simpson == pytest.approx(1.0, rel=1e-5)


def test_integration_polynomial_analytical_known():
    # Integral de (3x^2 - 2x + 1) de 0 a 2 = [x^3 - x^2 + x]_0^2 = 8 - 4 + 2 = 6.0
    val_poly = mathlab.integrate_simpson(lambda x: 3 * x**2 - 2 * x + 1, 0.0, 2.0, 50)
    assert val_poly == pytest.approx(6.0, rel=1e-6)

    # Simpson 1/3 é exato para polinômios cúbicos: integral de x^3 de 0 a 2 = 16/4 = 4.0
    val_cubic = mathlab.integrate_simpson(lambda x: x**3, 0.0, 2.0, 10)
    assert val_cubic == pytest.approx(4.0, rel=1e-6)


def test_integration_strategy_with_scalar_limits():
    # Testa a assinatura unificada com escalares a e b
    val_simpson = mathlab.integrate(lambda x: x**2, 0.0, 3.0, 100, method="simpson")
    assert val_simpson == pytest.approx(9.0, rel=1e-4)

    val_trap = mathlab.integrate(lambda x: x, 0.0, 2.0, 100, method="trapezoidal")
    assert val_trap == pytest.approx(2.0, rel=1e-4)

    with pytest.raises(TypeError):
        mathlab.integrate(lambda x: x, 0.0, 2.0)  # n ausente sem ser interval


def test_integration_contracts_postconditions():
    """Verify that Boost.Contract postconditions detect non-finite results."""
    from mathlab import _mathcore

    with pytest.raises(RuntimeError, match="std::isfinite"):
        _mathcore.integrate_trapezoidal(lambda x: float("inf"), 0.0, 1.0, 10)

    with pytest.raises(RuntimeError, match="std::isfinite"):
        _mathcore.integrate_simpson(lambda x: float("nan"), 0.0, 1.0, 10)
