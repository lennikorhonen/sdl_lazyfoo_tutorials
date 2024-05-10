#include <SDL2/SDL.h>
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <cstddef>
#include <cstdio>
#include <string>

// Screen size constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Button constants
const int BUTTON_WIDTH = 300;
const int BUTTON_HEIGHT = 200;
const int TOTAL_BUTTONS = 4;

enum LButtonSprite
{
    BUTTON_SPRITE_MOUSE_OUT = 0,
    BUTTON_SPRITE_MOUSE_OVER_MOTION = 1,
    BUTTON_SPRITE_MOUSE_DOWN = 2,
    BUTTON_SPRITE_MOUSE_UP = 3,
    BUTTON_SPRITE_TOTAL = 4
};

class LTexture
{
    public:
        // Initialize
        LTexture();

        // Deallocate
        ~LTexture();

        // Load file at specified path
        bool loadFromFile(std::string path);

        #if defined (SDL_TTF_MAJOR_VERSION)
        // Creates image from font string
        bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
        #endif

        // Deallocate texture
        void free();

        // Set color modulation
        void setColor(Uint8 red, Uint8 green, Uint8 blue);

        // Set blending
        void setBlendMode(SDL_BlendMode blending);

        // Set alpha modulation
        void setAlpha(Uint8);

        // Renders texture at given point
        void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

        // Gets image dimensions
        int getWidth();
        int getHeight();

    private:
        // The acutal hardware texture
        SDL_Texture* mTexture;

        // Texture dimensions
        int mWidth;
        int mHeight;
};

class LButton
{
    public:
        // Initializes internal variables
        LButton();

        // Sets top left position
        void setPosition(int x, int y);

        // Handles mouse event
        void handleEvent(SDL_Event* e);

        // Shows button sprite
        void render();
    private:
        // Top left position
        SDL_Point mPosition;

        // Currently used global sprite
        LButtonSprite mCurrentSprite;
};

bool init();

bool loadMedia();

void close();

SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

SDL_Rect gSpriteClips[BUTTON_SPRITE_TOTAL];
LTexture gButtonSpriteSheetTexture;

LButton gButtons[TOTAL_BUTTONS];

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
            printf("Failed to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
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

LButton::LButton()
{
    mPosition.x = 0;
    mPosition.y = 0;

    mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
}

void LButton::setPosition(int x, int y)
{
    mPosition.x = x;
    mPosition.y = y;
}

void LButton::handleEvent(SDL_Event* e)
{
    // If mouse event happened
    if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP) {
        // Get mouse position
        int x, y;
        SDL_GetMouseState(&x, &y);

        // Check if mouse is in button
        bool inside = true;

        // Mouse is left of the button
        if (x < mPosition.x) {
            inside = false;
        }
        // Mouse is right of the button
        else if (x > mPosition.x + BUTTON_WIDTH) {
            inside = false;
        }
        // Mouse above the position
        else if (y < mPosition.y) {
            inside = false;
        }
        // Mouse below the button
        else if (y > mPosition.y + BUTTON_HEIGHT) {
            inside = false;
        }

        // Mouse is outside button
        if (!inside) {
            mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
        }
        // Mouse is inside button
        else {
            // Set mouse over sprite
            switch(e->type) {
                case SDL_MOUSEMOTION:
                mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
                break;

                case SDL_MOUSEBUTTONDOWN:
                mCurrentSprite = BUTTON_SPRITE_MOUSE_DOWN;
                break;

                case SDL_MOUSEBUTTONUP:
                mCurrentSprite = BUTTON_SPRITE_MOUSE_UP;
                break;
            }
        }
    }
}

void LButton::render()
{
    // Show current button sprite
    gButtonSpriteSheetTexture.render(mPosition.x, mPosition.y, &gSpriteClips[mCurrentSprite]);
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
                    printf("SDL_Image failed to initialize! SDL_Image Error: %s\n", SDL_GetError());
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

    if (!gButtonSpriteSheetTexture.loadFromFile("button.png")) {
        printf("Failed to load button texture!\n");
        success = false;
    } else {
        for (int i = 0; i < BUTTON_SPRITE_TOTAL; ++i) {
            gSpriteClips[i].x = 0;
            gSpriteClips[i].y = i * 200;
            gSpriteClips[i].w = BUTTON_WIDTH;
            gSpriteClips[i].h = BUTTON_HEIGHT;
        }

        gButtons[0].setPosition(0, 0);
        gButtons[1].setPosition(SCREEN_WIDTH - BUTTON_WIDTH, 0);
        gButtons[2].setPosition(0, SCREEN_HEIGHT - BUTTON_HEIGHT);
        gButtons[3].setPosition(SCREEN_WIDTH - BUTTON_WIDTH, SCREEN_HEIGHT - BUTTON_HEIGHT);
    }

    return success;
}

void close()
{
    gButtonSpriteSheetTexture.free();

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
                    // User requests quit
                    if (e.type == SDL_QUIT) {
                        quit = true;
                    }

                    for (int i = 0; i < TOTAL_BUTTONS; ++i) {
                        gButtons[i].handleEvent(&e);
                    }
                }

                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);

                for (int i = 0; i < TOTAL_BUTTONS; ++i) {
                    gButtons[i].render();
                }

                SDL_RenderPresent(gRenderer);
            }
        }
    }

    close();

    return 0;
}
