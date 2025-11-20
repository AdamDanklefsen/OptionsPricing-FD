from setuptools import setup
from pybind11.setup_helpers import Pybind11Extension, build_ext

ext_modules = [
    Pybind11Extension(
        "cpphello",
        ["hello.cpp"],
    ),
]

setup(
    name="cpphello",
    version="0.0.1",
    description="A tiny example showing pybind11 on Windows",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
)