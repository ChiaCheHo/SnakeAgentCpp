# snake_game.pyx
cdef extern from "snake_game.h":
    cdef cppclass SnakeGame:
        SnakeGame(int seed, int board_size, bool silent_mode)
        bool python_step(int action)
        void python_game_start()

cdef class PySnakeGame:
    cdef SnakeGame* game

    def __cinit__(self, int seed, int board_size, bool silent_mode):
        self.game = new SnakeGame(seed, board_size, silent_mode)

    def game_start(self):
        self.game.python_game_start()

    def step(self, int action):
        return self.game.python_step(action)
