import mathlab
import pytest


def test_soma():
    assert mathlab.soma(5, 10) == 15.0
    assert mathlab.soma(-3, -7) == -10.0
    assert mathlab.soma(2.5, 3.2) == pytest.approx(5.7)
    assert mathlab.soma(0.0, 42.0) == 42.0
    assert mathlab.soma(42.0, 0.0) == 42.0


def test_soma_contracts_preconditions():
    """Verify that Boost.Contract preconditions reject non-finite inputs."""
    with pytest.raises(ValueError, match="std::isfinite"):
        mathlab.soma(float("nan"), 1.0)

    with pytest.raises(ValueError, match="std::isfinite"):
        mathlab.soma(1.0, float("nan"))

    with pytest.raises(ValueError, match="std::isfinite"):
        mathlab.soma(float("inf"), 2.0)

    with pytest.raises(ValueError, match="std::isfinite"):
        mathlab.soma(2.0, float("-inf"))
