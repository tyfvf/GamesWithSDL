#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

int main(int argc, char* argv[]){

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        std::cout << "SDL could not be initialized " << SDL_GetError() << "\n";
    }

    if(TTF_Init() == -1){
        std::cout << "SDL TTF could not be initialized " << SDL_GetError() << "\n";
    }

    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;
    
    SDL_Window* window = SDL_CreateWindow("Runner", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_Event event;
    bool gameRunning = true;

    while(gameRunning){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                gameRunning = false;
            }

        }

        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_RenderClear(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}