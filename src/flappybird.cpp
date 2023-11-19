#include <iostream>
#include <sstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

// Colision check func
bool AABB(SDL_Rect a, SDL_Rect b);

// Struct to make animated sprites
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

    // Initializing SDL and it's libs
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        std::cerr << "SDL could not be initialized. Error: " << SDL_GetError() << "\n";
        return 1;
    }

    if(TTF_Init() == -1){
        std::cerr << "SDL TTF could not be initialized. Error: " << SDL_GetError() << "\n";
        return 1;
    }

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1){
        std::cerr << "SDL Mixer could not be initialized. Error: " << SDL_GetError() << "\n";
        return 1;
    }

    // Constants
    const int WINDOW_WIDTH = 288;
    const int WINDOW_HEIGHT = 512;
    const int volume = (MIX_MAX_VOLUME * 50) / 100;

    // Utils variables
    bool game_running = true;
    bool can_score = true;
    bool menu = false;
    const int FPS = 60;
    int score = 0;
    int player_speed = 4, player_velocity = 1;
    int y = rand() % 200;
    int angle_velocity = 1, angle_speed = 5;
    int angle = 0;
    int frame_time = 0;
    std::stringstream convert;

    // Essential variables and initializations
    SDL_Event event;
    SDL_Window* window = SDL_CreateWindow("Flappy Bird", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Textures
    SDL_Texture* player_texture = IMG_LoadTexture(renderer, "res/flappybird/bird.png");
    SDL_Texture* background_texture = IMG_LoadTexture(renderer, "res/flappybird/background.png");
    SDL_Texture* ground_texture = IMG_LoadTexture(renderer, "res/flappybird/ground.png");
    SDL_Texture* pipe_up_texture = IMG_LoadTexture(renderer, "res/flappybird/pipe_up.png");
    SDL_Texture* pipe_down_texture = IMG_LoadTexture(renderer, "res/flappybird/pipe_down.png");
    TTF_Font* font = TTF_OpenFont("res/flappybird/flappy-font.ttf", 32);
    SDL_Texture* font_texture;
    SDL_Surface* font_surface;
    SDL_Texture* font_menu_texture;

    // Rects
    Entity player {player_texture, 10, 10, 3, 1};

    SDL_Rect background_rect {0, 0, 0, 0};
    SDL_QueryTexture(background_texture, nullptr, nullptr, &background_rect.w, &background_rect.h);

    SDL_Rect ground_rect {0, 400, 0, 0};
    SDL_QueryTexture(ground_texture, nullptr, nullptr, &ground_rect.w, &ground_rect.h);

    SDL_Rect pipe_up_rect {200, -(y), 0, 0};
    SDL_QueryTexture(pipe_up_texture, nullptr, nullptr, &pipe_up_rect.w, &pipe_up_rect.h);

    SDL_Rect pipe_down_rect {200, pipe_up_rect.y + pipe_up_rect.h + 100, 0, 0};
    SDL_QueryTexture(pipe_down_texture, nullptr, nullptr, &pipe_down_rect.w, &pipe_down_rect.h);

    SDL_Rect score_font_rect {WINDOW_WIDTH / 2 - 10, 10, 0, 0};
    SDL_Rect menu_font_rect {WINDOW_WIDTH / 2, 100, 0, 0};

    // Sounds
    Mix_Chunk* flap_sound = NULL;
    flap_sound = Mix_LoadWAV("res/flappybird/sfx_wing.wav");

    Mix_Chunk* hit_sound = NULL;
    hit_sound = Mix_LoadWAV("res/flappybird/hit.wav");

    Mix_Chunk* point_sound = NULL;
    point_sound = Mix_LoadWAV("res/flappybird/point.wav");

    if(flap_sound == NULL || point_sound == NULL || hit_sound == NULL){
        std::cerr << "Failed to load sound. Error: " << SDL_GetError() << "\n";
        return 1;
    }

    Mix_Volume(-1, volume);

    // Game Loop
    while(game_running){
        // Game is being played
        if(!menu){
            // Input handler
            while(SDL_PollEvent(&event)){
                if(event.type == SDL_QUIT){
                    game_running = false;
                }

                if(event.type == SDL_KEYDOWN){
                    switch(event.key.keysym.sym){
                        case SDLK_SPACE:
                            Mix_PlayChannel(-1, flap_sound, 0);
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

            // Update score texture
            convert << score;
            font_surface = TTF_RenderText_Blended(font, convert.str().c_str(), SDL_Color{255, 255, 255, 255});
            font_texture = SDL_CreateTextureFromSurface(renderer, font_surface);
            SDL_QueryTexture(font_texture, nullptr, nullptr, &score_font_rect.w, &score_font_rect.h);
            convert.str("");
            score_font_rect.x = (WINDOW_WIDTH / 2) - (score_font_rect.w / 2);

            // Frame count to animate properly
            frame_time++;
            if(FPS / frame_time == 4){
                frame_time = 0;
                player.animate();
            }

            // Update bird's angle
            angle += angle_velocity * angle_speed;

            if(angle >= 50){
                angle = 50;
            }else if (angle <= -30){
                angle = -30;
            }

            // Pushing player and obstacles
            player.dst_rect.y += player_speed * player_velocity;
            pipe_up_rect.x -= 2;
            pipe_down_rect.x -= 2;
            ground_rect.x -= 2;

            // Player boundary funcs
            if(player.dst_rect.y + player.dst_rect.h >= ground_rect.y){
                player.dst_rect.y = ground_rect.y - player.dst_rect.h;
            }

            if(player.dst_rect.y <= 0){
                player.dst_rect.y = 0;
            }

            // Reset pipe
            if(pipe_up_rect.x + pipe_up_rect.w <= 0 && pipe_down_rect.x + pipe_down_rect.w <= 0){
                y = rand() & 200;
                pipe_up_rect.x = WINDOW_WIDTH;
                pipe_up_rect.y = -(y);
                pipe_down_rect.x = WINDOW_WIDTH;
                pipe_down_rect.y = pipe_up_rect.y + pipe_up_rect.h + 100;
                can_score = true;
            }

            // Make ground move
            if(ground_rect.x <= -48){
                ground_rect.x = 0;
            }

            // Collision between player and pipes
            if(AABB(player.dst_rect, pipe_up_rect) || AABB(player.dst_rect, pipe_down_rect)){
                Mix_PlayChannel(-1, hit_sound, 0);
                menu = true;
            }

            // Check if player scored
            if(player.dst_rect.x > pipe_up_rect.x && can_score){
                Mix_PlayChannel(-1, point_sound, 0);
                score++;
                can_score = false;
            }

        
            // Render and clear sprites
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, background_texture, nullptr, &background_rect);
            SDL_RenderCopyEx(renderer, player_texture, &player.src_rect, &player.dst_rect, angle, nullptr, SDL_FLIP_NONE);
            SDL_RenderCopy(renderer, pipe_up_texture, nullptr, &pipe_up_rect);
            SDL_RenderCopy(renderer, pipe_down_texture, nullptr, &pipe_down_rect);
            SDL_RenderCopy(renderer, ground_texture, nullptr, &ground_rect);
            SDL_RenderCopy(renderer, font_texture, nullptr, &score_font_rect);
            SDL_RenderPresent(renderer);
        
        // Game is not being played
        }else{
            // Input handler
            while(SDL_PollEvent(&event)){
                if(event.type == SDL_QUIT){
                    game_running = false;
                }

                if(event.type == SDL_KEYDOWN){
                    switch(event.key.keysym.sym){
                        // Reset everything
                        case SDLK_r:
                            y = rand() & 200;
                            pipe_up_rect.x = WINDOW_WIDTH;
                            pipe_up_rect.y = -(y);
                            pipe_down_rect.x = WINDOW_WIDTH;
                            pipe_down_rect.y = pipe_up_rect.y + pipe_up_rect.h + 100;
                            player_velocity = 1;
                            player_speed = 4;
                            angle_velocity = 1;
                            angle_speed = 5;
                            can_score = true;
                            menu = false;
                            score = 0;

                    }
                }
            }

            // Update font texture
            font_surface = TTF_RenderText_Blended(font, "Press R to reset", SDL_Color{255, 255, 255, 255});
            font_menu_texture = SDL_CreateTextureFromSurface(renderer, font_surface);
            SDL_QueryTexture(font_menu_texture, nullptr, nullptr, &menu_font_rect.w, &menu_font_rect.h);
            menu_font_rect.x = (WINDOW_WIDTH / 2) - (menu_font_rect.w / 2);
            
            // Render and clear sprites
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, background_texture, nullptr, &background_rect);
            SDL_RenderCopyEx(renderer, player_texture, &player.src_rect, &player.dst_rect, angle, nullptr, SDL_FLIP_NONE);
            SDL_RenderCopy(renderer, pipe_up_texture, nullptr, &pipe_up_rect);
            SDL_RenderCopy(renderer, pipe_down_texture, nullptr, &pipe_down_rect);
            SDL_RenderCopy(renderer, ground_texture, nullptr, &ground_rect);
            SDL_RenderCopy(renderer, font_texture, nullptr, &score_font_rect);
            SDL_RenderCopy(renderer, font_menu_texture, nullptr, &menu_font_rect);
            SDL_RenderPresent(renderer);
        }
    }

    // Cleanup functions
    Mix_FreeChunk(hit_sound);
    Mix_FreeChunk(point_sound);
    Mix_FreeChunk(flap_sound);
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