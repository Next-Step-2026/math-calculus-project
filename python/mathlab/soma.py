from ._soma_cpp import soma as _cpp_soma  # type: ignore


def soma(a: float, b: float) -> float:
    """Adds two numbers using the underlying C++ pybind11 extension."""
    return _cpp_soma(a, b)
