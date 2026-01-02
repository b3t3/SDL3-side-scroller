#include "Menu.h"
#include <SDL3/SDL.h>

Menu::Menu(SDL_Renderer* renderer)
    : renderer_(renderer), startGame(false) 
    {
    }

void Menu::handleEvent(const SDL_Event& e)
{
    if (e.type == SDL_EVENT_KEY_DOWN) {
        if (e.key.key == SDLK_RETURN || e.key.key == SDLK_SPACE) {
            startGame = true;
        }
    }
    
    if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        startGame = true;
    }
}

void Menu::render()
{
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);

    SDL_ SetRenderDrawColor (renderer_, 180, 0, 0, 255);
    SDL_Rect title = { 150, 100, 400, 80};

    SDL_FRect prompt = { 200, 300, 400, 80};
    
    SDL_RenderPresent(renderer_, &title);
    SDL_RenderPresent(renderer_, &prompt);

    SDL_RenderPresent(renderer_);
}

bool Menu::shouldStartGame() const {
    return startGame;
}
