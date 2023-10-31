#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

SDL_Rect CreateRect(int x, int y, int w, int h);
bool AABB(SDL_Rect a, SDL_Rect b);

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

    SDL_Rect player = CreateRect(100, WINDOW_HEIGHT - 32, 32, 32);
    int velocity = 1;
    int speed = 5;

    SDL_Rect enemy = CreateRect(WINDOW_WIDTH, WINDOW_HEIGHT - 32, 32, 32);

    SDL_Event event;
    bool gameRunning = true;

    while(gameRunning){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                gameRunning = false;
            }

            if(event.type == SDL_KEYDOWN){
                switch(event.key.keysym.sym){
                    case SDLK_SPACE:
                        velocity = -1;
                        break;
                }
            }

            if(event.type == SDL_KEYUP){
                switch(event.key.keysym.sym){
                    case SDLK_SPACE:
                        velocity = 1;
                        break;
                }
            }

        }

        player.y += velocity * speed;
        enemy.x -= 5;

        if(AABB(player, enemy)){
            std::cout << "HIT" << "\n";
        }

        if(player.y  + player.h >= WINDOW_HEIGHT){
            player.y = WINDOW_HEIGHT - player.h;
        }

        if(enemy.x < 0){
            enemy.x = WINDOW_WIDTH;
        }

        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &player);
        SDL_RenderFillRect(renderer, &player);
        SDL_RenderDrawRect(renderer, &enemy);
        SDL_RenderFillRect(renderer, &enemy);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderPresent(renderer);
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