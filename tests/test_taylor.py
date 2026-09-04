import math

import mathlab
import pytest


def test_taylor_exp_around_zero():
    # exp(1) com ordem 10 em torno de x0=0
    val = mathlab.approximate_taylor("exp", 1.0, 0.0, 10)
    assert val == pytest.approx(math.e, rel=1e-6)


def test_taylor_sin_around_zero():
    # sin(pi/6) = 0.5
    x = math.pi / 6.0
    val = mathlab.approximate_taylor("sin", x, 0.0, 9)
    assert val == pytest.approx(0.5, rel=1e-5)


def test_taylor_cos_around_zero():
    # cos(pi/3) = 0.5
    x = math.pi / 3.0
    val = mathlab.approximate_taylor("cos", x, 0.0, 10)
    assert val == pytest.approx(0.5, rel=1e-5)


def test_taylor_ln_around_one():
    # ln(1.5) em torno de x0=1.0
    val = mathlab.approximate_taylor("ln", 1.5, 1.0, 15)
    assert val == pytest.approx(math.log(1.5), rel=1e-4)


def test_taylor_order_zero():
    # Ordem 0 retorna f(x0)
    val = mathlab.approximate_taylor("exp", 5.0, 0.0, 0)
    assert val == pytest.approx(1.0)


def test_taylor_order_twenty():
    # Ordem 20 e limite maximo permitido
    val = mathlab.approximate_taylor("exp", 1.0, 0.0, 20)
    assert val == pytest.approx(math.e, rel=1e-9)


def test_taylor_invalid_order_negative():
    with pytest.raises(ValueError, match="order must be between 0 and 20"):
        mathlab.approximate_taylor("exp", 1.0, 0.0, -1)


def test_taylor_invalid_order_exceeds_twenty():
    with pytest.raises(ValueError, match="order must be between 0 and 20"):
        mathlab.approximate_taylor("exp", 1.0, 0.0, 21)


def test_taylor_unsupported_function():
    with pytest.raises(ValueError, match="func_name must be one of"):
        mathlab.approximate_taylor("tan", 1.0, 0.0, 5)


def test_taylor_type_errors():
    with pytest.raises(TypeError):
        mathlab.approximate_taylor(123, 1.0, 0.0, 5)  # type: ignore

    with pytest.raises(TypeError):
        mathlab.approximate_taylor("exp", "invalid", 0.0, 5)  # type: ignore


def test_evaluate_polynomial_horner():
    # P(x) = 1 + 2x + 3x^2 em x=2 -> 1 + 4 + 12 = 17
    result = mathlab.evaluate_polynomial([1.0, 2.0, 3.0], 2.0, center=0.0)
    assert result == pytest.approx(17.0)


def test_evaluate_polynomial_with_center():
    # P(x) = 1 + 2(x - 1) + 3(x - 1)^2 em x=3 -> 1 + 2(2) + 3(4) = 17
    result = mathlab.evaluate_polynomial([1.0, 2.0, 3.0], 3.0, center=1.0)
    assert result == pytest.approx(17.0)
