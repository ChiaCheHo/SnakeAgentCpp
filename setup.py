from setuptools import setup, Extension
from Cython.Build import cythonize

extensions = [
    Extension(
        "snake_game",
        sources=["snake_game.pyx", "main.cpp"],  # 只保留手寫的 main.cpp，讓 Cython 處理 snake_game.cpp
        language="c++",  # 指定使用 C++
        extra_compile_args=["-std=c++17"],  # 使用 C++17
        include_dirs=["/path/to/headers"],  # 包含需要的頭文件路徑
        libraries=["SDL2", "SDL2_mixer", "SDL2_ttf"]  # 需要鏈接的 SDL 庫
        # library_dirs=["/path/to/libs"],  # 庫文件路徑
    )
]

setup(
    name="snake_game",
    ext_modules=cythonize(extensions),
)
