#include <iostream>
#include <vector>
#include <random>
#include <sstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

bool AABB(SDL_Rect a, SDL_Rect b);
class Entity{
    public:
        int frameWidth, frameHeight;
        int textureWidth, textureHeight;
        SDL_Rect srcRect, dstRect;

        Entity(SDL_Texture* tex, int destx, int desty){
            SDL_QueryTexture(tex, nullptr, nullptr, &textureWidth, &textureHeight);
            frameWidth = textureWidth / 2;
            frameHeight = textureHeight / 1;
            srcRect = {0, 0, frameWidth, frameHeight};
            dstRect = {destx, desty, frameWidth, frameHeight};
        };

        void update(){
            srcRect.x += frameWidth;
            if(srcRect.x > frameWidth){
                srcRect.x = 0;
            }
        };
};

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
    const int FPS = 60;
    int frameTime = 0;
    
    SDL_Window* window = SDL_CreateWindow("Runner", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_Texture* skyTex = IMG_LoadTexture(renderer, "res/sky.png");
    SDL_Texture* groundTex = IMG_LoadTexture(renderer, "res/ground.png");
    SDL_Texture* playerTex = IMG_LoadTexture(renderer, "res/player.png");
    SDL_Texture* snailTex = IMG_LoadTexture(renderer, "res/snail.png");
    SDL_Texture* flyTex = IMG_LoadTexture(renderer, "res/fly.png");
    TTF_Font* comicSans = TTF_OpenFont("res/comic.ttf", 32);

    SDL_Rect skyRect {0, 0, 800, 300};
    SDL_Rect groundRect {0, skyRect.h, 800, 100};

    Entity player {playerTex, 100, WINDOW_HEIGHT - 32};
    Entity snail {snailTex, WINDOW_WIDTH, skyRect.h - 32};
    Entity fly {flyTex, WINDOW_WIDTH, skyRect.h - 130};

    SDL_Rect fontRect {WINDOW_WIDTH / 2, 0, 32, 32};
    

    int velocity = 1;
    int speed = 1;
    std::vector <SDL_Rect> enemyRect = {fly.dstRect, snail.dstRect};
    float enemySpeed = 5;

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
                        if(player.dstRect.y == groundRect.y - player.dstRect.h){
                            velocity = -1;
                            speed = 20;
                        } 
                        break;
                }
            }

        }

        frameTime++;

        if(FPS / frameTime == 4){
            frameTime = 0;
            player.update();
            snail.update();
            fly.update();
        }

        std::stringstream convert;
        convert << score;
        SDL_Surface* fontSurf = TTF_RenderText_Blended(comicSans, convert.str().c_str(), SDL_Color{15, 15, 15, 255});
        SDL_Texture* fontTex = SDL_CreateTextureFromSurface(renderer, fontSurf);
        SDL_FreeSurface(fontSurf);
        SDL_QueryTexture(fontTex, nullptr, nullptr, &fontRect.w, &fontRect.h);
        convert.clear();

        player.dstRect.y += velocity * speed;
        enemyRect[enemy].x -= enemySpeed;

        if(player.dstRect.y <= 50){
            velocity = 1;
            speed = 5;
        }

        if(AABB(player.dstRect, enemyRect[enemy])){
            score = 0;
            enemySpeed = 5;
        }

        if(player.dstRect.y  + player.dstRect.h >= skyRect.h){
            player.dstRect.y = skyRect.h - player.dstRect.h;
        }

        if(enemyRect[enemy].x < 0){
            score++;
            enemy = rand() % 2;
            enemyRect[0].x = WINDOW_WIDTH;
            enemyRect[1].x = WINDOW_WIDTH;
            enemySpeed *= 1.01;
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, skyTex, nullptr, &skyRect);
        SDL_RenderCopy(renderer, groundTex, nullptr, &groundRect);
        SDL_RenderCopy(renderer, playerTex, &player.srcRect, &player.dstRect);
        if(enemy == 1){
            SDL_RenderCopy(renderer, snailTex, &snail.srcRect, &enemyRect[enemy]);
        }else{
            SDL_RenderCopy(renderer, flyTex, &fly.srcRect, &enemyRect[enemy]);
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