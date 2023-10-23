#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

SDL_Rect CreateRect(int x, int y, int w, int h);
void DrawRect(int r, int g, int b, int a, SDL_Renderer* renderer, SDL_Rect rect);
bool AABB(SDL_Rect a, SDL_Rect b);
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

    std::tuple<int, int> xy = GenerateRandomXYPosition(WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_Rect snake = CreateRect(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 25, 25);
    SDL_Rect apple = CreateRect(std::get<0>(xy), std::get<1>(xy), 25, 25);
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
                        yvelocity = -1;
                        xvelocity = 0;
                        break;
                    case SDLK_d:
                        yvelocity = 0;
                        xvelocity = 1;
                        break;
                    case SDLK_s:
                        yvelocity = 1;
                        xvelocity = 0;
                        break;
                    case SDLK_a:
                        yvelocity = 0;
                        xvelocity = -1;
                        break;
                }
            }
        }
        snake.x += xvelocity * speed;
        snake.y += yvelocity * speed;

        if(snake.x + snake.w >= WINDOW_WIDTH){
            snake.x = WINDOW_WIDTH - snake.w;
        }
        if(snake.x <= 0){
            snake.x = 0;
        }
        if(snake.y + snake.h >= WINDOW_HEIGHT){
            snake.y = WINDOW_HEIGHT - snake.h;
        }
        if(snake.y <= 0){
            snake.y = 0;
        }
        if(AABB(snake, apple)){
            xy = GenerateRandomXYPosition(WINDOW_WIDTH, WINDOW_HEIGHT);
            apple.x = std::get<0>(xy);
            apple.y = std::get<1>(xy);
        }

        SDL_RenderClear(renderer);
        DrawRect(0, 255, 0, 255, renderer, snake);
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

SDL_Rect CreateRect(int x, int y, int w, int h){
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    return rect;
}

void DrawRect(int r, int g, int b, int a, SDL_Renderer* renderer, SDL_Rect rect){
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        SDL_RenderDrawRect(renderer, &rect);
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

bool AABB(SDL_Rect a, SDL_Rect b){
    if(
        a.x + a.w >= b.x &&
        b.x + b.w >= a.x &&
        a.y + a.h >= b.y &&
        b.y + b.h >= a.y 
    ){
        return true;
    }else{
        return false;
    }
}

std::tuple<int, int> GenerateRandomXYPosition(int window_width, int window_height){
    int x = (std::rand() % window_width) / 25;
    int y = (std::rand() % window_height) / 25;
    x = x * 25;
    y = y * 25;
    return {x, y};
}