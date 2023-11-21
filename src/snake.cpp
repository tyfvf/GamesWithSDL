#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

void DrawRect(int r, int g, int b, int a, SDL_Renderer* renderer, SDL_Rect rect);
std::tuple<int, int> GenerateRandomXYPosition(int window_width, int window_height);

int main(int argc, char* argv[]){
    
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cout << "ERROR! SDL COULD NOT BE INITIALIZED " << SDL_GetError() << "\n";
    }

    if(TTF_Init() == -1){
        std::cout << "ERROR! SDL TTF COULD NOT BE INITIALIZED " << SDL_GetError() << "\n";
    }

    int WINDOW_HEIGHT = 600;
    int WINDOW_WIDTH = 400;

    SDL_Window* window = SDL_CreateWindow("Snake game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    std::vector <SDL_Rect> snake = {SDL_Rect{0, 0, 25, 25}};

    std::tuple<int, int> xy = GenerateRandomXYPosition(WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_Rect apple {std::get<0>(xy), std::get<1>(xy), 25, 25};
    int xvelocity = 0;
    int yvelocity = 0;
    int speed = 25;

    SDL_Event event;
    bool gameRunning = true;

    Uint64 start;
    Uint64 end;
    float elapsedMS;

    while(gameRunning){

        start = SDL_GetPerformanceCounter();

        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                gameRunning = false;
            }

            if(event.type == SDL_KEYDOWN){
                switch(event.key.keysym.sym){
                    case SDLK_w:
                        if(yvelocity != 1){
                            yvelocity = -1;
                            xvelocity = 0;
                        } 
                        break;
                    case SDLK_d:
                        if(xvelocity != -1){
                            yvelocity = 0;
                            xvelocity = 1;
                        } 
                        break;
                    case SDLK_s:
                        if(yvelocity != -1){
                            yvelocity = 1;
                            xvelocity = 0;
                        }  
                        break;
                    case SDLK_a:
                        if(xvelocity != 1){
                            yvelocity = 0;
                            xvelocity = -1;
                        } 
                        break;
                }
            }
        }
        
        for(uint16_t i = snake.size(); i > 0; i--){
            snake[i].x = snake[i-1].x;
            snake[i].y = snake[i-1].y;
        }
        snake[0].x += xvelocity * speed; 
        snake[0].y += yvelocity * speed;

        if(snake[0].x + snake[0].w > WINDOW_WIDTH){
            snake[0].x = 0;
        }
        if(snake[0].x < 0){
            snake[0].x = WINDOW_WIDTH;
        }
        if(snake[0].y + snake[0].h > WINDOW_HEIGHT){
            snake[0].y = 0;
        }
        if(snake[0].y < 0){
            snake[0].y = WINDOW_HEIGHT;
        }
        if(snake[0].x == apple.x && snake[0].y == apple.y){
            xy = GenerateRandomXYPosition(WINDOW_WIDTH, WINDOW_HEIGHT);
            apple.x = std::get<0>(xy);
            apple.y = std::get<1>(xy);
            snake.emplace_back(SDL_Rect{-30, -30, 25, 25});
        }
        for(uint16_t i = 0; i < snake.size(); i++){
            if(i == 0){
                continue;
            }

            if(snake[0].x == snake[i].x && snake[0].y == snake[i].y){
                gameRunning = false;
            }
        }

        SDL_RenderClear(renderer);
        for(SDL_Rect piece : snake){
            DrawRect(0, 255, 0, 255, renderer, piece);
        }
        DrawRect(255, 0, 0, 255, renderer, apple);
        SDL_RenderPresent(renderer);


        end = SDL_GetPerformanceCounter();
        elapsedMS = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
        SDL_Delay(100 - elapsedMS);
    }

    SDL_RenderClear(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    
    return 0;
}

void DrawRect(int r, int g, int b, int a, SDL_Renderer* renderer, SDL_Rect rect){
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderDrawRect(renderer, &rect);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

std::tuple<int, int> GenerateRandomXYPosition(int window_width, int window_height){
    int x = (std::rand() % window_width) / 25;
    int y = (std::rand() % window_height) / 25;
    x = x * 25;
    y = y * 25;
    return {x, y};
}