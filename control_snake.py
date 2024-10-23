# control_snake.py
import snake_game

game = snake_game.PySnakeGame(seed=0, board_size=12, silent_mode=False)

game.game_start()

while True:
    action = int(input("Enter action (0: UP, 1: LEFT, 2: RIGHT, 3: DOWN): "))
    game_over = game.step(action)
    if game_over:
        print("Game Over!")
        break
