#include <iostream>
#include <vector>
#include <random>
#include <sstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

bool AABB(SDL_Rect a, SDL_Rect b);
void AudioCallback(void *userdata, Uint8 *stream, int len);
void PlaySound(const char* path);
struct Entity{
    int frameWidth, frameHeight;
    int textureWidth, textureHeight;
    SDL_Rect srcRect, dstRect;

    Entity(SDL_Texture* tex, int destx, int desty, int ncols, int nrows){
        SDL_QueryTexture(tex, nullptr, nullptr, &textureWidth, &textureHeight);
        frameWidth = textureWidth / ncols;
        frameHeight = textureHeight / nrows;
        srcRect = {0, 0, frameWidth, frameHeight};
        dstRect = {destx, desty, frameWidth, frameHeight};
    };

    void animate(){
        srcRect.x += frameWidth;
        if(srcRect.x > textureWidth - frameWidth){
            srcRect.x = 0;
        }
    };
};

Uint8 *audioPos;
Uint32 audioLen;

int main(int argc, char* argv[]){

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        std::cerr << "SDL could not be initialized " << SDL_GetError() << "\n";
        return 1;
    }

    if(TTF_Init() == -1){
        std::cerr << "SDL TTF could not be initialized " << SDL_GetError() << "\n";
        return 1;
    }

    PlaySound("res/runner/music.wav");

    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 400;
    int score = 0;
    const int FPS = 60;
    int frameTime = 0;
    
    SDL_Window* window = SDL_CreateWindow("Runner", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_Texture* skyTex = IMG_LoadTexture(renderer, "res/runner/sky.png");
    SDL_Texture* groundTex = IMG_LoadTexture(renderer, "res/runner/ground.png");
    SDL_Texture* playerTex = IMG_LoadTexture(renderer, "res/runner/player.png");
    SDL_Texture* playerStandTex = IMG_LoadTexture(renderer, "res/runner/player_stand.png");
    SDL_Texture* snailTex = IMG_LoadTexture(renderer, "res/runner/snail.png");
    SDL_Texture* flyTex = IMG_LoadTexture(renderer, "res/runner/fly.png");
    TTF_Font* pixelType = TTF_OpenFont("res/runner/pixeltype.ttf", 32);

    SDL_Rect skyRect {0, 0, 800, 300};
    SDL_Rect groundRect {0, skyRect.h, 800, 100};
    SDL_Rect fontRect {WINDOW_WIDTH / 2 - 40, 40, 0, 0};
    SDL_Rect playerStandRect {WINDOW_WIDTH / 2 - 75, WINDOW_HEIGHT / 2 - 75, 150, 180};

    SDL_Rect fontMenuTitleRect {WINDOW_WIDTH / 2 - 128, 40, 256, 40};
    SDL_Surface* fontMenuTitleSurf = TTF_RenderText_Blended(pixelType, "Pixel Runner", SDL_Color{135, 200, 187, 255});
    SDL_Texture* fontMenuTitleTex = SDL_CreateTextureFromSurface(renderer, fontMenuTitleSurf);
    SDL_FreeSurface(fontMenuTitleSurf);

    SDL_Rect fontMenuStartRect {WINDOW_WIDTH / 2 - 181, WINDOW_HEIGHT - 60, 362, 40};
    SDL_Surface* fontMenuStartSurf = TTF_RenderText_Blended(pixelType, "Press space to run", SDL_Color{135, 200, 187, 255});
    SDL_Texture* fontMenuStartTex = SDL_CreateTextureFromSurface(renderer, fontMenuStartSurf);
    SDL_FreeSurface(fontMenuStartSurf);

    SDL_Rect fontMenuScoreRect {WINDOW_WIDTH / 2 - 132, WINDOW_HEIGHT - 60, 264, 40};

    Entity player {playerTex, 100, WINDOW_HEIGHT - 32, 2, 2};
    Entity snail {snailTex, WINDOW_WIDTH, skyRect.h - 32, 2, 1};
    Entity fly {flyTex, WINDOW_WIDTH, skyRect.h - 130, 2, 1};

    int velocity = 1;
    int speed = 1;
    std::vector <SDL_Rect> enemyRect = {fly.dstRect, snail.dstRect};
    float enemySpeed = 5;

    SDL_Event event;
    bool gameRunning = true;
    bool gameMenu = true;
    int enemy = rand() % 2;

    while(gameRunning){

        if(audioLen == 0){
            SDL_CloseAudio();
            PlaySound("res/runner/music.wav");
        }

        if(gameMenu){
            while(SDL_PollEvent(&event)){
                if(event.type == SDL_QUIT){
                    gameRunning = false;
                }

                if(event.type == SDL_KEYDOWN){
                    switch(event.key.keysym.sym){
                        case SDLK_SPACE:
                            gameMenu = false;
                            score = 0;
                    }
                }
            }

            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 100, 130, 161, 255);
            SDL_RenderCopy(renderer, playerStandTex, nullptr, &playerStandRect);
            SDL_RenderCopy(renderer, fontMenuTitleTex, nullptr, &fontMenuTitleRect);
            if(score != 0){

                std::stringstream convert;
                convert << "Your score: " << score;
                SDL_Surface* fontSurf = TTF_RenderText_Blended(pixelType, convert.str().c_str(), SDL_Color{135, 200, 187, 255});
                SDL_Texture* fontTex = SDL_CreateTextureFromSurface(renderer, fontSurf);
                SDL_FreeSurface(fontSurf);
                convert.clear();

                SDL_RenderCopy(renderer, fontTex, nullptr, &fontMenuScoreRect);
            }else{
                SDL_RenderCopy(renderer, fontMenuStartTex, nullptr, &fontMenuStartRect);
            }
            SDL_RenderPresent(renderer);

        }else{
            while(SDL_PollEvent(&event)){
                if(event.type == SDL_QUIT){
                    gameRunning = false;
                }

                if(event.type == SDL_KEYDOWN){
                    switch(event.key.keysym.sym){
                        case SDLK_SPACE:
                            if(player.dstRect.y == groundRect.y - player.dstRect.h){
                                player.srcRect.y = player.frameHeight;
                                player.srcRect.x = 0;
                                velocity = -1;
                                speed = 15;
                            } 
                            break;
                    }
                }
            }

            frameTime++;

            if(FPS / frameTime == 4){
                frameTime = 0;
                if(player.dstRect.y == groundRect.y - player.dstRect.h){
                    player.srcRect.y = 0;
                    player.animate();
                }
                snail.animate();
                fly.animate();
            }

            std::stringstream convert;
            convert << "Score: " << score;
            SDL_Surface* fontSurf = TTF_RenderText_Blended(pixelType, convert.str().c_str(), SDL_Color{15, 15, 15, 255});
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
                enemySpeed = 5;
                enemy = rand() % 2;
                enemyRect[0].x = WINDOW_WIDTH;
                enemyRect[1].x = WINDOW_WIDTH;
                gameMenu = true;
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
    }

    
    SDL_RenderClear(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void AudioCallback(void *userdata, Uint8 *stream, int len){
    if(audioLen == 0){
        return;
    }

    len = ( len > audioLen ? audioLen : len);
    SDL_memset(stream, 0, len);
    SDL_MixAudio(stream, audioPos, len, 10);
    
    audioPos += len;
    audioLen -= len;
}

void PlaySound(const char* path){
    Uint32 wavLength;
    Uint8 *wavBuffer;
    SDL_AudioSpec wavSpec;

    if(SDL_LoadWAV(path, &wavSpec, &wavBuffer, &wavLength) == NULL){
        std::cerr << "Could not load audio. Error: " << SDL_GetError() << "\n";
    }

    wavSpec.callback = AudioCallback;
    wavSpec.userdata = NULL;

    audioPos = wavBuffer;
    audioLen = wavLength;

    if(SDL_OpenAudio(&wavSpec, NULL) < 0){
        std::cerr << "Could not open audio. Error: " << SDL_GetError() << "\n";
    }

    SDL_PauseAudio(0);
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