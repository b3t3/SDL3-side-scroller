#pragma once
#include <SDL3/SDL.h>
#include <stdbool.h>

class Menu {
public:
    Menu(SDL_Renderer* renderer);
    
    void handleEvent(const SDL_Event& event);
    void render();
    bool shouldStartGame() const;

private:
    SDL_Renderer* renderer_;
    bool startGame;

};
