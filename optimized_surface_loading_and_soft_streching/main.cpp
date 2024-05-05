#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <cstdio>
#include <string>

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

bool init();

bool loadMedia();

void close();

SDL_Window* gWindow = NULL;

SDL_Surface* gScreenSurface = NULL;

SDL_Surface* loadSurface( std::string path );

SDL_Surface* gStretchedSurface = NULL;

bool init()
{
    bool success = true;

    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    }
    else {
        gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == NULL) {
            printf("Failed to create window! SDL Error %s\n", SDL_GetError());
            success = false;
        }
        else {
            gScreenSurface = SDL_GetWindowSurface(gWindow);
        }
    }

    return success;
}

SDL_Surface* loadSurface( std::string path )
{
    SDL_Surface* optimizedSurface = NULL;

    SDL_Surface* loadedSurface = SDL_LoadBMP(path.c_str());
    if (loadedSurface == NULL) {
        printf("Failed to load surface %s\n", SDL_GetError());
    }
    else {
        optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
        if (optimizedSurface == NULL) {
            printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        }

        SDL_FreeSurface(loadedSurface);
    }

    return optimizedSurface;
}

bool loadMedia()
{
    bool success = true;

    gStretchedSurface = loadSurface("stretch.bmp");
    if (gStretchedSurface == NULL) {
        printf("Unable to load image %s! SDL Error: %s\n", "strecth.bmp", SDL_GetError());
        success = false;
    }

    return success;
}

void close()
{
    SDL_FreeSurface(gStretchedSurface);
    gStretchedSurface = NULL;

    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    SDL_Quit();
}

int main (int argc, char *argv[])
{
    if (!init()) {
        printf("Failed to initialize!\n");
    }
    else {
        if (!loadMedia()) {
            printf("Failed to load media!\n");
        }
        else {
            bool quit = false;

            SDL_Event e;

            // Main game loop. We wait for SDL_QUIT event and loop until that event happens
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
                SDL_BlitScaled(gStretchedSurface, NULL, gScreenSurface, &stretchRect);
                SDL_UpdateWindowSurface(gWindow);
            }
        }
    }
    close();
    
    return 0;
}
