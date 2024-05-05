#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <cstdio>
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

bool init();

bool loadMedia();

void close();

SDL_Texture* loadTexture(std::string path);

SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

SDL_Texture* gTexture = NULL;

bool init()
{
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to initialize SDL!\n");
        success = false;
    } else {
        gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == NULL) {
            printf("Failed to create window! SDL Error: %s\n", SDL_GetError());
            success = false;
        } else {
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if (gRenderer == NULL) {
                printf("Failed to create renderer! SDL Error: %s\n", SDL_GetError());
                success = false;
            } else {
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

                int imgFlag = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlag) & imgFlag)) {
                    printf("SDL_Image could not initialize! SDL_image Error: %s\n", IMG_GetError());
                }
            }
        }
    }
    
    return success;
}

SDL_Texture* loadTexture(std::string path)
{
    SDL_Texture* newTexture = NULL;

    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        printf("Failed to load image %s! SDL_image Error: %s\n", path.c_str());
    } else {
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (newTexture == NULL) {
            printf("Failed to create texture from surface %s! SDL_Error: %s\n", path.c_str(), SDL_GetError());
        }

        SDL_FreeSurface(loadedSurface);
    }

    return newTexture;
}

bool loadMedia()
{
    bool success = true;
    gTexture = loadTexture("viewport.png");
    if (gTexture == NULL) {
        printf("Failed to load texture %s! SDL_Error: %s\n", "viewport.png", SDL_GetError());
        success = false;
    }

    return success;
}

void close()
{
    SDL_DestroyTexture(gTexture);
    gTexture = NULL;

    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;

    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    IMG_Quit();
    SDL_Quit();
}

int main (int argc, char *argv[])
{
    if (!init()) {
        printf("Failed to initialize!\n");
    } else {
        if (!loadMedia()) {
            printf("Failed to load media!\n");
        } else {
            bool quit = false;

            SDL_Event e;

            while (!quit) {
                while (SDL_PollEvent(&e) != 0) {
                    if (e.type == SDL_QUIT) {
                        quit = true;
                    }
                }

                // Top left corner viewport
                SDL_Rect topLeftViewport;
                topLeftViewport.x = 0;
                topLeftViewport.y = 0;
                topLeftViewport.w = SCREEN_WIDTH / 2;
                topLeftViewport.h = SCREEN_HEIGHT / 2;
                SDL_RenderSetViewport(gRenderer, &topLeftViewport);

                // Render texture to screen
                SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);

                SDL_Rect topRightViewport;
                topRightViewport.x = SCREEN_WIDTH / 2;
                topRightViewport.y = 0;
                topRightViewport.w = SCREEN_WIDTH / 2;
                topRightViewport.h = SCREEN_HEIGHT / 2;
                SDL_RenderSetViewport(gRenderer, &topRightViewport);

                // Render texture to screen
                SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);

                SDL_Rect bottomViewport;
                bottomViewport.x = 0;
                bottomViewport.y = SCREEN_HEIGHT / 2;
                bottomViewport.w = SCREEN_WIDTH;
                bottomViewport.h = SCREEN_HEIGHT / 2;
                SDL_RenderSetViewport(gRenderer, &bottomViewport);

                // Render texture to screen
                SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);

                SDL_RenderPresent(gRenderer);
            }
        }
    }

    close();
    
    return 0;
}
