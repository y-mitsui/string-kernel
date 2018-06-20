from distutils.core import setup, Extension
from Cython.Build import cythonize
import os

ext = Extension("string_kernel",
                extra_compile_args=["-std=c99"],
                sources=["string_kernel.pyx", "ssk_kernel.c", "ssk_kernel_utf8.c","ssk_kernel_utf8_multi.c"],
                )

setup(name = 'string_kernel', ext_modules = cythonize([ext]))

