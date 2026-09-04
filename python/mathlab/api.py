import math
from collections.abc import Callable, Sequence
from typing import Final

from . import _mathcore
from .contracts import (
    InvariantError,
    PostconditionError,
    PreconditionError,
    check_invariant,
    ensure,
    postcondition,
    precondition,
    require,
)

ConvergenceCriteria: Final = _mathcore.ConvergenceCriteria
GeometricSeries: Final = _mathcore.GeometricSeries
IntegrationInterval: Final = _mathcore.IntegrationInterval
IntegrationResult: Final = _mathcore.IntegrationResult
PSeries: Final = _mathcore.PSeries
SeriesResult: Final = _mathcore.SeriesResult
TaylorResult: Final = _mathcore.TaylorResult

_SUPPORTED_TAYLOR_FUNCTIONS: Final = frozenset({"exp", "sin", "cos", "ln"})


def _require_number(value: object, name: str) -> None:
    if isinstance(value, bool) or not isinstance(value, (int, float)):
        raise TypeError(f"{name} must be a number")


def _require_integer(value: object, name: str) -> None:
    if isinstance(value, bool) or not isinstance(value, int):
        raise TypeError(f"{name} must be an int")


def _validate_integration_params(
    func: Callable[[float], float], a: float, b: float, n: int
) -> None:
    if not callable(func):
        raise TypeError("func must be callable")
    _require_number(a, "a")
    _require_number(b, "b")
    _require_integer(n, "n")
    require(a <= b, "a must be less than or equal to b")
    require(n > 0, "Number of intervals must be positive")


def _validate_series_params(value: object, name: str, eps: float, n_max: int) -> None:
    _require_number(value, name)
    _require_number(eps, "eps")
    _require_integer(n_max, "n_max")
    require(eps > 0, "eps must be strictly positive (> 0)")
    require(n_max >= 1, "n_max must be at least 1")


def integrate_trapezoidal(func: Callable[[float], float], a: float, b: float, n: int) -> float:
    """Retorna apenas o valor numérico (double) da integração Trapezoidal com contratos DbC."""
    _validate_integration_params(func, a, b, n)
    resultado = _mathcore.integrate_trapezoidal(func, a, b, n)
    ensure(math.isfinite(resultado.value), "Integration result must be finite")
    return resultado.value


def integrate_simpson(func: Callable[[float], float], a: float, b: float, n: int) -> float:
    """Retorna apenas o valor numérico (double) da integração de Simpson com contratos DbC."""
    _validate_integration_params(func, a, b, n)
    require(n % 2 == 0, "Simpson requires an even number of intervals")
    resultado = _mathcore.integrate_simpson(func, a, b, n)
    ensure(math.isfinite(resultado.value), "Integration result must be finite")
    return resultado.value


_INTEGRATION_STRATEGIES: Final[
    dict[str, Callable[[Callable[[float], float], float, float, int], float]]
] = {
    "simpson": integrate_simpson,
    "trapezoidal": integrate_trapezoidal,
}


def integrate(
    func: Callable[[float], float],
    a_or_interval: _mathcore.IntegrationInterval | float,
    b_or_n: float,
    n: int | None = None,
    method: str = "simpson",
) -> float:
    """Integra no intervalo informado usando método numérico selecionado com contratos DbC."""
    strategy = _INTEGRATION_STRATEGIES.get(method)
    require(strategy is not None, "method must be 'simpson' or 'trapezoidal'")
    assert strategy is not None
    if isinstance(a_or_interval, _mathcore.IntegrationInterval):
        _require_integer(b_or_n, "n")
        res = strategy(func, a_or_interval.lower, a_or_interval.upper, int(b_or_n))
        ensure(math.isfinite(res), "Integration result must be finite")
        return res
    if n is None:
        raise TypeError("integrate requires (func, a, b, n, method) or (func, interval, n, method)")
    _require_number(a_or_interval, "a")
    _require_number(b_or_n, "b")
    _require_integer(n, "n")
    res = strategy(func, float(a_or_interval), float(b_or_n), n)
    ensure(math.isfinite(res), "Integration result must be finite")
    return res


def compute_series(
    a: float,
    r: float,
    eps: float = 1e-7,
    n_max: int = 10000,
) -> _mathcore.SeriesResult:
    """Calcula convergência e soma de série geométrica com contratos DbC."""
    _validate_series_params(a, "a", eps, n_max)
    _require_number(r, "r")
    require(abs(r) < 1.0, "Geometric series diverges for |r| >= 1.0")
    resultado = _mathcore.compute_series(a, r, eps, n_max)
    ensure(math.isfinite(resultado.sum), "Series sum must be finite")
    ensure(1 <= resultado.iterations <= n_max, "Iterations must be between 1 and n_max")
    return resultado


def compute_p_series(
    p: float,
    eps: float = 1e-7,
    n_max: int = 10000,
) -> _mathcore.SeriesResult:
    """Calcula convergência e soma aproximada de p-série sum(1 / k^p) com contratos DbC."""
    _validate_series_params(p, "p", eps, n_max)
    require(p > 1.0, "p-series diverges for p <= 1.0")
    resultado = _mathcore.compute_p_series(p, eps, n_max)
    ensure(math.isfinite(resultado.sum), "Series sum must be finite")
    ensure(1 <= resultado.iterations <= n_max, "Iterations must be between 1 and n_max")
    return resultado


def evaluate_polynomial(coefficients: Sequence[float], x: float, center: float = 0.0) -> float:
    """Avalia polinômio em potências de (x - center) via algoritmo de Horner em C++ com contratos DbC."""
    if not isinstance(coefficients, (list, tuple)):
        raise TypeError("coefficients must be a list or tuple")
    _require_number(x, "x")
    _require_number(center, "center")
    if any(isinstance(c, bool) or not isinstance(c, (int, float)) for c in coefficients):
        raise TypeError("coefficients must contain only numbers")
    resultado = _mathcore.evaluate_polynomial(list(coefficients), x, center)
    ensure(math.isfinite(resultado), "Polynomial evaluation must be finite")
    return resultado


def approximate_taylor(
    func_name: str,
    x: float,
    x0: float,
    order: int,
) -> float:
    """Aproxima uma função suportada usando seu polinômio de Taylor com contratos DbC."""
    if not isinstance(func_name, str):
        raise TypeError("func_name must be a str")
    require(func_name in _SUPPORTED_TAYLOR_FUNCTIONS, "func_name must be one of: exp, sin, cos, ln")
    _require_number(x, "x")
    _require_number(x0, "x0")
    _require_integer(order, "order")
    require(0 <= order <= 20, "order must be between 0 and 20")
    if func_name == "ln":
        require(x > 0 and x0 > 0, "ln requires x > 0 and expansion center x0 > 0")
    resultado = _mathcore.approximate_taylor(func_name, x, x0, order)
    ensure(math.isfinite(resultado.value), "Taylor approximation result must be finite")
    return resultado.value


__all__ = [
    "ConvergenceCriteria",
    "GeometricSeries",
    "IntegrationInterval",
    "IntegrationResult",
    "InvariantError",
    "PSeries",
    "PostconditionError",
    "PreconditionError",
    "SeriesResult",
    "TaylorResult",
    "approximate_taylor",
    "check_invariant",
    "compute_p_series",
    "compute_series",
    "ensure",
    "evaluate_polynomial",
    "integrate",
    "integrate_simpson",
    "integrate_trapezoidal",
    "postcondition",
    "precondition",
    "require",
]
