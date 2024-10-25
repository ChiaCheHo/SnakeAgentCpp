# snake_game.pyx
cdef extern from "snake_game.hpp":
    cdef cppclass SnakeGame:
        SnakeGame(int seed, int board_size, bint silent_mode)  # bint 替代 bool
        bint python_step(int action)  # bint 替代 bool
        void python_game_start()

cdef class PySnakeGame:
    cdef SnakeGame* game

    def __cinit__(self, int seed, int board_size, bint silent_mode):  # 替換 bool 為 bint
        self.game = new SnakeGame(seed, board_size, silent_mode)

    def game_start(self):
        self.game.python_game_start()

    def step(self, int action):
        return self.game.python_step(action)
