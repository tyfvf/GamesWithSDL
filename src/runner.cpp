#include <iostream>
#include <vector>
#include <random>
#include <sstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

bool AABB(SDL_Rect a, SDL_Rect b);

int main(int argc, char* argv[]){

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        std::cout << "SDL could not be initialized " << SDL_GetError() << "\n";
    }

    if(TTF_Init() == -1){
        std::cout << "SDL TTF could not be initialized " << SDL_GetError() << "\n";
    }

    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 400;
    int score = 0;
    
    SDL_Window* window = SDL_CreateWindow("Runner", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_Texture* skyTex = IMG_LoadTexture(renderer, "res/sky.png");
    SDL_Texture* groundTex = IMG_LoadTexture(renderer, "res/ground.png");
    SDL_Texture* playerTex = IMG_LoadTexture(renderer, "res/player.png");
    SDL_Texture* snailTex = IMG_LoadTexture(renderer, "res/snail.png");
    SDL_Texture* flyTex = IMG_LoadTexture(renderer, "res/fly.png");
    TTF_Font* comicSans = TTF_OpenFont("res/comic.ttf", 32);

    

    SDL_Rect skyRect = {0, 0, 800, 300};
    SDL_Rect playerRect = {100, WINDOW_HEIGHT - 32, 32, 32};
    SDL_QueryTexture(playerTex, nullptr, nullptr, &playerRect.w, &playerRect.h);
    SDL_Rect groundRect = {0, skyRect.h, 800, 100};
    SDL_Rect snailRect = {WINDOW_WIDTH, skyRect.h - 32, 32, 32};
    SDL_QueryTexture(snailTex, nullptr, nullptr, &snailRect.w, &snailRect.h);
    SDL_Rect flyRect = {WINDOW_WIDTH, skyRect.h - 120, 32, 32};
    SDL_QueryTexture(flyTex, nullptr, nullptr, &flyRect.w, &flyRect.h);
    SDL_Rect fontRect = {WINDOW_WIDTH / 2, 0, 32, 32};
    

    int velocity = 1;
    int speed = 5;
    std::vector <SDL_Rect> enemyRect = {flyRect, snailRect};

    SDL_Event event;
    bool gameRunning = true;
    int enemy = rand() % 2;

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

        std::stringstream convert;
        convert << score;
        SDL_Surface* fontSurf = TTF_RenderText_Blended(comicSans, convert.str().c_str(), SDL_Color{15, 15, 15, 255});
        SDL_Texture* fontTex = SDL_CreateTextureFromSurface(renderer, fontSurf);
        SDL_FreeSurface(fontSurf);
        SDL_QueryTexture(fontTex, nullptr, nullptr, &fontRect.w, &fontRect.h);
        convert.clear();

        playerRect.y += velocity * speed;
        enemyRect[enemy].x -= 5;

        if(AABB(playerRect, enemyRect[enemy])){
            std::cout << "HIT" << "\n";
        }

        if(playerRect.y  + playerRect.h >= skyRect.h){
            playerRect.y = skyRect.h - playerRect.h;
        }

        if(enemyRect[enemy].x < 0){
            score++;
            enemy = rand() % 2;
            enemyRect[0].x = WINDOW_WIDTH;
            enemyRect[1].x = WINDOW_WIDTH;
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, skyTex, nullptr, &skyRect);
        SDL_RenderCopy(renderer, groundTex, nullptr, &groundRect);
        SDL_RenderCopy(renderer, playerTex, nullptr, &playerRect);
        if(enemy == 1){
            SDL_RenderCopy(renderer, snailTex, nullptr, &enemyRect[enemy]);
        }else{
            SDL_RenderCopy(renderer, flyTex, nullptr, &enemyRect[enemy]);
        }
        SDL_RenderCopy(renderer, fontTex, nullptr, &fontRect);
        SDL_RenderPresent(renderer);
    }

    SDL_RenderClear(renderer);
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