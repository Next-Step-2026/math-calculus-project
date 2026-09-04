import mathlab
import pytest


def test_geometric_series_convergence_basic():
    # S = a / (1 - r) = 1.0 / (1 - 0.5) = 2.0
    res = mathlab.compute_series(a=1.0, r=0.5, eps=1e-8, n_max=1000)
    assert res.converged is True
    assert res.sum == pytest.approx(2.0, rel=1e-6)
    assert res.iterations > 1


def test_geometric_series_with_alternating_ratio():
    # S = 1.0 / (1 - (-0.5)) = 1.0 / 1.5 = 2/3 ≈ 0.666667
    res = mathlab.compute_series(a=1.0, r=-0.5, eps=1e-8, n_max=1000)
    assert res.converged is True
    assert res.sum == pytest.approx(2.0 / 3.0, rel=1e-6)


def test_geometric_series_analytical_ratio_one_third():
    # S = a / (1 - r) = 3.0 / (1 - 1/3) = 3.0 / (2/3) = 4.5
    res = mathlab.compute_series(a=3.0, r=1.0 / 3.0, eps=1e-10, n_max=2000)
    assert res.converged is True
    assert res.sum == pytest.approx(4.5, rel=1e-8)


def test_series_fine_tolerance():
    # Tolerância fina de eps = 1e-10
    res = mathlab.compute_series(a=1.0, r=0.5, eps=1e-10, n_max=2000)
    assert res.converged is True
    assert res.sum == pytest.approx(2.0, rel=1e-9)
    assert res.iterations > 20


def test_series_edge_n_max_one():
    # n_max = 1 deve retornar apenas o primeiro termo
    res = mathlab.compute_series(a=3.5, r=0.2, eps=1e-8, n_max=1)
    assert res.sum == pytest.approx(3.5)
    assert res.iterations == 1


def test_series_invalid_eps_throws_value_error():
    with pytest.raises(ValueError, match="eps must be strictly positive"):
        mathlab.compute_series(a=1.0, r=0.5, eps=0.0)

    with pytest.raises(ValueError, match="eps must be strictly positive"):
        mathlab.compute_series(a=1.0, r=0.5, eps=-1e-5)


def test_series_invalid_n_max_throws_value_error():
    with pytest.raises(ValueError, match="n_max must be at least 1"):
        mathlab.compute_series(a=1.0, r=0.5, n_max=0)


def test_series_divergence_throws_value_error():
    with pytest.raises(ValueError, match="diverges"):
        mathlab.compute_series(a=1.0, r=1.0)

    with pytest.raises(ValueError, match="diverges"):
        mathlab.compute_series(a=1.0, r=1.5)

    with pytest.raises(ValueError, match="diverges"):
        mathlab.compute_series(a=1.0, r=-1.0)


def test_p_series_convergence_p_two():
    # sum(1 / k^2) converge para pi^2 / 6 ≈ 1.644934
    res = mathlab.compute_p_series(p=2.0, eps=1e-5, n_max=10000)
    assert res.converged is True
    assert res.sum == pytest.approx(1.644934, rel=1e-2)
    assert res.iterations > 10


def test_p_series_divergence_p_one():
    # Série harmônica (p=1.0) diverge
    with pytest.raises(ValueError, match="diverges for p <= 1.0"):
        mathlab.compute_p_series(p=1.0)


def test_p_series_divergence_p_less_than_one():
    with pytest.raises(ValueError, match="diverges for p <= 1.0"):
        mathlab.compute_p_series(p=0.5)


def test_p_series_invalid_params():
    with pytest.raises(ValueError, match="eps must be strictly positive"):
        mathlab.compute_p_series(p=2.0, eps=0.0)

    with pytest.raises(ValueError, match="n_max must be at least 1"):
        mathlab.compute_p_series(p=2.0, n_max=0)
