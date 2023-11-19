#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <iostream>
#include <vector>
#include <ctime>

const int BALL_RADIUS = 15;

struct vector2 {
    int x, y;
    vector2(){
        x = 0;
        y = 0;
    };
    vector2(int x, int y){
        this->x = x;
        this->y = y;
    };
    void set(int x, int y){
        this->x = x;
        this->y = y;
    };
};

int getRand(int rMin, int rMax){
    int r = rand() % (rMax - rMin + 1) + rMin;
    return r;
}

bool isColliding(std::vector<vector2> obstacles, int x1, int y1){
    for(size_t i = 0; i < obstacles.size(); i++){
        int x2 = obstacles[i].x;
        int y2 = obstacles[i].y;

        if(
            x1 < x2 + BALL_RADIUS &&
            x1 + BALL_RADIUS > x2 &&
            y1 < y2 + BALL_RADIUS &&
            BALL_RADIUS + y1 > y2
        ){
            return true;
        }
    }

    return false;
}

bool isColliding(int x1, int y1, int x2, int y2){
    if(
        x1 < x2 + BALL_RADIUS &&
        x1 + BALL_RADIUS > x2 &&
        y1 < y2 + BALL_RADIUS &&
        BALL_RADIUS + y1 > y2
    ){
        return true;
    }

    return false;
}

void drawDrop(SDL_Renderer* renderer, int x, int y) {
    filledCircleRGBA(renderer, x, y, BALL_RADIUS, 0, 0, 255, 150);
}

void drawObstacle(SDL_Renderer* renderer, int x, int y) {
    filledCircleRGBA(renderer, x, y, BALL_RADIUS, 50, 0, 0, 255);
}

void drawCursor(SDL_Renderer* renderer, int x, int y){
    filledCircleRGBA(renderer, x, y, 10, 255, 255, 255, 255);
}

int main(int argc, char* argv[]){
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        std::cerr << "SDL could not be initialized. Error" << SDL_GetError() << "\n";
    }
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event event;
    bool isRunning = true;
    const int GAME_WIDTH = 1200;
    const int GAME_HEIGHT  = 900;

    srand(time(NULL));

    window = SDL_CreateWindow("Rain", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, GAME_WIDTH, GAME_HEIGHT, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    std::vector<vector2> drops;
    std::vector<vector2> otherDrops;
    std::vector<vector2> obstacles;
    vector2 prevObstacle(0, 0);

    bool isMousePressed = false;
    bool isErasing = false;
    const int speedOfDrops = 20;
    int mouseX = 0, mouseY = 0;

    while(isRunning){

        if(drops.size() < 1000){
            drops.push_back(vector2(getRand(0, GAME_WIDTH), 0));
        }

        for(size_t i = 0; i < drops.size(); i++){
            otherDrops = drops;
            otherDrops.erase(otherDrops.begin() + i);

            if(isColliding(obstacles, drops[i].x, drops[i].y)){
                drops[i].set(drops[i].x, drops[i].y - BALL_RADIUS);
            }

            if(
                !isColliding(obstacles, drops[i].x, drops[i].y + BALL_RADIUS) &&
                !isColliding(otherDrops, drops[i].x, drops[i].y + BALL_RADIUS)
            ){
                drops[i].set(drops[i].x, drops[i].y + speedOfDrops);
            }else if(
                !isColliding(obstacles, drops[i].x - BALL_RADIUS, drops[i].y) &&
                !isColliding(otherDrops, drops[i].x - BALL_RADIUS, drops[i].y)
            ) {
                drops[i].set(drops[i].x - (speedOfDrops / 4), drops[i].y);
            }else if(
                !isColliding(obstacles, drops[i].x + BALL_RADIUS, drops[i].y) &&
                !isColliding(otherDrops, drops[i].x + BALL_RADIUS, drops[i].y)
            ){
                drops[i].set(drops[i].x + (speedOfDrops / 4), drops[i].y);
            }

            drawDrop(renderer, drops[i].x, drops[i].y);

            if(drops[i].x > GAME_WIDTH || drops[i].x < 0 || drops[i].y > GAME_HEIGHT || drops[i].y < 0){
                drops.erase(drops.begin() + i);
            }

        }

        for(size_t i = 0; i < obstacles.size(); i++){
            drawObstacle(renderer, obstacles[i].x, obstacles[i].y);
        }

        if(isErasing){
            SDL_GetMouseState(&mouseX, &mouseY);
            drawCursor(renderer, mouseX, mouseY);

            for(size_t i = 0; i < obstacles.size(); i++){
                if(isColliding(obstacles[i].x, obstacles[i].y, mouseX, mouseY)){
                    obstacles.erase(obstacles.begin() + i);
                }
            }
        }

        if(SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }

            if(event.type == SDL_MOUSEBUTTONDOWN){
                if (event.button.button == SDL_BUTTON_LEFT){
                    isMousePressed = true;
                    obstacles.push_back(vector2(event.button.x, event.button.y));
                    prevObstacle.set(event.button.x, event.button.y);
                }

                if(event.button.button == SDL_BUTTON_MIDDLE){
                    obstacles.clear();
                }

                if(event.button.button == SDL_BUTTON_RIGHT){
                    isErasing = true;
                }
            }

            if(event.type == SDL_MOUSEBUTTONUP){
                if(event.button.button == SDL_BUTTON_LEFT){
                    isMousePressed = false;
                }

                if(event.button.button == SDL_BUTTON_RIGHT){
                    isErasing = false;
                }
            }

            if(event.type == SDL_MOUSEMOTION){
                if(isMousePressed){
                    if(!isColliding(event.button.x, event.button.y, prevObstacle.x, prevObstacle.y)) {
                        obstacles.push_back(vector2(event.button.x, event.button.y));
                        prevObstacle.set(event.button.x, event.button.y);
                    }
                }
            }
        }

        SDL_RenderPresent(renderer);
        SDL_SetRenderDrawColor(renderer, 51, 51, 51, 255);
        SDL_RenderClear(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

