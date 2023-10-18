#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

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
    SDL_Rect src;
    SDL_Rect dst;
    SDL_Rect dstFont;

    src.x = 0;
    src.y = 0;
    src.w = 32;
    src.h = 32;

    dst.x = WINDOW_WIDTH / 2;
    dst.y = WINDOW_HEIGHT / 2;
    dst.w = 32;
    dst.h = 32;

    dstFont.x = WINDOW_WIDTH / 2;
    dstFont.y = 0;
    SDL_QueryTexture(fontTexture, nullptr, nullptr, &dstFont.w, &dstFont.h);

    int xvelocity = 0;
    int yvelocity = 0;
    int speed = 5;

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

        dst.x += xvelocity * speed;
        dst.y += yvelocity * speed;

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, grassTex, &src, &dst);
        SDL_RenderCopy(renderer, fontTexture, nullptr, &dstFont);
        SDL_RenderPresent(renderer);

    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}