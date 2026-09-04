import math

import mathlab
import pytest
from mathlab import _mathcore, _soma_cpp


@pytest.mark.smoke
def test_smoke_cpp_extensions_loaded():
    """Verifies that native C++ shared libraries are loaded with expected symbols."""
    assert hasattr(_soma_cpp, "soma"), "C++ extension _soma_cpp missing 'soma' function"
    for expected_symbol in (
        "integrate_trapezoidal",
        "integrate_simpson",
        "compute_series",
        "compute_p_series",
        "approximate_taylor",
        "evaluate_polynomial",
    ):
        assert hasattr(_mathcore, expected_symbol), (
            f"C++ extension _mathcore missing '{expected_symbol}'"
        )


@pytest.mark.smoke
def test_smoke_soma_module():
    """Module 1 (Soma): Minimal verification of C++ addition via pybind11."""
    assert mathlab.soma(2.0, 3.0) == pytest.approx(5.0)


@pytest.mark.smoke
def test_smoke_integration_module():
    """Module 2 (Integration): Minimal verification of C++ quadrature with Python callbacks."""
    # Integral of f(x) = x from 0 to 2 is 2.0
    trap_res = mathlab.integrate_trapezoidal(lambda x: x, 0.0, 2.0, 50)
    assert trap_res == pytest.approx(2.0, rel=1e-3)

    # Integral of f(x) = x^2 from 0 to 3 is 9.0
    simp_res = mathlab.integrate_simpson(lambda x: x**2, 0.0, 3.0, 50)
    assert simp_res == pytest.approx(9.0, rel=1e-3)


@pytest.mark.smoke
def test_smoke_series_module():
    """Module 3 (Series): Minimal verification of C++ convergence evaluation."""
    # Geometric series: sum_{k=0}^inf (0.5)^k = 1 / (1 - 0.5) = 2.0
    geo_res = mathlab.compute_series(a=1.0, r=0.5, eps=1e-7, n_max=1000)
    assert geo_res.converged is True
    assert geo_res.sum == pytest.approx(2.0, rel=1e-5)

    # p-Series: sum_{k=1}^inf 1 / k^2 converges for p = 2.0
    p_res = mathlab.compute_p_series(p=2.0, eps=1e-5, n_max=1000)
    assert p_res.converged is True
    assert p_res.sum == pytest.approx(math.pi**2 / 6.0, rel=1e-2)


@pytest.mark.smoke
def test_smoke_taylor_module():
    """Module 4 (Taylor): Minimal verification of C++ Horner evaluation and series expansion."""
    # Taylor expansion of exp(1) centered at 0 of order 10
    exp_res = mathlab.approximate_taylor("exp", 1.0, 0.0, 10)
    assert exp_res == pytest.approx(math.e, rel=1e-6)

    # Horner polynomial evaluation: 1 + 2*(2) + 3*(2^2) = 17
    poly_res = mathlab.evaluate_polynomial([1.0, 2.0, 3.0], 2.0, center=0.0)
    assert poly_res == pytest.approx(17.0)
