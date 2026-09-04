from collections.abc import Callable, Sequence
from typing import Final

from . import _mathcore

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
    if a > b:
        raise ValueError("a must be less than or equal to b")
    if n <= 0:
        raise ValueError("Number of intervals must be positive")


def _validate_series_params(value: object, name: str, eps: float, n_max: int) -> None:
    _require_number(value, name)
    _require_number(eps, "eps")
    _require_integer(n_max, "n_max")
    if eps <= 0:
        raise ValueError("eps must be strictly positive (> 0)")
    if n_max < 1:
        raise ValueError("n_max must be at least 1")


def integrate_trapezoidal(func: Callable[[float], float], a: float, b: float, n: int) -> float:
    """Retorna apenas o valor numérico (double) da integração Trapezoidal."""
    _validate_integration_params(func, a, b, n)
    resultado = _mathcore.integrate_trapezoidal(func, a, b, n)
    return resultado.value


def integrate_simpson(func: Callable[[float], float], a: float, b: float, n: int) -> float:
    """Retorna apenas o valor numérico (double) da integração de Simpson."""
    _validate_integration_params(func, a, b, n)
    if n % 2 != 0:
        raise ValueError("Simpson requires an even number of intervals")
    resultado = _mathcore.integrate_simpson(func, a, b, n)
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
    """Integra no intervalo informado usando método numérico selecionado."""
    strategy = _INTEGRATION_STRATEGIES.get(method)
    if strategy is None:
        raise ValueError("method must be 'simpson' or 'trapezoidal'")
    if isinstance(a_or_interval, _mathcore.IntegrationInterval):
        _require_integer(b_or_n, "n")
        return strategy(func, a_or_interval.lower, a_or_interval.upper, int(b_or_n))
    if n is None:
        raise TypeError("integrate requires (func, a, b, n, method) or (func, interval, n, method)")
    _require_number(a_or_interval, "a")
    _require_number(b_or_n, "b")
    _require_integer(n, "n")
    return strategy(func, float(a_or_interval), float(b_or_n), n)


def compute_series(
    a: float,
    r: float,
    eps: float = 1e-7,
    n_max: int = 10000,
) -> _mathcore.SeriesResult:
    """Calcula convergência e soma de série geométrica com guard clauses defensivas."""
    _validate_series_params(a, "a", eps, n_max)
    _require_number(r, "r")
    if abs(r) >= 1.0:
        raise ValueError("Geometric series diverges for |r| >= 1.0")
    return _mathcore.compute_series(a, r, eps, n_max)


def compute_p_series(
    p: float,
    eps: float = 1e-7,
    n_max: int = 10000,
) -> _mathcore.SeriesResult:
    """Calcula convergência e soma aproximada de p-série sum(1 / k^p)."""
    _validate_series_params(p, "p", eps, n_max)
    if p <= 1.0:
        raise ValueError("p-series diverges for p <= 1.0")
    return _mathcore.compute_p_series(p, eps, n_max)


def evaluate_polynomial(coefficients: Sequence[float], x: float, center: float = 0.0) -> float:
    """Avalia polinômio em potências de (x - center) via algoritmo de Horner em C++."""
    if not isinstance(coefficients, (list, tuple)):
        raise TypeError("coefficients must be a list or tuple")
    _require_number(x, "x")
    _require_number(center, "center")
    if any(isinstance(c, bool) or not isinstance(c, (int, float)) for c in coefficients):
        raise TypeError("coefficients must contain only numbers")
    return _mathcore.evaluate_polynomial(list(coefficients), x, center)


def approximate_taylor(
    func_name: str,
    x: float,
    x0: float,
    order: int,
) -> float:
    """Aproxima uma função suportada usando seu polinômio de Taylor."""
    if not isinstance(func_name, str):
        raise TypeError("func_name must be a str")
    if func_name not in _SUPPORTED_TAYLOR_FUNCTIONS:
        raise ValueError("func_name must be one of: exp, sin, cos, ln")
    _require_number(x, "x")
    _require_number(x0, "x0")
    _require_integer(order, "order")
    if order < 0 or order > 20:
        raise ValueError("order must be between 0 and 20")
    if func_name == "ln" and (x <= 0 or x0 <= 0):
        raise ValueError("ln requires x > 0 and expansion center x0 > 0")
    resultado = _mathcore.approximate_taylor(func_name, x, x0, order)
    return resultado.value


__all__ = [
    "ConvergenceCriteria",
    "GeometricSeries",
    "IntegrationInterval",
    "IntegrationResult",
    "PSeries",
    "SeriesResult",
    "TaylorResult",
    "approximate_taylor",
    "compute_p_series",
    "compute_series",
    "evaluate_polynomial",
    "integrate",
    "integrate_simpson",
    "integrate_trapezoidal",
]
