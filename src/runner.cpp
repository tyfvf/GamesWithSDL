#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

std::vector <SDL_Rect> CreateRect(int x, int y, int w, int h);
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
    
    SDL_Window* window = SDL_CreateWindow("Runner", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_Texture* skyTex = IMG_LoadTexture(renderer, "res/sky.png");
    SDL_Texture* groundTex = IMG_LoadTexture(renderer, "res/ground.png");
    SDL_Texture* playerTex = IMG_LoadTexture(renderer, "res/player.png");
    SDL_Texture* snailTex = IMG_LoadTexture(renderer, "res/snail.png");

    std::vector <SDL_Rect> skyRect = CreateRect(0, 0, 800, 300);
    
    std::vector <SDL_Rect> groundRect = CreateRect(0, skyRect[1].h, 800, 100);

    std::vector <SDL_Rect> playerRect = CreateRect(100, WINDOW_HEIGHT - 32, 32, 32);
    SDL_QueryTexture(playerTex, nullptr, nullptr, &playerRect[0].w, &playerRect[0].h);
    SDL_QueryTexture(playerTex, nullptr, nullptr, &playerRect[1].w, &playerRect[1].h);
    int velocity = 1;
    int speed = 5;

    std::vector <SDL_Rect> snailRect = CreateRect(WINDOW_WIDTH, skyRect[1].h - 32, 32, 32);
    SDL_QueryTexture(snailTex, nullptr, nullptr, &snailRect[0].w, &snailRect[0].h);
    SDL_QueryTexture(snailTex, nullptr, nullptr, &snailRect[1].w, &snailRect[1].h);

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

        playerRect[1].y += velocity * speed;
        snailRect[1].x -= 5;

        if(AABB(playerRect[1], snailRect[1])){
            std::cout << "HIT" << "\n";
        }

        if(playerRect[1].y  + playerRect[1].h >= skyRect[1].h){
            playerRect[1].y = skyRect[1].h - playerRect[1].h;
        }

        if(snailRect[1].x < 0){
            snailRect[1].x = WINDOW_WIDTH;
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, skyTex, &skyRect[0], &skyRect[1]);
        SDL_RenderCopy(renderer, groundTex, &groundRect[0], &groundRect[1]);
        SDL_RenderCopy(renderer, playerTex, &playerRect[0], &playerRect[1]);
        SDL_RenderCopy(renderer, snailTex, &snailRect[0], &snailRect[1]);
        SDL_RenderPresent(renderer);
    }

    SDL_RenderClear(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

std::vector <SDL_Rect> CreateRect(int x, int y, int w, int h){
    SDL_Rect srcRect;
    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = w;
    srcRect.h = h;

    SDL_Rect dstRect;
    dstRect.x = x;
    dstRect.y = y;
    dstRect.w = w;
    dstRect.h = h;

    std::vector <SDL_Rect> rect;
    rect.emplace_back(srcRect);
    rect.emplace_back(dstRect);

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