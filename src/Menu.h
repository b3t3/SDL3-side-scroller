#pragma once
#include <SDL3/SDL.h>

class Menu {
public:
    Menu(SDL_Renderer* renderer);
    
    void handleEvent(const SDL_Event& event);
    void update(float deltaTime);  // Add this
    void render();
    bool shouldStartGame() const;
    void reset();

private:
    SDL_Renderer* renderer_;
    bool startGame;
    float pulseTime;
};
