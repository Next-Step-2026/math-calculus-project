import math

import mathlab
import pytest
from mathlab.contracts import (
    ContractError,
    InvariantError,
    PostconditionError,
    PreconditionError,
    check_invariant,
    ensure,
    invariant,
    postcondition,
    precondition,
    require,
)


def test_contracts_hierarchy():
    assert issubclass(PreconditionError, ValueError)
    assert issubclass(PreconditionError, ContractError)
    assert issubclass(PostconditionError, RuntimeError)
    assert issubclass(PostconditionError, ContractError)
    assert issubclass(InvariantError, RuntimeError)
    assert issubclass(InvariantError, ContractError)


def test_require_precondition():
    require(1 + 1 == 2, "Matemática básica falhou")
    with pytest.raises(PreconditionError, match="x must be positive"):
        x = -5
        require(x > 0, "x must be positive")
    with pytest.raises(PreconditionError, match="x must be positive"):
        x = 0
        require(x > 0, "x must be positive")


def test_ensure_postcondition():
    val = 10.5
    ensure(math.isfinite(val), "Valor deve ser finito")
    with pytest.raises(PostconditionError, match="Result must not be NaN"):
        res = float("nan")
        ensure(not math.isnan(res), "Result must not be NaN")


def test_check_invariant():
    balance = 100
    check_invariant(balance >= 0, "Saldo não pode ser negativo")
    with pytest.raises(InvariantError, match="Saldo não pode ser negativo"):
        balance = -10
        check_invariant(balance >= 0, "Saldo não pode ser negativo")


def test_precondition_decorator():
    @precondition(lambda x: x > 0, "Argument x must be strictly positive")
    def sqrt_approx(x: float) -> float:
        return math.sqrt(x)

    assert sqrt_approx(4.0) == 2.0
    with pytest.raises(PreconditionError, match="Argument x must be strictly positive"):
        sqrt_approx(-1.0)


def test_postcondition_decorator():
    @postcondition(lambda res: res > 0, "Result must be positive")
    def square(x: float) -> float:
        return x * x

    assert square(3.0) == 9.0

    @postcondition(lambda res: res > 0, "Result must be positive")
    def faulty_abs(x: float) -> float:
        return -abs(x)

    with pytest.raises(PostconditionError, match="Result must be positive"):
        faulty_abs(5.0)


def test_invariant_decorator():
    @invariant(lambda obj: obj.width >= 0 and obj.height >= 0, "Dimensions must be non-negative")
    class Rectangle:
        def __init__(self, width: float, height: float):
            self.width = width
            self.height = height

        def scale(self, factor: float) -> None:
            self.width *= factor
            self.height *= factor

    r = Rectangle(10.0, 20.0)
    assert r.width == 10.0 and r.height == 20.0
    r.scale(2.0)
    assert r.width == 20.0 and r.height == 40.0

    with pytest.raises(InvariantError, match="Dimensions must be non-negative"):
        Rectangle(-5.0, 10.0)

    with pytest.raises(InvariantError, match="Post-call invariant violated"):
        r.scale(-1.0)


def test_integration_with_mathlab_preconditions():
    with pytest.raises(PreconditionError, match="Number of intervals must be positive"):
        mathlab.integrate_trapezoidal(lambda x: x, 0.0, 1.0, 0)

    with pytest.raises(PreconditionError, match="eps must be strictly positive"):
        mathlab.compute_series(1.0, 0.5, eps=-1.0)

    with pytest.raises(PreconditionError, match="Geometric series diverges"):
        mathlab.compute_series(1.0, 2.0)

    with pytest.raises(PreconditionError, match="order must be between 0 and 20"):
        mathlab.approximate_taylor("exp", 1.0, 0.0, 25)
