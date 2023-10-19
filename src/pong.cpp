#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

int main(int argc, char* argv[]){

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        std::cout << "ERROR! SDL COULD NOT BE INITIALIZED: " << SDL_GetError() << "\n";
    }

    if(TTF_Init() == -1){
        std::cout << "ERROR! SDL TTF COULD NOT BE INITIALIZED: " << SDL_GetError() << "\n";
    }

    const int WINDOW_HEIGHT = 600;
    const int WINDOW_WIDTH = 800;

    SDL_Window* window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_Rect player;
    player.x = 10;
    player.y = WINDOW_HEIGHT / 2;
    player.w = 25;
    player.h = 100;

    SDL_Rect player2;
    player2.w = 25;
    player2.h = 100;
    player2.x = WINDOW_WIDTH - player2.w - 10;
    player2.y = WINDOW_HEIGHT / 2;

    bool gameRunning = true;
    SDL_Event event;

    while(gameRunning){

        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                gameRunning = false;
            }
        }

        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &player);
        SDL_RenderFillRect(renderer, &player);
        SDL_RenderDrawRect(renderer, &player2);
        SDL_RenderFillRect(renderer, &player2);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderPresent(renderer);

    }

    SDL_RenderClear(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();


    return 0;
}