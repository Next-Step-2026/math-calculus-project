from mathlab.soma import soma

from .api import (
    IntegrationInterval,
    approximate_taylor,
    compute_p_series,
    compute_series,
    evaluate_polynomial,
    integrate,
    integrate_simpson,
    integrate_trapezoidal,
)

__all__ = [
    "IntegrationInterval",
    "approximate_taylor",
    "compute_p_series",
    "compute_series",
    "evaluate_polynomial",
    "integrate",
    "integrate_simpson",
    "integrate_trapezoidal",
    "soma",
]