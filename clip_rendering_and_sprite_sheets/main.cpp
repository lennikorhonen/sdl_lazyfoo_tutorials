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

// Texture wrapper class
class LTexture
{
    public:
        // Initialized variable
        LTexture();

        // Deallocates memory
        ~LTexture();

        // Loads image from specified path
        bool loadFromFile(std::string path);

        // Deallocates texture
        void free();

        // Render texture at given point
        void render(int x, int y, SDL_Rect* clip = NULL);

        // Gets image dimensions
        int getWidth();
        int getHeight();

    private:
        // The acutal hardware texture
        SDL_Texture* mTexture;

        // Image dimensions
        int mWidth;
        int mHeight;
};

bool init();

bool loadMedia();

void close();

SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

// Scene sprites
SDL_Rect gSpriteClips[4];
LTexture gSpriteSheetTexture;

bool init()
{
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL failed to initialize!\n");
        success = false;
    } else {
        gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == NULL) {
            printf("Failed to create a window! SDL Error: %s\n", SDL_GetError());
            success = false;
        } else {
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if (gRenderer == NULL) {
                printf("Failed to create renderer! SDL Error: %s\n", SDL_GetError());
                success = false;
            } else {
                int imgFlag = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlag) & imgFlag)) {
                    printf("SDL_Image failed to initialize! SDL_Image Error: %s\n", IMG_GetError());
                    success = false;
                }
            }
        }
    }

    return success;
}

LTexture::LTexture()
{
    // Initialize
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

void LTexture::free()
{
    if (mTexture != NULL) {
        SDL_DestroyTexture(mTexture);
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

LTexture::~LTexture()
{
    // Deallocate
    free();
}

bool LTexture::loadFromFile(std::string path)
{
    // Get rid of preexisting texture
    free();

    // The final texture
    SDL_Texture* newTexture = NULL;

    // Load image from path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_Image Error: %s\n", path.c_str(), IMG_GetError());
    } else {
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (newTexture == NULL) {
            printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        } else {
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        SDL_FreeSurface(loadedSurface);
    }

    mTexture = newTexture;
    return mTexture != NULL;
}

void LTexture::render(int x, int y, SDL_Rect* clip)
{
    // Set rendering space and render to screen
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};

    // Set clip rendering dimensions
    if (clip != NULL) {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    // Render to screen
    SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuad);
}

int LTexture::getWidth()
{
    return mWidth;
}

int LTexture::getHeight()
{
    return mHeight;
}

bool loadMedia()
{
    bool success = true;

    if (!gSpriteSheetTexture.loadFromFile("dots.png")) {
        printf("Failed to load sprite sheet texture\n");
        success = false;
    } else {
        // Set top left sprite
        gSpriteClips[0].x = 0;
        gSpriteClips[0].y = 0;
        gSpriteClips[0].w = 100;
        gSpriteClips[0].h = 100;

        // Set top right sprite
        gSpriteClips[1].x = 100;
        gSpriteClips[1].y = 0;
        gSpriteClips[1].w = 100;
        gSpriteClips[1].h = 100;

        // Set bottom left sprite
        gSpriteClips[2].x = 0;
        gSpriteClips[2].y = 100;
        gSpriteClips[2].w = 100;
        gSpriteClips[2].h = 100;

        // Set bottom right sprite
        gSpriteClips[3].x = 100;
        gSpriteClips[3].y = 100;
        gSpriteClips[3].w = 100;
        gSpriteClips[3].h = 100;
    }

    return success;
}

void close()
{
    gSpriteSheetTexture.free();

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

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

                // Clear screen
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);

                // Render top left sprite
                gSpriteSheetTexture.render(0, 0, &gSpriteClips[0]);

                // Render top right sprite
                gSpriteSheetTexture.render(SCREEN_WIDTH - gSpriteClips[1].w, 0, &gSpriteClips[1]);

                // Render bottom left sprite
                gSpriteSheetTexture.render(0, SCREEN_HEIGHT - gSpriteClips[2].h, &gSpriteClips[2]);

                // Render bottom right sprite
                gSpriteSheetTexture.render(SCREEN_WIDTH - gSpriteClips[3].w, SCREEN_HEIGHT - gSpriteClips[3].h, &gSpriteClips[3]);

                // Update screen
                SDL_RenderPresent(gRenderer);
            }
        }
    }

    close();
    
    return 0;
}
