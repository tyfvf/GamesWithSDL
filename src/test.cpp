#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <sstream>

int main(int argc, char* argv[]){

    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow("Replica game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    TTF_Font* comicSans = TTF_OpenFont("res/comic.ttf", 32);
    SDL_Surface* fontSurface = TTF_RenderText_Blended(comicSans, "0", SDL_Color{0, 0, 0, 255});
    SDL_Texture* fontTexture = SDL_CreateTextureFromSurface(renderer, fontSurface);
    SDL_FreeSurface(fontSurface);

    SDL_Texture* grassTex = IMG_LoadTexture(renderer, "res/grass.png");
    SDL_Rect srcPlayer;
    SDL_Rect dstPlayer;
    SDL_Rect dstFont;

    srcPlayer.x = 0;
    srcPlayer.y = 0;
    srcPlayer.w = 32;
    srcPlayer.h = 32;

    dstPlayer.x = WINDOW_WIDTH / 2;
    dstPlayer.y = WINDOW_HEIGHT / 2;
    dstPlayer.w = 32;
    dstPlayer.h = 32;

    dstFont.x = WINDOW_WIDTH / 2;
    dstFont.y = 0;

    SDL_QueryTexture(fontTexture, nullptr, nullptr, &dstFont.w, &dstFont.h);

    std::vector<SDL_Rect> enemies;

    for(int i = 0; i < 20; i++){
        SDL_Rect dst;
        dst.x = std::rand() % WINDOW_WIDTH;
        dst.y = -1 * (std::rand() % 300 + 100);
        dst.w = 32;
        dst.h = 32;
        enemies.emplace_back(dst);
    }

    int xvelocity = 0;
    int yvelocity = 0;
    int speed = 5;
    int count = 0;

    bool gameRunning = true;

    SDL_Event event;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    while(gameRunning){

        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                gameRunning = false;
            }

            if(event.type == SDL_KEYDOWN){
                switch(event.key.keysym.sym){
                    case SDLK_w:
                        yvelocity = -1;
                        break;
                    case SDLK_s:
                        yvelocity = 1;
                        break;
                    case SDLK_a:
                        xvelocity = -1;
                        break;
                    case SDLK_d:
                        xvelocity = 1;
                        break;
                }
            }


            if(event.type == SDL_KEYUP){
                switch(event.key.keysym.sym){
                    case SDLK_w:
                        yvelocity = 0;
                        break;
                    case SDLK_s:
                        yvelocity = 0;
                        break;
                    case SDLK_a:
                        xvelocity = 0;
                        break;
                    case SDLK_d:
                        xvelocity = 0;
                        break;
                }
            }
        }

        dstPlayer.x += xvelocity * speed;
        dstPlayer.y += yvelocity * speed;

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, grassTex, &srcPlayer, &dstPlayer);

        for(uint8_t i = 0; i < enemies.size(); i++){
            enemies[i].y += 5;
            SDL_RenderCopy(renderer, grassTex, &srcPlayer, &enemies[i]);
            if(enemies[i].y > WINDOW_HEIGHT + 50){
                enemies[i].y = -1 * (std::rand() % 300 + 100);
                enemies[i].x = std::rand() % WINDOW_WIDTH;
                count++;
            }
            if(
                enemies[i].x + enemies[i].w >= dstPlayer.x &&
                dstPlayer.x + dstPlayer.w >= enemies[i].x &&
                enemies[i].y + enemies[i].h >= dstPlayer.y &&
                dstPlayer.y + dstPlayer.h >= enemies[i].y 
            ){
                count = 0;
                enemies[i].y = -1 * (std::rand() % 50 + 100);
                enemies[i].x = std::rand() % WINDOW_WIDTH;
                dstPlayer.x = WINDOW_WIDTH / 2;
                dstPlayer.y = WINDOW_HEIGHT / 2;
            }
        }

        std::stringstream convert;
        convert << count;
        SDL_Surface* fontSurface = TTF_RenderText_Blended(comicSans, convert.str().c_str(), SDL_Color{0, 0, 0, 255});
        SDL_Texture* fontTexture = SDL_CreateTextureFromSurface(renderer, fontSurface);
        SDL_FreeSurface(fontSurface);
        SDL_RenderCopy(renderer, fontTexture, nullptr, &dstFont);
        
        SDL_RenderPresent(renderer);

    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}