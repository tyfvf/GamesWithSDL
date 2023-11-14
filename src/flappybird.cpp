#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

struct Entity{
    int frame_width, frame_height;
    int texture_width, texture_height;
    SDL_Rect src_rect, dst_rect;

    Entity(SDL_Texture* texture, int dest_x, int dest_y, int n_cols, int n_rows){
        SDL_QueryTexture(texture, nullptr, nullptr, &texture_width, &texture_height);
        frame_width = texture_width / n_cols;
        frame_height = texture_height / n_rows;
        src_rect = {0, 0, frame_width, frame_height};
        dst_rect = {dest_x, dest_y, frame_width, frame_height};
    }

    void animate(){
        src_rect.x += frame_width;
        if(src_rect.x > texture_width - frame_width){
            src_rect.x = 0;
        }
    }
};

int main(int argc, char* argv[]){

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        std::cerr << "SDL could not be initialized. Error: " << SDL_GetError() << "\n";
        return 1;
    }

    const int WINDOW_WIDTH = 288;
    const int WINDOW_HEIGHT = 512;
    bool game_running = true;
    int player_speed = 4;
    int player_velocity = 1;
    const int FPS = 60;
    int frame_time = 0;

    SDL_Event event;
    SDL_Window* window = SDL_CreateWindow("Flappy Bird", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_Texture* player_texture = IMG_LoadTexture(renderer, "res/flappybird/bird.png");
    SDL_Texture* background_texture = IMG_LoadTexture(renderer, "res/flappybird/background.png");
    SDL_Texture* ground_texture = IMG_LoadTexture(renderer, "res/flappybird/ground.png");
    SDL_Texture* pipe_up_texture = IMG_LoadTexture(renderer, "res/flappybird/pipe_up.png");
    SDL_Texture* pipe_down_texture = IMG_LoadTexture(renderer, "res/flappybird/pipe_down.png");

    Entity player {player_texture, 10, 10, 3, 1};

    SDL_Rect background_rect {0, 0, 0, 0};
    SDL_QueryTexture(background_texture, nullptr, nullptr, &background_rect.w, &background_rect.h);

    SDL_Rect ground_rect {0, 400, 0, 0};
    SDL_QueryTexture(ground_texture, nullptr, nullptr, &ground_rect.w, &ground_rect.h);

    SDL_Rect pipe_up_rect {200, -90, 0, 0};
    SDL_QueryTexture(pipe_up_texture, nullptr, nullptr, &pipe_up_rect.w, &pipe_up_rect.h);

    SDL_Rect pipe_down_rect {200, WINDOW_HEIGHT - 180, 0, 0};
    SDL_QueryTexture(pipe_down_texture, nullptr, nullptr, &pipe_down_rect.w, &pipe_down_rect.h);

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

        frame_time++;

        if(FPS / frame_time == 4){
            frame_time = 0;
            player.animate();
        }

        player.dst_rect.y += player_speed * player_velocity;
        pipe_up_rect.x -= 2;
        pipe_down_rect.x -= 2;

        if(player.dst_rect.y + player.dst_rect.h >= ground_rect.y){
            player.dst_rect.y = ground_rect.y - player.dst_rect.h;
        }

        if(pipe_up_rect.x + pipe_up_rect.w <= 0 && pipe_down_rect.x + pipe_down_rect.w <= 0){
            pipe_up_rect.x = WINDOW_WIDTH;
            pipe_down_rect.x = WINDOW_WIDTH;
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, background_texture, nullptr, &background_rect);
        SDL_RenderCopy(renderer, player_texture, &player.src_rect, &player.dst_rect);
        SDL_RenderCopy(renderer, pipe_up_texture, nullptr, &pipe_up_rect);
        SDL_RenderCopy(renderer, pipe_down_texture, nullptr, &pipe_down_rect);
        SDL_RenderCopy(renderer, ground_texture, nullptr, &ground_rect);
        SDL_RenderPresent(renderer);
    }

    SDL_RenderClear(renderer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}