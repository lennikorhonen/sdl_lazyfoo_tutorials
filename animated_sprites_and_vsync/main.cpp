#include <SDL2/SDL.h>
#include <SDL2/SDL_blendmode.h>
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
const int WALKING_ANIMATION_FRAMES = 4;

class LTexture
{
    public:
        // Initialize
        LTexture();

        // Deallocate
        ~LTexture();

        // Load image from specified path
        bool loadFromFile(std::string path);

        // Deallocate texture
        void free();

        // Set color modulation
        void setColor(Uint8 red, Uint8 green, Uint8 blue);

        // Set blending
        void setBlendMode(SDL_BlendMode blending);

        // Set alpha modulation
        void setAlpha(Uint8 alpha);

        // Render
        void render(int x, int y, SDL_Rect* clip = NULL);

        // Image dimensions
        int getWidth();
        int getHeight();

    private:
        // Actual hardware texture
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

SDL_Rect gSpriteClips[WALKING_ANIMATION_FRAMES];
LTexture gSpriteSheetTexture;

LTexture::LTexture()
{
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
    free();
}

bool LTexture::loadFromFile(std::string path)
{
    SDL_Texture* newTexture = NULL;

    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        printf("Unable to load image %s! SDL_Image Error: %s\n", path.c_str(), IMG_GetError());
    } else {
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (newTexture == NULL) {
            printf("Unable to create texture from %s! SDL_Image Error: %s\n", path.c_str(), SDL_GetError());
        } else {
            SDL_SetColorKey(loadedSurface, 0xFF, 0xFF);

            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        SDL_FreeSurface(loadedSurface);
    }

    mTexture = newTexture;
    return mTexture != NULL;
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
    SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
    SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
    SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect* clip)
{
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};

    if (clip != NULL) {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuad);
}

bool init()
{
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL failed to initialize!\n");
        success = false;
    } else {
        gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == NULL) {
            printf("Failed to create window! SDL Error: %s\n", SDL_GetError());
            success = false;
        } else {
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if (gRenderer == NULL) {
                printf("Failed to create renderer! SDL Error: %s\n", SDL_GetError());
                success = false;
            } else {
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

                int imgFlag = IMG_INIT_PNG;
                if (!(IMG_Init(IMG_INIT_PNG) & imgFlag)) {
                    printf("SDL_Image failed to initialize! SDL_Image Error: %s\n", IMG_GetError());
                    success = false;
                }
            }
        }
    }

    return success;
}

bool loadMedia()
{
    bool success = true;

    if (!gSpriteSheetTexture.loadFromFile("foo.png")) {
        printf("Failed to load walking animation texture!\n");
        success = false;
    } else {
        // Set the sprite clips
        gSpriteClips[0].x = 0;
        gSpriteClips[0].y = 0;
        gSpriteClips[0].w = 64;
        gSpriteClips[0].h = 205;

        gSpriteClips[1].x = 64;
        gSpriteClips[1].y = 0;
        gSpriteClips[1].w = 64;
        gSpriteClips[1].h = 205;

        gSpriteClips[2].x = 128;
        gSpriteClips[2].y = 0;
        gSpriteClips[2].w = 64;
        gSpriteClips[2].h = 205;

        gSpriteClips[3].x = 192;
        gSpriteClips[3].y = 0;
        gSpriteClips[3].w = 64;
        gSpriteClips[3].h = 205;
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

int main (int argc, char *argv[]) {
    if (!init()) {
        printf("Failed to initialize!\n");
    } else {
        if (!loadMedia()) {
            printf("Failed to load media!\n");
        } else {
            bool quit = false;

            SDL_Event e;

            int frame = 0;

            while (!quit) {
                while (SDL_PollEvent(&e) != 0) {
                    if (e.type == SDL_QUIT) {
                        quit = true;
                    }
                }

                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);

                SDL_Rect* currentClip = &gSpriteClips[frame / 10];
                gSpriteSheetTexture.render((SCREEN_WIDTH - currentClip->w) / 2, (SCREEN_HEIGHT - currentClip->h) / 2, currentClip);

                SDL_RenderPresent(gRenderer);

                ++frame;

                if (frame / 10 >= WALKING_ANIMATION_FRAMES) {
                    frame = 0;
                }
            }
        }
    }

    close();

    return 0;
}
