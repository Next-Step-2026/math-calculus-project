from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import find_packages, setup

ext_modules = [
    Pybind11Extension(
        "mathlab._soma_cpp",
        ["cpp/src/soma.cpp"]
    )
]

setup(
    name="mathlab",
    version="0.1.0",
    package_dir={"": "python"},
    packages=find_packages(where="python"),
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
)
