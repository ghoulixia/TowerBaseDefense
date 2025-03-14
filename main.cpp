#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include <string>

const int SCREEN_WIDTH = 1280; //resolution game
const int SCREEN_HEIGHT = 720;

SDL_Window* window = nullptr;     
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr;
TTF_Font* logoFont = nullptr;

enum MenuOption { START, OPTIONS, EXIT, TOTAL };
MenuOption selectedOption = START;

int startGameWidth, startGameHeight; //khai báo để tính size
int startGameX;
int optionsWidth, optionsHeight;
int optionsX;
int exitWidth, exitHeight;
int exitX;

bool init() {  //khởi tạo, check lỗi
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() == -1) {
        std::cout << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Tower Defense - Menu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    font = TTF_OpenFont("NintendBoldRM8E.ttf", 48);
    if (!font) {
        std::cout << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    logoFont = TTF_OpenFont("MightySouly.ttf", 88);
    if (!logoFont) {
        std::cout << "Failed to load logo font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    //tính size nút,logo -> ở chính giữa 
    TTF_SizeText(font, "Start Game", &startGameWidth, &startGameHeight);
    startGameX = 640 - (startGameWidth / 2);

    TTF_SizeText(font, "Options", &optionsWidth, &optionsHeight);
    optionsX = 640 - (optionsWidth / 2);

    TTF_SizeText(font, "Exit", &exitWidth, &exitHeight);
    exitX = 640 - (exitWidth / 2);

    return true;
}

//settings chữ
void renderText(const std::string& text, int x, int y, bool isSelected, TTF_Font* textFont) {
    SDL_Color color = isSelected ? SDL_Color{ 255, 255, 0, 255 } : SDL_Color{ 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(textFont, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect destRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void renderMenu() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    //vẽ logo
    int textWidth, textHeight;
    TTF_SizeText(logoFont, "Tower Base Defense", &textWidth, &textHeight);
    int logoX = 640 - (textWidth / 2);
    renderText("Tower Base Defense", logoX, 100, false, logoFont);

    //vẽ nút
    renderText("Start Game", startGameX, 320, selectedOption == START, font);
    renderText("Options", optionsX, 400, selectedOption == OPTIONS, font);
    renderText("Exit", exitX, 480, selectedOption == EXIT, font);

    SDL_RenderPresent(renderer);
}

//hàm cửa sổ options
void showOptions() {
    bool optionsRunning = true;
    while (optionsRunning) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                optionsRunning = false;
            }
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    optionsRunning = false;
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderClear(renderer);
        int textWidth, textHeight;
        TTF_SizeText(font, "OPTIONS", &textWidth, &textHeight);
        int optionsMenuX = 640 - (textWidth / 2);
        renderText("OPTIONS", optionsMenuX, 100, false, font);
        renderText("Sound: On  Off", optionsMenuX - 60, 200, false, font);
        //thêm options?
        SDL_RenderPresent(renderer);
    }
}

//loop menu
void menuLoop() {
    bool running = true;
    while (running) {
        bool performAction = false;
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_UP:
                    selectedOption = static_cast<MenuOption>((selectedOption - 1 + TOTAL) % TOTAL);
                    break;
                case SDLK_DOWN:
                    selectedOption = static_cast<MenuOption>((selectedOption + 1) % TOTAL);
                    break;
                case SDLK_RETURN:
                    performAction = true;
                    break;
                }
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                int clickX = event.button.x;
                int clickY = event.button.y;

                //check vị trí chuột
                if (clickX >= startGameX && clickX < startGameX + startGameWidth &&
                    clickY >= 320 && clickY < 320 + startGameHeight) {
                    selectedOption = START;
                    performAction = true;
                }
                else if (clickX >= optionsX && clickX < optionsX + optionsWidth &&
                    clickY >= 400 && clickY < 400 + optionsHeight) {
                    selectedOption = OPTIONS;
                    performAction = true;
                }
                else if (clickX >= exitX && clickX < exitX + exitWidth &&
                    clickY >= 480 && clickY < 480 + exitHeight) {
                    selectedOption = EXIT;
                    performAction = true;
                }
            }
        }

        if (performAction) {
            if (selectedOption == EXIT) {
                running = false;
            }
            else if (selectedOption == START) {
                //func vào cửa sổ chọn màn
            }
            else if (selectedOption == OPTIONS) {
                showOptions();
            }
        }

        renderMenu();
    }
}

//xóa
void close() {
    TTF_CloseFont(font);
    TTF_CloseFont(logoFont);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if (!init()) {
        return -1;
    }

    menuLoop();
    close();

    return 0;
}