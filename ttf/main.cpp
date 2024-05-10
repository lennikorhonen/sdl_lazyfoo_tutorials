#include <SDL2/SDL.h>
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <cstdio>
#include <string>
#include <cmath>

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

        // Creates image from font string
        bool loadFromRenderedText(std::string textureText, SDL_Color textColor);

        // Deallocate texture
        void free();

        // Set color modulation
        void setColor(Uint8 red, Uint8 green, Uint8 blue);

        // Set blending
        void setBlending(SDL_BlendMode blending);

        // Set alpha modulation
        void setAlpha(Uint8 alpha);

        // Renders texture at given point
        void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

        // Gets image dimensions;
        int getWidth();
        int getHeight();
        
    private:
        // The actual hardware texture
        SDL_Texture* mTexture;

        // Dimensions
        int mWidth;
        int mHeight;
};

bool init();

bool loadMedia();

void close();

SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

TTF_Font* gFont = NULL;

LTexture gTextTexture;

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
        printf("Failed to load image from %s! SDL_Image Error: %s\n", path.c_str(), IMG_GetError());
    } else {
        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (newTexture == NULL) {
            printf("Failed to create texture from %s! SDL Error: %s\n", path.c_str(), IMG_GetError());
        } else {
            SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        SDL_FreeSurface(loadedSurface);
    }

    mTexture = newTexture;
    return mTexture != NULL;
}

bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
    free();

    // Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
    if (textSurface == NULL) {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    } else {
        // Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
        if (mTexture == NULL) {
            printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        } else {
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }

        SDL_FreeSurface(textSurface);
    }

    return mTexture != NULL;
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
    SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlending(SDL_BlendMode blending)
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

                if (TTF_Init() == -1) {
                    printf("SDL_ttf failed to initialize! SDL_ttf Error: %s\n", TTF_GetError());
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

    gFont = TTF_OpenFont("lazy.ttf", 28);
    if (gFont == NULL) {
        printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
        success = false;
    } else {
        SDL_Color textColor = {0, 0, 0};
        if (!gTextTexture.loadFromRenderedText("The quick brown fox jumps over the lazy dog", textColor)) {
            printf("Failed to render text texture!\n");
            success = false;
        }
    }

    return success;
}

void close()
{
    gTextTexture.free();

    TTF_CloseFont(gFont);
    gFont = NULL;

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    TTF_Quit();
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
                while (SDL_PollEvent(&e)) {
                    if (e.type == SDL_QUIT) {
                        quit = true;
                    }
                }

                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);

                gTextTexture.render((SCREEN_WIDTH - gTextTexture.getWidth()) / 2, (SCREEN_HEIGHT - gTextTexture.getHeight()) / 2);

                SDL_RenderPresent(gRenderer);
            }
        }
    }

    close();

    return 0;
}
