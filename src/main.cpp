#include <SDL3/SDL.h>
#include "Menu.h"

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow(
        "Red Veil",
        800, 600,
        SDL_WINDOW_RESIZABLE
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL, 0);

    bool running = true;
    bool inMenu = true;

    Menu menu(renderer);
    
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }

            if (inMenu) {
                menu.handleEvent(event);
            }
        }

        if (inMenu) {
            menu.render();
            if (menu.shouldStartGame()) {
                inMenu = false;
            }
        } else {
           
            SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
