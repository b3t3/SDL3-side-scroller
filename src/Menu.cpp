#include "Menu.h"
#include <SDL3/SDL.h>

Menu::Menu(SDL_Renderer* renderer)
    : renderer_(renderer), startGame(false) 
{
}

void Menu::handleEvent(const SDL_Event& e)
{
    if (e.type == SDL_EVENT_KEY_DOWN) {
        if (e.key.scancode == SDL_SCANCODE_RETURN || e.key.scancode == SDL_SCANCODE_SPACE) {
            startGame = true;
        }
    }
    
    if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        startGame = true;
    }
}

void Menu::reset() {
    startGame = false;
}

void Menu::render()
{
    SDL_SetRenderDrawColor(renderer_, 40, 0, 0, 255);  
    SDL_RenderClear(renderer_);

    SDL_SetRenderDrawColor(renderer_, 60, 0, 0, 255);  
    for (int i = 0; i < 10; i++) {
        SDL_FRect pattern = { 
            static_cast<float>(i * 80), 
            0, 
            80.0f, 
            600.0f 
        };
        if (i % 2 == 0) {
            SDL_RenderFillRect(renderer_, &pattern);
        }
    }

    // Main title panel - dark red with black border
    SDL_SetRenderDrawColor(renderer_, 100, 0, 0, 255);  // Dark red
    SDL_FRect titlePanel = { 100, 80, 600, 120 };
    SDL_RenderFillRect(renderer_, &titlePanel);
    
    // Black border around title
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_FRect titleBorder = { 98, 78, 604, 124 };
    SDL_RenderRect(renderer_, &titleBorder);

    // Start button - black with dark red border
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);  // Black
    SDL_FRect startButton = { 250, 300, 300, 80 };
    SDL_RenderFillRect(renderer_, &startButton);
    
    // Dark red border around start button
    SDL_SetRenderDrawColor(renderer_, 120, 0, 0, 255);
    SDL_FRect buttonBorder = { 248, 298, 304, 84 };
    SDL_RenderRect(renderer_, &buttonBorder);

    SDL_SetRenderDrawColor(renderer_, 180, 40, 40, 255);  // Lighter red for text
    
    // Title text "RED VEIL"
    SDL_FRect titleText1 = { 220, 110, 60, 15 };
    SDL_FRect titleText2 = { 290, 110, 60, 15 };
    SDL_FRect titleText3 = { 360, 110, 60, 15 };
    SDL_FRect titleText4 = { 430, 110, 60, 15 };
    SDL_RenderFillRect(renderer_, &titleText1);
    SDL_RenderFillRect(renderer_, &titleText2);
    SDL_RenderFillRect(renderer_, &titleText3);
    SDL_RenderFillRect(renderer_, &titleText4);

    // "START GAME" text on button
    SDL_FRect startText1 = { 280, 325, 50, 10 };
    SDL_FRect startText2 = { 340, 325, 50, 10 };
    SDL_FRect startText3 = { 400, 325, 50, 10 };
    SDL_RenderFillRect(renderer_, &startText1);
    SDL_RenderFillRect(renderer_, &startText2);
    SDL_RenderFillRect(renderer_, &startText3);

    // Prompt text at bottom - dark red
    SDL_SetRenderDrawColor(renderer_, 100, 0, 0, 255);
    SDL_FRect prompt1 = { 200, 450, 100, 8 };
    SDL_FRect prompt2 = { 310, 450, 100, 8 };
    SDL_FRect prompt3 = { 420, 450, 100, 8 };
    SDL_RenderFillRect(renderer_, &prompt1);
    SDL_RenderFillRect(renderer_, &prompt2);
    SDL_RenderFillRect(renderer_, &prompt3);

    // Decorative
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    
    // Top left corner accent
    SDL_FRect accent1 = { 50, 50, 20, 20 };
    SDL_RenderFillRect(renderer_, &accent1);
    
    // Top right corner accent
    SDL_FRect accent2 = { 730, 50, 20, 20 };
    SDL_RenderFillRect(renderer_, &accent2);
    
    // Bottom left corner accent
    SDL_FRect accent3 = { 50, 530, 20, 20 };
    SDL_RenderFillRect(renderer_, &accent3);
    
    // Bottom right corner accent
    SDL_FRect accent4 = { 730, 530, 20, 20 };
    SDL_RenderFillRect(renderer_, &accent4);

    SDL_RenderPresent(renderer_);
}

bool Menu::shouldStartGame() const {
    return startGame;
}
