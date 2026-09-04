from collections.abc import Callable, Sequence

class ConvergenceCriteria:
    eps: float
    n_max: int
    def __init__(self, eps: float = 1e-7, n_max: int = 10000) -> None: ...

class GeometricSeries:
    a: float
    r: float
    def __init__(self, a: float = 1.0, r: float = 0.5) -> None: ...

class PSeries:
    p: float
    def __init__(self, p: float = 2.0) -> None: ...

class SeriesResult:
    converged: bool
    sum: float
    iterations: int

def compute_series(a: float, r: float, eps: float = 1e-7, n_max: int = 10000) -> SeriesResult: ...
def compute_p_series(p: float, eps: float = 1e-7, n_max: int = 10000) -> SeriesResult: ...

class IntegrationInterval:
    def __init__(self, a: float, b: float) -> None: ...
    @property
    def lower(self) -> float: ...
    @property
    def upper(self) -> float: ...
    @property
    def width(self) -> float: ...
    @property
    def is_degenerate(self) -> bool: ...

class IntegrationResult:
    value: float
    intervals: int
    method: str

def integrate_trapezoidal(
    f: Callable[[float], float], a: float, b: float, n: int
) -> IntegrationResult: ...
def integrate_simpson(
    f: Callable[[float], float], a: float, b: float, n: int
) -> IntegrationResult: ...

class TaylorResult:
    value: float
    order: int
    center: float

def approximate_taylor(function_name: str, x: float, x0: float, order: int) -> TaylorResult: ...
def evaluate_polynomial(coefficients: Sequence[float], x: float, center: float = 0.0) -> float: ...
