#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <cstdio>
#include <string>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

class LTexture
{
    public:
        // Initialize
        LTexture();

        // Deallocate
        ~LTexture();

        // Load image at specified path
        bool loadFromFile(std::string path);

        // Deallocates texture
        void free();

        // Set color modulation
        void setColor(Uint8 red, Uint8 green, Uint8 blue);

        // Set blending
        void setBlendMode(SDL_BlendMode blending);

        // Set alpha modulation
        void setAlpha(Uint8 alpha);

        // Renders texture at given point
        void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

        // Gets image dimensions
        int getWidth();
        int getHeight();

    private:
        // The actual hardware texture
        SDL_Texture* mTexture;

        int mWidth;
        int mHeight;
};

bool init();

bool loadMedia();

void close();

SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

LTexture gArrowTexture;

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
        printf("Failed to load image %s! SDL_Image Error: %s\n", path.c_str(), IMG_GetError());
    } else {
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (newTexture == NULL) {
            printf("Failed to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
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

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};

    if (clip != NULL) {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth()
{
    return mWidth;
}

int LTexture::getHeight()
{
    return mHeight;
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
                if (!(IMG_Init(imgFlag) & imgFlag)) {
                    printf("SDL Image failed to initialize! SDL_Image Error: %s\n", IMG_GetError());
                    success = false;
                }
            }
        }
    }

    return true;
}

bool loadMedia()
{
    bool success = true;

    if (!gArrowTexture.loadFromFile("arrow.png")) {
        printf("Failed to arrow texture!\n");
        success = false;
    }

    return success;
}

void close()
{
    gArrowTexture.free();

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

            double degrees = 0;

            SDL_RendererFlip flipType = SDL_FLIP_NONE;

            while(!quit) {
                while (SDL_PollEvent(&e) != 0) {
                    if (e.type == SDL_QUIT) {
                        quit = true;
                    } else if (e.type == SDL_KEYDOWN) {
                        switch(e.key.keysym.sym) {
                            case SDLK_a:
                            degrees -= 60;
                            break;

                            case SDLK_d:
                            degrees += 60;
                            break;

                            case SDLK_q:
                            flipType = SDL_FLIP_HORIZONTAL;
                            break;

                            case SDLK_w:
                            flipType = SDL_FLIP_NONE;
                            break;

                            case SDLK_e:
                            flipType = SDL_FLIP_VERTICAL;
                            break;
                        }
                    }
                }

                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);

                gArrowTexture.render((SCREEN_WIDTH - gArrowTexture.getWidth()) / 2, (SCREEN_HEIGHT - gArrowTexture.getHeight()) / 2, NULL, degrees, NULL, flipType);

                SDL_RenderPresent(gRenderer);
            }
        }
    }

    close();

    return 0;
}

