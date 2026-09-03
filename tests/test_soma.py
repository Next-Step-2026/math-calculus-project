import mathlab
import pytest


def test_soma():
    assert mathlab.soma(5, 10) == 15.0
    assert mathlab.soma(-3, -7) == -10.0
    assert mathlab.soma(2.5, 3.2) == pytest.approx(5.7)