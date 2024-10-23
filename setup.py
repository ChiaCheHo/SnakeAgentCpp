# setup.py
from setuptools import setup, Extension
from Cython.Build import cythonize

extensions = [
    Extension(
        "snake_game",
        sources=["snake_game.pyx", "snake_game.cpp"],
        language="c++",
        include_dirs=["/path/to/sdl/include", "/path/to/sdl_mixer/include", "/path/to/sdl_ttf/include"],
        libraries=["SDL2", "SDL2_mixer", "SDL2_ttf"],
        library_dirs=["/path/to/sdl/lib", "/path/to/sdl_mixer/lib", "/path/to/sdl_ttf/lib"],
    )
]

setup(
    name="snake_game",
    ext_modules=cythonize(extensions),
)
