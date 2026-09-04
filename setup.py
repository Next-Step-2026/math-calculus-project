import sys
from pathlib import Path

from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import find_packages, setup

# Ensure Boost is available locally if missing
boost_contract = (
    Path(__file__).resolve().parent / "cpp" / "include" / "boost" / "contract.hpp"
)
if not boost_contract.exists():
    try:
        from scripts.fetch_boost import install_boost

        install_boost()
    except (OSError, RuntimeError, ImportError) as exc:
        print(f"Warning: Could not auto-fetch boost: {exc}", file=sys.stderr)

ext_modules = [
    Pybind11Extension(
        "mathlab._soma_cpp",
        ["cpp/src/soma.cpp"],
        include_dirs=["cpp", "cpp/include"],
        cxx_std=17,
    ),
    Pybind11Extension(
        "mathlab._mathcore",
        sources=[
            "cpp/bindings.cpp",
            "cpp/src/integration.cpp",
            "cpp/src/series.cpp",
            "cpp/src/taylor.cpp",
        ],
        include_dirs=["cpp", "cpp/include"],
        cxx_std=17,
    ),
]

setup(
    name="mathlab",
    version="0.1.0",
    description="Biblioteca de cálculo numérico com núcleo em C++ e fachada Python",
    python_requires=">=3.10",
    install_requires=["numpy>=1.20.0"],
    package_dir={"": "python"},
    packages=find_packages(where="python", include=["mathlab", "mathlab.*"]),
    include_package_data=True,
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
)
