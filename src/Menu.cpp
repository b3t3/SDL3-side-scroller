#include "Menu.h"
#include <SDL3/SDL.h>
#include <cmath>

Menu::Menu(SDL_Renderer* renderer)
    : renderer_(renderer), startGame(false), pulseTime(0.0f)
{
}

void Menu::handleEvent(const SDL_Event& e)
{
    if (e.type == SDL_EVENT_KEY_DOWN) {
        if (e.key.scancode == SDL_SCANCODE_RETURN || 
            e.key.scancode == SDL_SCANCODE_SPACE) {
            startGame = true;
        }
    }
    
    if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        startGame = true;
    }
}

void Menu::update(float deltaTime) {
    pulseTime += deltaTime;
}

void Menu::reset() {
    startGame = false;
    pulseTime = 0.0f;
}

void Menu::render()
{
    // Dark background gradient effect
    SDL_SetRenderDrawColor(renderer_, 10, 0, 5, 255);
    SDL_RenderClear(renderer_);

    // Create vertical stripe pattern for depth
    for (int i = 0; i < 20; i++) {
        int alpha = 20 + (i % 3) * 10;
        SDL_SetRenderDrawColor(renderer_, 20, 0, 10, alpha);
        SDL_FRect stripe = { 
            static_cast<float>(i * 32), 
            0, 
            32.0f, 
            320.0f 
        };
        SDL_RenderFillRect(renderer_, &stripe);
    }

    // Pulsing effect for title
    float pulse = (std::sin(pulseTime * 2.0f) + 1.0f) * 0.5f; // 0 to 1
    int titleAlpha = 180 + static_cast<int>(pulse * 75);

    // Main title background - blood red panel
    SDL_SetRenderDrawColor(renderer_, 80, 0, 0, 255);
    SDL_FRect titleBg = { 80, 60, 480, 80 };
    SDL_RenderFillRect(renderer_, &titleBg);
    
    // Title border - brighter red
    SDL_SetRenderDrawColor(renderer_, 140, 10, 10, titleAlpha);
    SDL_FRect titleBorder1 = { 76, 56, 488, 88 };
    SDL_RenderRect(renderer_, &titleBorder1);
    SDL_FRect titleBorder2 = { 78, 58, 484, 84 };
    SDL_RenderRect(renderer_, &titleBorder2);

    // Title text "RED VEIL" - large blocky letters
    SDL_SetRenderDrawColor(renderer_, 200, 20, 20, titleAlpha);
    
    // R
    SDL_FRect r1 = { 120, 80, 12, 40 };
    SDL_FRect r2 = { 132, 80, 20, 12 };
    SDL_FRect r3 = { 132, 96, 20, 12 };
    SDL_FRect r4 = { 145, 108, 12, 12 };
    SDL_RenderFillRect(renderer_, &r1);
    SDL_RenderFillRect(renderer_, &r2);
    SDL_RenderFillRect(renderer_, &r3);
    SDL_RenderFillRect(renderer_, &r4);

    // E
    SDL_FRect e1 = { 170, 80, 12, 40 };
    SDL_FRect e2 = { 182, 80, 20, 12 };
    SDL_FRect e3 = { 182, 94, 16, 12 };
    SDL_FRect e4 = { 182, 108, 20, 12 };
    SDL_RenderFillRect(renderer_, &e1);
    SDL_RenderFillRect(renderer_, &e2);
    SDL_RenderFillRect(renderer_, &e3);
    SDL_RenderFillRect(renderer_, &e4);

    // D
    SDL_FRect d1 = { 220, 80, 12, 40 };
    SDL_FRect d2 = { 232, 80, 16, 12 };
    SDL_FRect d3 = { 244, 86, 8, 28 };
    SDL_FRect d4 = { 232, 108, 16, 12 };
    SDL_RenderFillRect(renderer_, &d1);
    SDL_RenderFillRect(renderer_, &d2);
    SDL_RenderFillRect(renderer_, &d3);
    SDL_RenderFillRect(renderer_, &d4);

    // V
    SDL_FRect v1 = { 290, 80, 10, 24 };
    SDL_FRect v2 = { 300, 104, 10, 16 };
    SDL_FRect v3 = { 310, 104, 10, 16 };
    SDL_FRect v4 = { 320, 80, 10, 24 };
    SDL_RenderFillRect(renderer_, &v1);
    SDL_RenderFillRect(renderer_, &v2);
    SDL_RenderFillRect(renderer_, &v3);
    SDL_RenderFillRect(renderer_, &v4);

    // E
    SDL_FRect e5 = { 350, 80, 12, 40 };
    SDL_FRect e6 = { 362, 80, 20, 12 };
    SDL_FRect e7 = { 362, 94, 16, 12 };
    SDL_FRect e8 = { 362, 108, 20, 12 };
    SDL_RenderFillRect(renderer_, &e5);
    SDL_RenderFillRect(renderer_, &e6);
    SDL_RenderFillRect(renderer_, &e7);
    SDL_RenderFillRect(renderer_, &e8);

    // I
    SDL_FRect i1 = { 400, 80, 12, 40 };
    SDL_RenderFillRect(renderer_, &i1);

    // L
    SDL_FRect l1 = { 430, 80, 12, 40 };
    SDL_FRect l2 = { 442, 108, 20, 12 };
    SDL_RenderFillRect(renderer_, &l1);
    SDL_RenderFillRect(renderer_, &l2);

    // Subtitle effect
    SDL_SetRenderDrawColor(renderer_, 120, 30, 30, 200);
    SDL_FRect subtitle = { 200, 150, 240, 3 };
    SDL_RenderFillRect(renderer_, &subtitle);

    // Start button with hover glow effect
    float buttonPulse = (std::sin(pulseTime * 3.0f) + 1.0f) * 0.5f;
    int buttonGlow = 60 + static_cast<int>(buttonPulse * 40);
    
    SDL_SetRenderDrawColor(renderer_, buttonGlow, 0, 0, 255);
    SDL_FRect buttonGlowRect = { 215, 195, 210, 50 };
    SDL_RenderFillRect(renderer_, &buttonGlowRect);

    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_FRect startButton = { 220, 200, 200, 40 };
    SDL_RenderFillRect(renderer_, &startButton);
    
    // Button border
    SDL_SetRenderDrawColor(renderer_, 150, 20, 20, 255);
    SDL_FRect buttonBorder = { 218, 198, 204, 44 };
    SDL_RenderRect(renderer_, &buttonBorder);

    // "START" text on button
    SDL_SetRenderDrawColor(renderer_, 200, 40, 40, 255);
    // S
    SDL_FRect s1 = { 250, 212, 16, 6 };
    SDL_FRect s2 = { 250, 206, 6, 6 };
    SDL_FRect s3 = { 250, 212, 6, 6 };
    SDL_FRect s4 = { 260, 218, 6, 6 };
    SDL_FRect s5 = { 250, 224, 16, 6 };
    SDL_RenderFillRect(renderer_, &s1);
    SDL_RenderFillRect(renderer_, &s2);
    SDL_RenderFillRect(renderer_, &s3);
    SDL_RenderFillRect(renderer_, &s4);
    SDL_RenderFillRect(renderer_, &s5);

    // T
    SDL_FRect t1 = { 275, 206, 16, 6 };
    SDL_FRect t2 = { 280, 212, 6, 18 };
    SDL_RenderFillRect(renderer_, &t1);
    SDL_RenderFillRect(renderer_, &t2);

    // A
    SDL_FRect a1 = { 300, 212, 6, 18 };
    SDL_FRect a2 = { 306, 206, 8, 6 };
    SDL_FRect a3 = { 314, 212, 6, 18 };
    SDL_FRect a4 = { 306, 218, 8, 6 };
    SDL_RenderFillRect(renderer_, &a1);
    SDL_RenderFillRect(renderer_, &a2);
    SDL_RenderFillRect(renderer_, &a3);
    SDL_RenderFillRect(renderer_, &a4);

    // R
    SDL_FRect r5 = { 330, 206, 6, 24 };
    SDL_FRect r6 = { 336, 206, 10, 6 };
    SDL_FRect r7 = { 336, 215, 10, 6 };
    SDL_FRect r8 = { 342, 221, 6, 9 };
    SDL_RenderFillRect(renderer_, &r5);
    SDL_RenderFillRect(renderer_, &r6);
    SDL_RenderFillRect(renderer_, &r7);
    SDL_RenderFillRect(renderer_, &r8);

    // T
    SDL_FRect t3 = { 358, 206, 16, 6 };
    SDL_FRect t4 = { 363, 212, 6, 18 };
    SDL_RenderFillRect(renderer_, &t3);
    SDL_RenderFillRect(renderer_, &t4);

    // Press any key prompt
    int promptAlpha = 100 + static_cast<int>(buttonPulse * 100);
    SDL_SetRenderDrawColor(renderer_, 140, 40, 40, promptAlpha);
    
    SDL_FRect prompt1 = { 230, 270, 8, 8 };
    SDL_FRect prompt2 = { 242, 270, 8, 8 };
    SDL_FRect prompt3 = { 254, 270, 8, 8 };
    SDL_FRect prompt4 = { 266, 270, 8, 8 };
    SDL_FRect prompt5 = { 278, 270, 8, 8 };
    
    SDL_FRect prompt6 = { 296, 270, 8, 8 };
    SDL_FRect prompt7 = { 308, 270, 8, 8 };
    SDL_FRect prompt8 = { 320, 270, 8, 8 };
    
    SDL_FRect prompt9 = { 338, 270, 8, 8 };
    SDL_FRect prompt10 = { 350, 270, 8, 8 };
    SDL_FRect prompt11 = { 362, 270, 8, 8 };
    SDL_FRect prompt12 = { 374, 270, 8, 8 };

    SDL_RenderFillRect(renderer_, &prompt1);
    SDL_RenderFillRect(renderer_, &prompt2);
    SDL_RenderFillRect(renderer_, &prompt3);
    SDL_RenderFillRect(renderer_, &prompt4);
    SDL_RenderFillRect(renderer_, &prompt5);
    SDL_RenderFillRect(renderer_, &prompt6);
    SDL_RenderFillRect(renderer_, &prompt7);
    SDL_RenderFillRect(renderer_, &prompt8);
    SDL_RenderFillRect(renderer_, &prompt9);
    SDL_RenderFillRect(renderer_, &prompt10);
    SDL_RenderFillRect(renderer_, &prompt11);
    SDL_RenderFillRect(renderer_, &prompt12);

    // Corner decorative elements - blood drop style
    SDL_SetRenderDrawColor(renderer_, 100, 0, 0, 180);
    
    // Top corners - fangs
    SDL_FRect fang1 = { 30, 20, 8, 20 };
    SDL_FRect fang2 = { 34, 40, 4, 10 };
    SDL_RenderFillRect(renderer_, &fang1);
    SDL_RenderFillRect(renderer_, &fang2);
    
    SDL_FRect fang3 = { 602, 20, 8, 20 };
    SDL_FRect fang4 = { 602, 40, 4, 10 };
    SDL_RenderFillRect(renderer_, &fang3);
    SDL_RenderFillRect(renderer_, &fang4);

    // Bottom corners - blood drops
    SDL_FRect drop1 = { 30, 290, 12, 8 };
    SDL_FRect drop2 = { 34, 298, 4, 8 };
    SDL_RenderFillRect(renderer_, &drop1);
    SDL_RenderFillRect(renderer_, &drop2);
    
    SDL_FRect drop3 = { 598, 290, 12, 8 };
    SDL_FRect drop4 = { 602, 298, 4, 8 };
    SDL_RenderFillRect(renderer_, &drop3);
    SDL_RenderFillRect(renderer_, &drop4);

    SDL_RenderPresent(renderer_);
}

bool Menu::shouldStartGame() const {
    return startGame;
}
