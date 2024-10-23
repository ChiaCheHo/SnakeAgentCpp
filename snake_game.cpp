#include "snake_game.hpp"

int main(int argc, char *argv[])
{
    SnakeGame game(rand(), 12, false);
    game.gameStart();
    return 0;
}
