#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


bool AABB(SDL_Rect a, SDL_Rect b);
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
    int player_speed = 4, player_velocity = 1;
    const int FPS = 60;
    int frame_time = 0;
    int y = rand() % 200;
    int angle = 0;
    int angle_velocity = 1, angle_speed = 5;

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

    SDL_Rect pipe_up_rect {200, -(y), 0, 0};
    SDL_QueryTexture(pipe_up_texture, nullptr, nullptr, &pipe_up_rect.w, &pipe_up_rect.h);

    SDL_Rect pipe_down_rect {200, pipe_up_rect.y + pipe_up_rect.h + 100, 0, 0};
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
                        angle_velocity = -1;
                        angle_speed = 14;
                }
            }

            if(event.type == SDL_KEYUP){
                switch(event.key.keysym.sym){
                    case SDLK_SPACE:
                        player_velocity = 1;
                        player_speed = 4;
                        angle_velocity = 1;
                        angle_speed = 5;
                }
            }
        }

        frame_time++;
        angle += angle_velocity * angle_speed;

        if(angle >= 50){
            angle = 50;
        }else if (angle <= -30){
            angle = -30;
        }

        if(FPS / frame_time == 4){
            frame_time = 0;
            player.animate();
        }

        player.dst_rect.y += player_speed * player_velocity;
        pipe_up_rect.x -= 2;
        pipe_down_rect.x -= 2;
        ground_rect.x -= 2;

        if(player.dst_rect.y + player.dst_rect.h >= ground_rect.y){
            player.dst_rect.y = ground_rect.y - player.dst_rect.h;
        }

        if(pipe_up_rect.x + pipe_up_rect.w <= 0 && pipe_down_rect.x + pipe_down_rect.w <= 0){
            y = rand() & 200;
            pipe_up_rect.x = WINDOW_WIDTH;
            pipe_up_rect.y = -(y);
            pipe_down_rect.x = WINDOW_WIDTH;
            pipe_down_rect.y = pipe_up_rect.y + pipe_up_rect.h + 100;
        }

        if(ground_rect.x <= -48){
            ground_rect.x = 0;
        }

        if(AABB(player.dst_rect, pipe_up_rect) || AABB(player.dst_rect, pipe_down_rect)){
            std::cout << "HIT" << "\n";
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, background_texture, nullptr, &background_rect);
        SDL_RenderCopyEx(renderer, player_texture, &player.src_rect, &player.dst_rect, angle, nullptr, SDL_FLIP_NONE);
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