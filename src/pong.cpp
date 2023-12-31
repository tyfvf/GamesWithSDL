#include <iostream>
#include <sstream>
#include <random>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

bool AABB(SDL_Rect a, SDL_Rect b);

int main(int argc, char* argv[]){

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        std::cout << "ERROR! SDL COULD NOT BE INITIALIZED: " << SDL_GetError() << "\n";
    }

    if(TTF_Init() == -1){
        std::cout << "ERROR! SDL TTF COULD NOT BE INITIALIZED: " << SDL_GetError() << "\n";
    }

    const int WINDOW_HEIGHT = 600;
    const int WINDOW_WIDTH = 800;

    SDL_Window* window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    TTF_Font* comicSans = TTF_OpenFont("res/comic.ttf", 32);
    SDL_Surface* fontSurface = TTF_RenderText_Blended(comicSans, "0", SDL_Color{255, 255, 255, 255});
    SDL_Texture* fontTexture = SDL_CreateTextureFromSurface(renderer, fontSurface);
    SDL_FreeSurface(fontSurface);

    SDL_Rect font1 {WINDOW_WIDTH / 2 - 75, 50, 0, 0};
    SDL_QueryTexture(fontTexture, nullptr, nullptr, &font1.w, &font1.h);

    SDL_Rect font2 {WINDOW_WIDTH / 2 + 50, 50, 0, 0};
    SDL_QueryTexture(fontTexture, nullptr, nullptr, &font2.w, &font2.h);

    SDL_Rect ball {WINDOW_WIDTH / 2 - 12, WINDOW_HEIGHT / 2 - 12, 20, 20};
    int x = rand() % 2;
    int choices[2] = {1, -1};
    int ballxvelocity = choices[x];
    int ballyvelocity = choices[x];
    int ballspeed = 2;

    SDL_Rect player;
    player.w = 25;
    player.h = 100;
    player.y = WINDOW_HEIGHT / 2 - player.h / 2;
    player.x = 10;
    int velocity1 = 0;
    int count1 = 0;

    SDL_Rect player2;
    player2.w = 25;
    player2.h = 100;
    player2.x = WINDOW_WIDTH - player2.w - 10;
    player2.y = WINDOW_HEIGHT / 2 - player2.h / 2;
    int velocity2 = 0;
    int count2 = 0;

    bool gameRunning = true;
    SDL_Event event;
    int speed = 7;

    while(gameRunning){

        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                gameRunning = false;
            }

            if(event.type == SDL_KEYDOWN){
                switch (event.key.keysym.sym)
                {
                    case SDLK_w:
                        velocity1 = -1;
                        break;                  
                    case SDLK_s:
                        velocity1 = 1;
                        break;
                    case SDLK_UP:
                        velocity2 = -1;
                        break;                  
                    case SDLK_DOWN:
                        velocity2 = 1;
                        break;
                    case SDLK_r:
                        count1 = 0;
                        count2 = 0;
                        ball.x = WINDOW_WIDTH / 2 - 12;
                        ball.y = WINDOW_HEIGHT / 2 - 12;
                        x = rand() % 2;
                        ballxvelocity = choices[x];
                        ballyvelocity = choices[x];
                        ballspeed = 2;
                        player.y = WINDOW_HEIGHT / 2 - player.h / 2;
                        player.x = 10;
                        player2.x = WINDOW_WIDTH - player2.w - 10;
                        player2.y = WINDOW_HEIGHT / 2 - player2.h / 2;
                    default:
                        break;
                }
            }

            if(event.type == SDL_KEYUP){
                switch (event.key.keysym.sym)
                {
                    case SDLK_w:
                        velocity1 = 0;
                        break;                  
                    case SDLK_s:
                        velocity1 = 0;
                        break;
                    case SDLK_UP:
                        velocity2 = 0;
                        break;                  
                    case SDLK_DOWN:
                        velocity2 = 0;
                        break;
                    default:
                        break;
                }
            }
        }

        player.y += velocity1 * speed;
        player2.y += velocity2 * speed;

        if(player.y + player.h >= WINDOW_HEIGHT){
            player.y = WINDOW_HEIGHT - player.h;
        }
        if(player.y <= 0){
            player.y = 0;
        }
        if(player2.y + player2.h >= WINDOW_HEIGHT){
            player2.y = WINDOW_HEIGHT - player2.h;
        }
        if(player2.y <= 0){
            player2.y = 0;
        }

        std::stringstream convert;

        convert << count1;

        SDL_Surface* fontSurface1 = TTF_RenderText_Blended(comicSans, convert.str().c_str(), SDL_Color{255, 255, 255, 255});
        SDL_Texture* fontTexture1 = SDL_CreateTextureFromSurface(renderer, fontSurface1);
        SDL_FreeSurface(fontSurface1);

        convert.str("");
        convert.clear();
        convert << count2;

        SDL_Surface* fontSurface2 = TTF_RenderText_Blended(comicSans, convert.str().c_str(), SDL_Color{255, 255, 255, 255});
        SDL_Texture* fontTexture2 = SDL_CreateTextureFromSurface(renderer, fontSurface2);
        SDL_FreeSurface(fontSurface2);

        ball.x += ballxvelocity * ballspeed;
        ball.y += ballyvelocity * ballspeed;

        if(ball.y + ball.h >= WINDOW_HEIGHT){
            ballyvelocity *= -1;
        }
        if(ball.y <= 0){
            ballyvelocity *= -1;
        }
        if(AABB(ball, player2)){
            ballxvelocity *= -1;
            ballspeed += 1;
        }
        if(AABB(ball, player)){
            ballxvelocity *= -1;
            ballspeed += 1;
        }

        if(ball.x + ball.w >= WINDOW_WIDTH){
            count1 += 1;
            ball.x = WINDOW_WIDTH / 2 - 12;
            ball.y = WINDOW_HEIGHT / 2 - 12;
            x = rand() % 2;
            ballxvelocity = choices[x];
            ballyvelocity = choices[x];
            ballspeed = 2;
        }
        if(ball.x <= 0){
            count2 += 1;
            ball.x = WINDOW_WIDTH / 2 - 12;
            ball.y = WINDOW_HEIGHT / 2 - 12;
            x = rand() % 2;
            ballxvelocity = choices[x];
            ballyvelocity = choices[x];
            ballspeed = 2;
        }

        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &player);
        SDL_RenderFillRect(renderer, &player);
        SDL_RenderDrawRect(renderer, &player2);
        SDL_RenderFillRect(renderer, &player2);
        SDL_RenderDrawLine(renderer, WINDOW_WIDTH / 2, 0, WINDOW_WIDTH /2, WINDOW_HEIGHT);
        SDL_RenderDrawRect(renderer, &ball);
        SDL_RenderFillRect(renderer, &ball);
        SDL_RenderCopy(renderer, fontTexture1, nullptr, &font1);
        SDL_RenderCopy(renderer, fontTexture2, nullptr, &font2);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderPresent(renderer);

    }

    SDL_RenderClear(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();


    return 0;
}

bool AABB(SDL_Rect a, SDL_Rect b){
    if(a.x + a.w >= b.x && b.x + b.w >= a.x && a.y + a.h >= b.y && b.y + b.h >= a.y){
        return true;
    }else{
        return false;
    }
}