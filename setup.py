from distutils.core import setup, Extension

module = Extension("simple_graphs", sources = ["simple_graphs.cpp"])

setup(name="simple_graphs",
    version="0.01",
    description="Graph module for C",
    ext_modules=[module])