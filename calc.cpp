#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <sstream>
#include <vector>

// Screen dimensions
const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 550;

// Button dimensions
const int BUTTON_WIDTH = 80;
const int BUTTON_HEIGHT = 80;
const int BUTTON_MARGIN = 10;

// Colors
const SDL_Color WHITE = {255, 255, 255, 255};
const SDL_Color BLACK = {0, 0, 0, 255};

// Structure to store button properties
struct Button {
    SDL_Rect rect;
    std::string label;
};

// Global variables
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr;
std::vector<Button> buttons;
std::string input = "";
std::string result = "";

// Function to render text
void renderText(const std::string& text, int x, int y) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), BLACK);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dest = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &dest);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Function to evaluate the arithmetic expression
std::string evaluateExpression(const std::string& expr) {
    double num1, num2;
    char op;
    std::stringstream ss(expr);
    ss >> num1 >> op >> num2;
    
    if (ss.fail() || !ss.eof()) return "Err";

    double result;
    switch (op) {
        case '+': result = num1 + num2; break;
        case '-': result = num1 - num2; break;
        case '*': result = num1 * num2; break;
        case '/': result = (num2 != 0) ? num1 / num2 : 0; break;
        default: return "Err";
    }

    return std::to_string(result);
}

// Function to initialize SDL2
bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL Initialization Failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf Initialization Failed: " << TTF_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("SDL2 Calculator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window Creation Failed: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer Creation Failed: " << SDL_GetError() << std::endl;
        return false;
    }

    font = TTF_OpenFont("/System/Library/Fonts/Courier.ttc", 24);
    if (!font) {
        std::cerr << "Font Loading Failed: " << TTF_GetError() << std::endl;
        return false;
    }

    return true;
}

// Function to create buttons
void createButtons() {
    std::vector<std::string> labels = {
        "7", "8", "9", "/",
        "4", "5", "6", "*",
        "1", "2", "3", "-",
        "C", "0", "=", "+"
    };

    int startX = 20, startY = 150;
    for (size_t i = 0; i < labels.size(); i++) {
        Button btn;
        btn.rect = {
            static_cast<int>(startX + (i % 4) * (BUTTON_WIDTH + BUTTON_MARGIN)),
            static_cast<int>(startY + (i / 4) * (BUTTON_HEIGHT + BUTTON_MARGIN)),
            BUTTON_WIDTH,
            BUTTON_HEIGHT
        };
        btn.label = labels[i];
        buttons.push_back(btn);
    }
}

// Function to handle button clicks
void handleButtonClick(int x, int y) {
    for (auto& btn : buttons) {
        if (x >= btn.rect.x && x <= btn.rect.x + BUTTON_WIDTH &&
            y >= btn.rect.y && y <= btn.rect.y + BUTTON_HEIGHT) {
            std::string val = btn.label;
            if (val == "=") {
                result = evaluateExpression(input);
            } else if (val == "C") {
                input.clear();
                result.clear();
            } else {
                input += val;
            }
        }
    }
}

// Function to render UI
void renderUI() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // Render input text
    renderText("Input: " + input, 20, 50);

    // Render result text
    renderText("Result: " + result, 20, 100);

    // Render buttons
    for (auto& btn : buttons) {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderFillRect(renderer, &btn.rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &btn.rect);
        renderText(btn.label, btn.rect.x + 30, btn.rect.y + 30);
    }

    SDL_RenderPresent(renderer);
}

// Function to handle events
void handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            TTF_CloseFont(font);
            TTF_Quit();
            SDL_Quit();
            exit(0);
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            handleButtonClick(event.button.x, event.button.y);
        }
    }
}

// Main function
int main() {
    if (!initSDL()) return 1;

    createButtons();

    while (true) {
        handleEvents();
        renderUI();
        SDL_Delay(16);  // Prevent CPU overuse
    }

    return 0;
}
