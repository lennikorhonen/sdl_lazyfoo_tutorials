#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <cstdio>

#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 1080

// The window we'll be rendering
SDL_Window* gWindow = NULL;

// The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

// The image we will load and show on screen
SDL_Surface* gHelloWorld = NULL;

// Start up SDL and create a window
bool init()
{
    // Initialization flag
    bool success = true;

    // Initialize SDL
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0)
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        // Create window
        gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if ( gWindow == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            // Get window surface
            gScreenSurface = SDL_GetWindowSurface( gWindow );
        }
    }

    return success;
}

// Loads media
bool loadMedia()
{
    // Loading success flag
    bool success = true;

    // Load splash image
    gHelloWorld = SDL_LoadBMP( "./headphones_stay_on_copy.bmp" );
    if ( gHelloWorld == NULL )
    {
        printf( "Unable to load image %s! SDL Error: %s\n", "headphones_stay_on_copy.bmp", SDL_GetError() );
        success = false;
    }

    return success;
}

// Frees media and shuts down SDL
void close()
{
    // Deallocate surface 
    SDL_FreeSurface( gHelloWorld );
    gHelloWorld = NULL;

    // Destroy window
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;

    // Quit SDL subsystems
    SDL_Quit();
}

int main (int argc, char *argv[])
{
    // Start up SDL and create window
    if ( !init() )
    {
        printf( "Failed to initialize!\n" );
    }
    else 
    {
        // Load media()
        if ( !loadMedia() )
        {
            printf( "Failed to load media!\n" );
        }
        else
        {
            // Apply the image
            SDL_BlitSurface( gHelloWorld, NULL, gScreenSurface, NULL);

            // Update the surface
            SDL_UpdateWindowSurface( gWindow );

            // Hack to get window stay up
            SDL_Event e;

            bool quit = false;

            while ( !quit )
            {
                while( SDL_PollEvent( &e ) )
                {
                    if (e.type == SDL_QUIT ) 
                    {
                        quit = true;
                    }
                }
            }
        }
    }

    // Free resources and close SDL
    close();

    return 0;
}
