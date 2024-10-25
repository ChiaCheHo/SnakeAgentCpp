# control_snake.py
import snake_game
import sys

# 構造 PySnakeGame 對象，注意保持 silent_mode=False
game = snake_game.PySnakeGame(seed=0, board_size=12, silent_mode=False)

# 啟動遊戲，根據返回值來決定是否退出
if not game.game_start():
    print("Exiting game...")
    del game
    sys.exit(0)

try:
    # 開始遊戲輸入循環
    while True:
        action = input("Enter action (0: UP, 1: LEFT, 2: RIGHT, 3: DOWN): ")

        # 確保輸入的是有效數字
        if action.isdigit():
            action = int(action)
            if action not in [0, 1, 2, 3]:
                print("Invalid action. Please enter 0, 1, 2, or 3.")
                continue
        else:
            print("Invalid input. Please enter a number.")
            continue

        game_over = game.step(action)
        if game_over:
            print("Game Over!")
            break

except KeyboardInterrupt:
    print("Exiting game...")
finally:
    # 在遊戲結束後，確保正確釋放資源並退出
    del game
    print("Game resources have been released.")
