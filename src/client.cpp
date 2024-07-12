#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <locale>
#include <codecvt>
#include "KaraokeTrack.h"

const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;

std::ofstream logFile;

enum class Scene {
    MENU,
    TRACK_LOADED
};

typedef struct Button {
    SDL_Rect rect;
    std::string text;
    void (*onClick)();
} Button;

Scene currentScene = Scene::MENU;
KaraokeTrack loadedTrack;
std::vector<Button> menuButtons;
std::vector<Button> trackButtons;
TTF_Font* font = nullptr;

void log(const std::string& message) {
    if (!logFile.is_open()) {
        logFile.open("log.txt", std::ios::app);
    }
    
    std::time_t now = std::time(0);
    char* dt = std::ctime(&now);
    logFile << dt << message << std::endl << std::endl;
    
    // Also print to cout for debugging when running from command line
    std::cout << dt << message << std::endl;
}

void openFile() {
    try {
        loadedTrack.loadFromFile("tracks/test.json");
        log("Karaoke track loaded successfully: " + loadedTrack.metadata.title);
        currentScene = Scene::TRACK_LOADED;
    } catch (const std::exception& e) {
        log("Failed to load karaoke track: " + std::string(e.what()));
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to load karaoke track. Check log for details.", NULL);
    }
}

void playTrack() {
    // TODO implement this later
    log("Playing track: " + loadedTrack.metadata.title);
}

void renderText(SDL_Renderer* renderer, const std::string& text, int x, int y) {
    SDL_Color textColor = {255, 255, 255};
    SDL_Surface* surface = TTF_RenderUTF8_Solid(font, text.c_str(), textColor);
    if (!surface) {
        log("Failed to render text: " + std::string(TTF_GetError()));
        return;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        log("Failed to create texture from rendered text: " + std::string(SDL_GetError()));
        SDL_FreeSurface(surface);
        return;
    }
    
    int textWidth = surface->w;
    int textHeight = surface->h;
    SDL_Rect renderQuad = { x, y, textWidth, textHeight };
    
    SDL_RenderCopy(renderer, texture, NULL, &renderQuad);
    
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void renderButton(SDL_Renderer* renderer, const Button& button) {
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);  // Gray color for button
    SDL_RenderFillRect(renderer, &button.rect);
    renderText(renderer, button.text, button.rect.x + 10, button.rect.y + 10);
}

void handleMouseClick(int x, int y, const std::vector<Button>& buttons) {
    for (const auto& button : buttons) {
        if (x >= button.rect.x && x <= button.rect.x + button.rect.w &&
            y >= button.rect.y && y <= button.rect.y + button.rect.h) {
            if (button.onClick) {
                button.onClick();
            }
            break;
        }
    }
}

#ifdef _WIN32
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

int main(int argc, char* argv[]) {
    try {
        std::locale::global(std::locale("en_US.UTF-8"));
    } catch (const std::runtime_error&) {
        try {
            std::locale::global(std::locale(""));
        } catch (const std::runtime_error&) {
            std::locale::global(std::locale("C"));
        }
    }

    std::locale loc(std::locale(), new std::codecvt_utf8<wchar_t>);
    std::wcout.imbue(loc);
    std::wcin.imbue(loc);

    log("Karaoke application started.");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        log("SDL could not initialize! SDL_Error: " + std::string(SDL_GetError()));
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "SDL could not initialize!", NULL);
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        log("SDL_mixer could not initialize! SDL_mixer Error: " + std::string(Mix_GetError()));
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "SDL_mixer could not initialize!", NULL);
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Karaoke Client", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        log("Window could not be created! SDL_Error: " + std::string(SDL_GetError()));
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Window could not be created!", NULL);
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        log("Renderer could not be created! SDL Error: " + std::string(SDL_GetError()));
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Renderer could not be created!", NULL);
        return 1;
    }
    
    if (TTF_Init() == -1) {
        log("SDL_ttf could not initialize! SDL_ttf Error: " + std::string(TTF_GetError()));
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "SDL_ttf could not initialize!", NULL);
        return 1;
    }

    font = TTF_OpenFont("C:\\Windows\\Fonts\\msgothic.ttc", 24);
    if (font == NULL) {
        log("Failed to load font! SDL_ttf Error: " + std::string(TTF_GetError()));
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to load font!", NULL);
        return 1;
    }

    menuButtons.push_back({{SCREEN_WIDTH/2 - 50, SCREEN_HEIGHT/2 - 25, 100, 50}, "Open", openFile});
    trackButtons.push_back({{SCREEN_WIDTH/2 - 50, SCREEN_HEIGHT/2 - 25, 100, 50}, "Play", playTrack});

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                if (currentScene == Scene::MENU) {
                    handleMouseClick(mouseX, mouseY, menuButtons);
                } else if (currentScene == Scene::TRACK_LOADED) {
                    handleMouseClick(mouseX, mouseY, trackButtons);
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255);
        SDL_RenderClear(renderer);

        if (currentScene == Scene::MENU) {
            for (const auto& button : menuButtons) {
                renderButton(renderer, button);
            }
        } else if (currentScene == Scene::TRACK_LOADED) {
            renderText(renderer, "Track: " + loadedTrack.metadata.title, 10, 10);
            renderText(renderer, "Artist: " + loadedTrack.metadata.artist, 10, 40);
            for (const auto& button : trackButtons) {
                renderButton(renderer, button);
            }
        }

        SDL_RenderPresent(renderer);
    }

    log("Karaoke application closing.");
    logFile.close();
    return 0;
}