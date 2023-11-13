#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

void Draw_Rect(SDL_Renderer* renderer, SDL_Rect rect, int r, int g, int b, int a);

int main(int argc, char* argv[]){

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        std::cerr << "SDL could not be initialized. Error: " << SDL_GetError() << "\n";
        return 1;
    }

    const int WINDOW_WIDTH = 300;
    const int WINDOW_HEIGHT = 600;
    bool game_running = true;
    int player_speed = 4;
    int player_velocity = 1;
    SDL_Event event;

    SDL_Window* window = SDL_CreateWindow("Flappy Bird", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Rect player {10, 10, 32, 32};
    SDL_Rect up_pipe {200, 0, 100, 200};
    SDL_Rect down_pipe {200, 400, 100, 200};

    while(game_running){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                game_running = false;
            }

            if(event.type == SDL_KEYDOWN){
                switch(event.key.keysym.sym){
                    case SDLK_SPACE:
                        player_velocity = -1;
                        player_speed = 5;
                }
            }

            if(event.type == SDL_KEYUP){
                switch(event.key.keysym.sym){
                    case SDLK_SPACE:
                        player_velocity = 1;
                        player_speed = 4;
                }
            }
        }

        player.y += player_speed * player_velocity;
        up_pipe.x -= 2;
        down_pipe.x -= 2;

        if(player.y + player.h >= WINDOW_HEIGHT){
            player.y = WINDOW_HEIGHT - player.h;
        }

        if(up_pipe.x + up_pipe.w <= 0 && down_pipe.x + down_pipe.w <= 0){
            up_pipe.x = WINDOW_WIDTH;
            down_pipe.x = WINDOW_WIDTH;
        }

        SDL_RenderClear(renderer);
        Draw_Rect(renderer, player, 255, 0, 0, 255);
        Draw_Rect(renderer, up_pipe, 255, 0, 0, 255);
        Draw_Rect(renderer, down_pipe, 255, 0, 0, 255);
        SDL_RenderPresent(renderer);
    }

    SDL_RenderClear(renderer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void Draw_Rect(SDL_Renderer* renderer, SDL_Rect rect, int r, int g, int b, int a){
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderDrawRect(renderer, &rect);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}