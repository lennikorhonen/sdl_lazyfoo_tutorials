#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>
#include <cstdio>
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

bool init();

bool loadMedia();

void close();

SDL_Window* gWindow = NULL;

SDL_Surface* gScreenSurface = NULL;

SDL_Surface* loadSurface( std::string path );

SDL_Surface* gImgSurface = NULL;

bool init()
{
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    } else {
        gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == NULL) {
            printf("Unable to create window! SDL Error: %s\n", SDL_GetError());
            success = false;
        } else {
            int imgFlags = IMG_INIT_PNG;
            if (!(IMG_Init(imgFlags) & imgFlags)) {
                printf("SDL image could not initialize! SDL_image Error: %s\n", IMG_GetError());
                success = false;
            } else {
                gScreenSurface = SDL_GetWindowSurface(gWindow);
            }
        }
    }

    return success;
}

SDL_Surface* loadSurface(std::string path)
{
    SDL_Surface* optimizedSurface = NULL;

    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
    } else {
        optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
        if (optimizedSurface == NULL) {
            printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }

        SDL_FreeSurface(loadedSurface);
    }

    return  optimizedSurface;
}

bool loadMedia()
{
    bool success = true;
    gImgSurface = loadSurface("loaded.png");
    if (gImgSurface == NULL) {
        printf("Unable to load media %s! SDL Error %s\n", "loaded.png", SDL_GetError());
        success = false;
    }

    return success;
}

void close()
{
    SDL_FreeSurface(gImgSurface);
    gImgSurface = NULL;

    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

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

                SDL_Rect stretchRect;
                stretchRect.x = 0;
                stretchRect.y = 0;
                stretchRect.w = SCREEN_WIDTH;
                stretchRect.h = SCREEN_HEIGHT;

                SDL_BlitScaled(gImgSurface, NULL, gScreenSurface, &stretchRect);
                SDL_UpdateWindowSurface(gWindow);
            }
        }
    }
    
    close();

    return 0;
}
