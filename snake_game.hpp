#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <set>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <algorithm> // For std::find

#include <fstream>

class SnakeGame
{
public:
    SnakeGame(int seed = 0, int board_size = 12, bool silent_mode = true)
        : board_size(board_size), grid_size(board_size * board_size),
          cell_size(40), width(board_size * cell_size), height(board_size * cell_size),
          border_size(20), display_width(width + 2 * border_size),
          display_height(height + 2 * border_size + 40), silent_mode(silent_mode)
    {
        seed_value = seed;
        srand(seed);
        if (!silent_mode)
        {
            SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
            TTF_Init(); // Initialize TTF for font rendering
            window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, display_width, display_height, 0);
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
            font = TTF_OpenFont("./raleway/Raleway-Light.ttf", 8); // You will need to provide a valid font file path

            if (TTF_Init() == -1)
            {
                std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
                exit(1);
            }

            std::ifstream file("./raleway/Raleway-Light.ttf");
            if (!file.good())
            {
                std::cerr << "Font file not found!" << std::endl;
                exit(1);
            }

            // Initialize SDL_mixer and load sound effects
            Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
            sound_eat = Mix_LoadWAV("sound/eat.wav");
            sound_game_over = Mix_LoadWAV("sound/game_over.wav");
            sound_victory = Mix_LoadWAV("sound/victory.wav");
        }
        reset();
    }

    ~SnakeGame()
    {
        if (!silent_mode)
        {
            Mix_FreeChunk(sound_eat);
            Mix_FreeChunk(sound_game_over);
            Mix_FreeChunk(sound_victory);
            TTF_CloseFont(font);
            TTF_Quit();
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
        }
    }

    void reset()
    {
        snake = {
            {board_size / 2, board_size / 2},
            {board_size / 2 + 1, board_size / 2},
            {board_size / 2 + 2, board_size / 2}};
        direction = "UP"; // 修改為 "UP"
        non_snake.clear();
        for (int r = 0; r < board_size; ++r)
        {
            for (int c = 0; c < board_size; ++c)
            {
                if (std::find(snake.begin(), snake.end(), std::make_pair(r, c)) == snake.end())
                {
                    non_snake.insert({r, c});
                }
            }
        }
        score = 0;
        food = generate_food();
    }

    bool step(int action)
    {
        update_direction(action);

        auto [row, col] = snake.front();
        if (direction == "UP")
            --row;
        else if (direction == "DOWN")
            ++row;
        else if (direction == "LEFT")
            --col;
        else if (direction == "RIGHT")
            ++col;

        bool food_obtained = false;

        if (std::make_pair(row, col) == food)
        {
            food_obtained = true;
            score += 10;
            if (!silent_mode)
                Mix_PlayChannel(-1, sound_eat, 0);
        }
        else
        {
            non_snake.insert(snake.back());
            snake.pop_back();
        }

        // 檢查碰撞，跳過與蛇頭的比較
        bool done = row < 0 || row >= board_size || col < 0 || col >= board_size ||
                    (std::find(snake.begin() + 1, snake.end(), std::make_pair(row, col)) != snake.end());

        // 調試輸出
        // std::cout << "New Head Position: (" << row << ", " << col << "), Collision: " << done << std::endl;

        if (!done)
        {
            snake.insert(snake.begin(), {row, col});
            non_snake.erase({row, col});
        }
        else
        {
            if (!silent_mode)
                Mix_PlayChannel(-1, sound_game_over, 0);
            return true;
        }

        if (food_obtained)
        {
            food = generate_food();
        }

        render();
        return false;
    }

    void render()
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw snake
        for (auto [r, c] : snake)
        {
            SDL_Rect rect = {border_size + c * cell_size, border_size + r * cell_size, cell_size, cell_size};
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderFillRect(renderer, &rect);
        }

        // Draw food
        if (snake.size() < grid_size)
        {
            auto [r, c] = food;
            SDL_Rect rect = {border_size + c * cell_size, border_size + r * cell_size, cell_size, cell_size};
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &rect);
        }

        // Present everything
        SDL_RenderPresent(renderer);
    }

    bool display_game_over_menu()
    {
        SDL_Event event;
        bool menu_running = true;
        int selected_option = 0; // 0 = Restart, 1 = Exit

        // 渲染器和字體加載都應該已經在其他地方初始化成功
        if (!font)
        {
            std::cerr << "Font is not initialized!" << std::endl;
            return false;
        }

        // 創建 "Game Over" 標題的字體表面與 texture
        SDL_Color white = {255, 255, 255}; // 白色
        SDL_Surface *gameOverSurface = TTF_RenderText_Solid(font, "Game Over", white);
        if (!gameOverSurface)
        {
            std::cerr << "Failed to create surface for 'Game Over': " << TTF_GetError() << std::endl;
            return false;
        }
        SDL_Texture *gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
        SDL_FreeSurface(gameOverSurface); // 釋放表面
        if (!gameOverTexture)
        {
            std::cerr << "Failed to create texture for 'Game Over': " << SDL_GetError() << std::endl;
            return false;
        }

        // 創建選項 "Continue" 和 "Exit" 的字體表面與 textures
        const char *options[2] = {"Continue", "Exit"};
        SDL_Texture *optionTextures[2];
        SDL_Texture *highlightedOptionTextures[2];
        SDL_Rect optionRects[2];

        for (int i = 0; i < 2; ++i)
        {
            // 普通狀態
            SDL_Surface *optionSurface = TTF_RenderText_Solid(font, options[i], white);
            if (!optionSurface)
            {
                std::cerr << "Failed to create surface for option: " << options[i] << " - " << TTF_GetError() << std::endl;
                return false;
            }
            optionTextures[i] = SDL_CreateTextureFromSurface(renderer, optionSurface);
            SDL_FreeSurface(optionSurface); // 釋放表面
            if (!optionTextures[i])
            {
                std::cerr << "Failed to create texture for option: " << options[i] << " - " << SDL_GetError() << std::endl;
                return false;
            }

            // 高亮狀態
            SDL_Surface *highlightedSurface = TTF_RenderText_Solid(font, options[i], SDL_Color{255, 0, 0}); // 紅色
            if (!highlightedSurface)
            {
                std::cerr << "Failed to create surface for highlighted option: " << options[i] << " - " << TTF_GetError() << std::endl;
                return false;
            }
            highlightedOptionTextures[i] = SDL_CreateTextureFromSurface(renderer, highlightedSurface);
            SDL_FreeSurface(highlightedSurface); // 釋放表面
            if (!highlightedOptionTextures[i])
            {
                std::cerr << "Failed to create texture for highlighted option: " << options[i] << " - " << SDL_GetError() << std::endl;
                return false;
            }

            // 定義選項的位置和大小
            optionRects[i] = {display_width / 2 - 150, display_height / 2 + 50 + i * 60, 300, 50};
        }

        // 主循環
        while (menu_running)
        {
            // 處理事件
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    return false; // 玩家點擊窗口的關閉按鈕
                }
                if (event.type == SDL_KEYDOWN)
                {
                    switch (event.key.keysym.sym)
                    {
                    case SDLK_UP:
                    case SDLK_DOWN:
                        selected_option = 1 - selected_option; // 切換選項
                        break;
                    case SDLK_RETURN: // 確定選擇
                        if (selected_option == 0)
                        {
                            // 重新開始遊戲
                            return true;
                        }
                        else
                        {
                            // 退出遊戲
                            return false;
                        }
                    }
                }
            }

            // 清除畫面
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // 黑色背景
            SDL_RenderClear(renderer);

            // 渲染 "Game Over" 標題
            SDL_Rect gameOverRect = {display_width / 2 - 150, display_height / 2 - 100, 300, 60};
            SDL_RenderCopy(renderer, gameOverTexture, nullptr, &gameOverRect);

            // 渲染選項
            for (int i = 0; i < 2; ++i)
            {
                SDL_Texture *currentTexture = (i == selected_option) ? highlightedOptionTextures[i] : optionTextures[i];
                SDL_RenderCopy(renderer, currentTexture, nullptr, &optionRects[i]);
            }

            // 更新畫面
            SDL_RenderPresent(renderer);

            // 減少 CPU 負載，避免過度刷新
            SDL_Delay(100);
        }

        // 清理資源
        SDL_DestroyTexture(gameOverTexture);
        for (int i = 0; i < 2; ++i)
        {
            SDL_DestroyTexture(optionTextures[i]);
            SDL_DestroyTexture(highlightedOptionTextures[i]);
        }

        return false;
    }

    void gameStart()
    {
        bool running = true;
        SDL_Event event;
        int action = -1;

        while (running)
        {
            // game.reset(); // 初始化遊戲
            this->reset();

            bool game_over = false;
            while (!game_over)
            {
                while (SDL_PollEvent(&event))
                {
                    if (event.type == SDL_QUIT)
                    {
                        running = false;
                        game_over = true;
                    }
                    if (event.type == SDL_KEYDOWN)
                    {
                        switch (event.key.keysym.sym)
                        {
                        case SDLK_UP:
                            action = 0;
                            break;
                        case SDLK_DOWN:
                            action = 3;
                            break;
                        case SDLK_LEFT:
                            action = 1;
                            break;
                        case SDLK_RIGHT:
                            action = 2;
                            break;
                        case SDLK_q:
                            running = false;
                            game_over = true;
                            break;
                        }
                    }
                }

                // 如果 step() 返回 true，表示遊戲結束
                // if (game.step(action))
                if (this->step(action))
                {
                    game_over = true; // 遊戲結束
                }

                SDL_Delay(150); // 控制遊戲速度
            }

            // 遊戲結束後，顯示選單
            // if (!game.display_game_over_menu()) // 顯示遊戲結束選單
            if (!this->display_game_over_menu())
            {
                running = false; // 如果選擇退出遊戲
            }
        }
    }

private:
    int board_size, grid_size, cell_size, width, height, border_size, display_width, display_height, score;
    std::vector<std::pair<int, int>> snake;
    std::set<std::pair<int, int>> non_snake;
    std::pair<int, int> food;
    std::string direction;
    bool silent_mode;
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    Mix_Chunk *sound_eat = nullptr;
    Mix_Chunk *sound_game_over = nullptr;
    Mix_Chunk *sound_victory = nullptr;
    TTF_Font *font = nullptr;
    int seed_value;

    void update_direction(int action)
    {
        if (action == 0 && direction != "DOWN")
            direction = "UP";
        else if (action == 1 && direction != "RIGHT")
            direction = "LEFT";
        else if (action == 2 && direction != "LEFT")
            direction = "RIGHT";
        else if (action == 3 && direction != "UP")
            direction = "DOWN";
    }

    std::pair<int, int> generate_food()
    {
        if (!non_snake.empty())
        {
            auto it = non_snake.begin();
            std::advance(it, rand() % non_snake.size());
            return *it;
        }
        else
        {
            return {0, 0};
        }
    }
};
