#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

#include "GameMap.h"
#include "Enemy.h"
#include "Tower.h"
#include "Projectile.h"
#include "SoundManager.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int MAX_STAGES = 10;
const int TILE_W = 72;
const int TILE_H = 72;
const int TOOLBAR_PADDING = 15;
const int ICON_SIZE = 64;
const int ICON_SPACING = 10;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr;
TTF_Font* logoFont = nullptr;
TTF_Font* uiFont = nullptr;

enum class GameState { MENU, CHOOSE_STAGE, PLAYING, GAME_OVER, OPTIONS, GAME_WON };
enum class MenuOption { START, OPTIONS, EXIT, TOTAL };

GameState currentState = GameState::MENU;
MenuOption selectedOption = MenuOption::START;
GameMap* map = nullptr;

int startGameWidth, startGameHeight, startGameX;
int optionsWidth, optionsHeight, optionsX;
int exitWidth, exitHeight, exitX;

SDL_Texture* stagePreviewTextures[MAX_STAGES] = { nullptr };
SDL_Texture* monsterTexture1 = nullptr;
SDL_Texture* monsterTexture2 = nullptr;
SDL_Texture* monsterTexture3 = nullptr;
SDL_Texture* monsterTextureBoss = nullptr;

SDL_Texture* texTurretBasic = nullptr;
SDL_Texture* texTurretPierce = nullptr;
SDL_Texture* texTurretMinigun = nullptr;
SDL_Texture* texTurretSniper = nullptr;

SDL_Texture* texProjectileBasic = nullptr;
SDL_Texture* texProjectilePierce = nullptr;
SDL_Texture* texProjectileMinigun = nullptr;
SDL_Texture* texProjectileSniper = nullptr;

SDL_Texture* texIconBasic = nullptr;
SDL_Texture* texIconPierce = nullptr;
SDL_Texture* texIconMinigun = nullptr;
SDL_Texture* texIconSniper = nullptr;

std::vector<Enemy*> activeEnemies;
std::vector<Tower*> placedTowers;
std::vector<Projectile*> activeProjectiles;

int playerHealth = 20;
int playerMoney = 250;
int currentWave = 0;

enum class EnemyType {
    MONSTER_NORMAL,
    MONSTER_TOUGH,
    MONSTER_FAST
};

//sinh quái từng đợt
static float spawnTimer = 0.0f;
const float spawnInterval = 0.8f;  //tg spawn của quái cách nhau xxxx
static int waveEnemyCount = 0;    //Quái đã sinh trong một đợt
const int enemiesPerWaveBase = 10;
const int enemiesPerWaveScaling = 3; //thêm quái mỗi đợt
const int MAX_WAVES = 10; //tổng đợt
static bool waveActive = false;
const float waveDelay = 3.0f;
static float waveDelayTimer = 0.0f;
const int WAVE_COMPLETION_REWARD_BASE = 30;
const int WAVE_COMPLETION_REWARD_SCALING = 7;

TowerType selectedBuildType = TowerType::UNKNOWN; //chưa chọn gì
bool showBuildPreview = false;

//khai báo nguyên hàm mẫu (function prototypes)
bool init();
void close();
void gameLoop();
void renderText(const std::string& text, int x, int y, SDL_Color color, TTF_Font* textFont, bool center = false);
void renderMenu();
void showOptions();
void drawStageButton(int x, int y, int stage);
int chooseStage();
SDL_Texture* loadTexture(const std::string& path);
void renderGameOver();
void resetGameState();
void renderToolbar(int toolbarX, int toolbarW);
void renderPlacementPreview(int mouseX, int mouseY);

int getBossCountForWave(int wave) {
    if (wave == 3) return 1;
    if (wave == 6) return 2;
    if (wave == 9) return 4;
    return 0;
}

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL init fail: " << SDL_GetError() << std::endl; return false;
    }
    if (TTF_Init() == -1) { 
        std::cerr << "TTF init fail: " << TTF_GetError() << std::endl; SDL_Quit(); return false;
    }
    window = SDL_CreateWindow("Tower Base Defense", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window fail: " << SDL_GetError() << std::endl; 
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "renderer fail: " << SDL_GetError() << std::endl; SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return false;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); //alpha blending để load ảnh background trong suốt

    font = TTF_OpenFont("Data/NintendBoldRM8E.ttf", 48);
    logoFont = TTF_OpenFont("Data/MightySouly.ttf", 88);
    uiFont = TTF_OpenFont("Data/Verdana-bold.ttf", 20);
    if (!font || !logoFont || !uiFont) {
        std::cerr << "Font load fail: " << TTF_GetError() << std::endl;
        close();
        return false;
    }

    //load texture preview màn 
    for (int i = 0; i < MAX_STAGES; ++i) {
        stagePreviewTextures[i] = loadTexture("Data/stage" + std::to_string(i + 1) + ".bmp");
    }

    monsterTexture1 = loadTexture("Data/monster1.bmp");    //load texture quái
    monsterTexture2 = loadTexture("Data/monster2.bmp");
    monsterTexture3 = loadTexture("Data/monster3.bmp");
    monsterTextureBoss = loadTexture("Data/monster_boss1.bmp");
    if (!monsterTexture1 || !monsterTexture2 || !monsterTexture3 || !monsterTextureBoss) {
        std::cerr << "Enemy load fail" << std::endl;
        close();
        return false;
    }

    texTurretBasic = loadTexture("Data/turret_basic.bmp");    //load texture trụ
    texTurretPierce = loadTexture("Data/turret_pierce.bmp");
    texTurretMinigun = loadTexture("Data/turret_minigun.bmp");
    texTurretSniper = loadTexture("Data/turret_sniper.bmp");
    if (!texTurretBasic || !texTurretPierce || !texTurretMinigun || !texTurretSniper) {
        std::cerr << "Turret load fail" << std::endl;
        close();
        return false;
    }

    texProjectileBasic = loadTexture("Data/projectile_basic.bmp");    //load texture đạn
    texProjectilePierce = loadTexture("Data/projectile_pierce.bmp");
    texProjectileMinigun = loadTexture("Data/projectile_minigun.bmp");
    texProjectileSniper = loadTexture("Data/projectile_sniper.bmp");
    if (!texProjectileBasic || !texProjectilePierce || !texProjectileMinigun || !texProjectileSniper) {
        std::cerr << "Projectile load fail" << std::endl;
        close();
        return false;
    }

    texIconBasic = loadTexture("Data/icon_basic.bmp");    //load icon
    texIconPierce = loadTexture("Data/icon_pierce.bmp");
    texIconMinigun = loadTexture("Data/icon_minigun.bmp");
    texIconSniper = loadTexture("Data/icon_sniper.bmp");
    if (!texIconBasic || !texIconPierce || !texIconMinigun || !texIconSniper) {
        std::cerr << "Icon load fail" << std::endl;
        close();
        return false;
    }

    if (!SoundManager::getInstance().init()) {
        std::cerr << "SoundManager init fail" << std::endl;
        close();
        return false;
    }

    TTF_SizeText(font, "Start Game", &startGameWidth, &startGameHeight); startGameX = SCREEN_WIDTH / 2 - startGameWidth / 2;
    TTF_SizeText(font, "Options", &optionsWidth, &optionsHeight); optionsX = SCREEN_WIDTH / 2 - optionsWidth / 2;
    TTF_SizeText(font, "Exit", &exitWidth, &exitHeight); exitX = SCREEN_WIDTH / 2 - exitWidth / 2;

    //tạo initial map object cho gamemap
    try {
        map = new GameMap(10, 15, TILE_W, TILE_H);
        map->loadTextures(renderer);
    }
    catch (const std::exception& e) {
        std::cerr << "gamemap fail" << std::endl;
        close();
        return false;
    }

    std::cout << "init successful" << std::endl;
    return true;
}

//giúp load texture
SDL_Texture* loadTexture(const std::string& path) {
    SDL_Surface* surface = SDL_LoadBMP(path.c_str());
    
    //color key để ảnh trong suốt
    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 0, 0));
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

//load ảnh
void renderText(const std::string& text, int x, int y, SDL_Color color, TTF_Font* textFont, bool center) {
    SDL_Surface* surface = TTF_RenderText_Solid(textFont, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect destRect;
    destRect.w = surface->w; destRect.h = surface->h; destRect.y = y;
    destRect.x = center ? (x - surface->w / 2) : x;
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);
    SDL_FreeSurface(surface); SDL_DestroyTexture(texture);
}

void renderMenu() {
    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
    SDL_RenderClear(renderer);
    renderText("Tower Base Defense", SCREEN_WIDTH / 2, 100, { 220, 220, 50, 255 }, logoFont, true);
    SDL_Color white = { 255, 255, 255, 255 };
    renderText("Start Game", startGameX, 320, white, font);
    renderText("Options", optionsX, 400, white, font);
    renderText("Exit", exitX, 480, white, font);
    SDL_RenderPresent(renderer);
}

void showOptions() {

    bool optionsRunning = true;
    SDL_Color white = { 255, 255, 255, 255 };

    while (optionsRunning && currentState == GameState::OPTIONS) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                optionsRunning = false;
                currentState = GameState::MENU; 
            }
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    optionsRunning = false;
                    currentState = GameState::MENU;
                }
                else if (event.key.keysym.sym == SDLK_LEFT) {
                    int volume = SoundManager::getInstance().getVolume();
                    volume = std::max(0, volume - 8);
                    SoundManager::getInstance().setVolume(volume);
                }
                else if (event.key.keysym.sym == SDLK_RIGHT) {
                    int volume = SoundManager::getInstance().getVolume();
                    volume = std::min(128, volume + 8);
                    SoundManager::getInstance().setVolume(volume);
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderClear(renderer);

        renderText("OPTIONS", SCREEN_WIDTH / 2, 100, white, font, true);

        // Âm lượng hiệu ứng
        int volume = SoundManager::getInstance().getVolume();
        renderText("SFX Volume:", SCREEN_WIDTH / 2 - 120, 200, white, uiFont);
        renderText(std::to_string(volume), SCREEN_WIDTH / 2 + 80, 200, white, uiFont);
        renderText("Left/Right Arrow: Adjust", SCREEN_WIDTH / 2, 240, white, uiFont, true);

        renderText("Press ESC to return", SCREEN_WIDTH / 2, 400, white, uiFont, true);

        SDL_RenderPresent(renderer);
    }
}

void drawStageButton(int x, int y, int stage) {
    const int buttonSize = 100;
    SDL_Rect rect = { x, y, buttonSize, buttonSize };

    //ảnh preview màn 
    int stageIndex = stage - 1;
    if (stageIndex >= 0 && stageIndex < MAX_STAGES && stagePreviewTextures[stageIndex]) {
        SDL_RenderCopy(renderer, stagePreviewTextures[stageIndex], NULL, &rect);
    }
    else {
        renderText(std::to_string(stage), x + buttonSize / 2, y + buttonSize / 2 - 15,
            { 255, 255, 255, 255 }, uiFont, true);
    }
}

int chooseStage() {
    bool choosing = true;
    int selectedStage = -1; //-1 là mặc định chưa chọn/hủy 

    const int buttonSize = 100;
    const int buttonsPerRow = 5;
    const int spacingX = 40;
    const int spacingY = 40;
    const int totalWidth = buttonsPerRow * buttonSize + (buttonsPerRow - 1) * spacingX;
    const int startX = (SCREEN_WIDTH - totalWidth) / 2;
    const int startY = 150; //điểm bắt đầu

    SDL_Color titleColor = { 220, 220, 220, 255 };

    while (choosing && currentState == GameState::CHOOSE_STAGE) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                choosing = false;
                selectedStage = -1;
                currentState = GameState::MENU;
            }
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    choosing = false;
                    selectedStage = -1;
                    currentState = GameState::MENU;
                }
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    int mouseX = event.button.x;
                    int mouseY = event.button.y;

                    for (int i = 0; i < MAX_STAGES; ++i) {
                        int row = i / buttonsPerRow;
                        int col = i % buttonsPerRow;
                        int buttonX = startX + col * (buttonSize + spacingX);
                        int buttonY = startY + row * (buttonSize + spacingY);
                        if (mouseX >= buttonX && mouseX < buttonX + buttonSize && mouseY >= buttonY && mouseY < buttonY + buttonSize) {
                            selectedStage = i + 1;
                            choosing = false;
                            break;
                        }
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 50, 255);
        SDL_RenderClear(renderer);

        renderText("Choose Your Stage", SCREEN_WIDTH / 2, 50, titleColor, font, true);

        for (int i = 0; i < MAX_STAGES; ++i) {
            int row = i / buttonsPerRow;
            int col = i % buttonsPerRow;
            int x = startX + col * (buttonSize + spacingX);
            int y = startY + row * (buttonSize + spacingY);
            drawStageButton(x, y, i + 1);
        }

        renderText("ESC to return", SCREEN_WIDTH / 2, SCREEN_HEIGHT - 60, titleColor, uiFont, true);

        SDL_RenderPresent(renderer);
    }
    return selectedStage;
}

void renderGameOver() {
    SoundManager::getInstance().playLosing();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect overlayRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_RenderFillRect(renderer, &overlayRect);

    renderText("GAME OVER!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 50, { 255, 0, 0, 255 }, logoFont, true);
    renderText("Press Enter to return to Menu", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 50, { 255, 255, 255, 255 }, uiFont, true);

    SDL_RenderPresent(renderer);
}

void renderGameWon() {
    SoundManager::getInstance().playWinning();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect overlayRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_RenderFillRect(renderer, &overlayRect);

    renderText("YOU WIN!", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 50, { 0, 255, 0, 255 }, logoFont, true);
    renderText("Press Enter to return to Menu", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 50, { 255, 255, 255, 255 }, uiFont, true);

    SDL_RenderPresent(renderer);
}


void resetGameState() {   //reset các biến của game sau khi kết thúc màn
    playerHealth = 20;
    playerMoney = 250;
    currentWave = 0;
    waveActive = false;
    waveDelayTimer = waveDelay;
    waveEnemyCount = 0;
    spawnTimer = 0.0f;

    for (Enemy* enemy : activeEnemies) {
        delete enemy;
    }
    activeEnemies.clear();
    for (Tower* tower : placedTowers) {
        delete tower;
    }
    placedTowers.clear();
    for (Projectile* proj : activeProjectiles) {
        delete proj;
    }
    activeProjectiles.clear();
    selectedBuildType = TowerType::UNKNOWN;
    showBuildPreview = false;

    if (map) {
        for (int r = 0; r < map->rows; ++r) {
            for (int c = 0; c < map->cols; ++c) {
                if (map->grid[r][c] == 'T') {
                    map->grid[r][c] = '*';
                }
            }
        }
    }
}

void renderToolbar(int toolbarX, int toolbarW) {
    SDL_SetRenderDrawColor(renderer, 30, 30, 35, 255);
    SDL_Rect toolbarRect = { toolbarX, 0, toolbarW, SCREEN_HEIGHT };
    SDL_RenderFillRect(renderer, &toolbarRect);

    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Color yellow = { 255, 255, 0, 255 };
    SDL_Color grey = { 120, 120, 120, 255 };
    SDL_Color iconGrey = { 100, 100, 100, 255 };
    SDL_Color highlightColor = { 0, 150, 255, 255 };

    int currentY = TOOLBAR_PADDING;
    int textX = toolbarX + TOOLBAR_PADDING;

    renderText("Health: " + std::to_string(playerHealth), textX, currentY, white, uiFont); currentY += 25;
    renderText("Money: " + std::to_string(playerMoney), textX, currentY, white, uiFont); currentY += 30;
    renderText("Wave: " + std::to_string(currentWave), textX, currentY, white, uiFont); currentY += 35;

    if (!waveActive && currentState == GameState::PLAYING && currentWave > 0) {
        renderText("Next wave in 3s", textX, currentY, white, uiFont); currentY += 25;
        currentY += 60;
    }
    else {
        currentY += 85;
    }

    int iconStartX = toolbarX + (toolbarW - ICON_SIZE) / 2;

    struct TowerUIData {
        SDL_Texture* iconTexture;
        int cost;
        TowerType type;
        const char* keyLabel;
    };

    TowerUIData towerInfo[] = {
        {texIconBasic, BASIC_COST, TowerType::BASIC, "1"},
        {texIconPierce, PIERCE_COST, TowerType::PIERCING, "2"},
        {texIconMinigun, MINIGUN_COST, TowerType::MINIGUN, "3"},
        {texIconSniper, SNIPER_COST, TowerType::SNIPER, "4"}
    };

    for (const auto& info : towerInfo) {
        bool affordable = playerMoney >= info.cost;
        bool selected = (selectedBuildType == info.type);

        if (info.iconTexture) {
            SDL_Rect iconRect = { iconStartX, currentY, ICON_SIZE, ICON_SIZE };

            if (affordable) {
                SDL_SetTextureColorMod(info.iconTexture, 255, 255, 255);
            }
            else {
                SDL_SetTextureColorMod(info.iconTexture, iconGrey.r, iconGrey.g, iconGrey.b);
            }
            SDL_RenderCopy(renderer, info.iconTexture, nullptr, &iconRect);
            SDL_SetTextureColorMod(info.iconTexture, 255, 255, 255);

            if (selected && showBuildPreview) {
                SDL_SetRenderDrawColor(renderer, highlightColor.r, highlightColor.g, highlightColor.b, 255);
                SDL_Rect outerBorderRect = { iconRect.x - 1, iconRect.y - 1, iconRect.w + 2, iconRect.h + 2 };
                SDL_RenderDrawRect(renderer, &outerBorderRect);
            }

            renderText(info.keyLabel, iconStartX - TOOLBAR_PADDING, currentY + ICON_SIZE / 2 - 10,
                (selected && showBuildPreview) ? yellow : white, uiFont);

            renderText("$" + std::to_string(info.cost), iconStartX + ICON_SIZE / 2, currentY + ICON_SIZE + 5,
                affordable ? white : grey, uiFont, true);

            currentY += ICON_SIZE + ICON_SPACING + 25;
        }
    }

    currentY = SCREEN_HEIGHT - 90;
    renderText("1/2/3/4: Select", textX, currentY, white, uiFont);
    currentY += 20;
    renderText("E: Delete", textX, currentY, white, uiFont);
    currentY += 20;
    renderText("U: Upgrade", textX, currentY, white, uiFont);
    currentY += 20;
    renderText("ESC: Quit", textX, currentY, white, uiFont);
}

void renderPlacementPreview(int mouseX, int mouseY) {
    if (!map || !showBuildPreview || selectedBuildType == TowerType::UNKNOWN) return;

    int tileX = mouseX / TILE_W;
    int tileY = mouseY / TILE_H;

    if (tileX < 0 || tileX >= map->cols || tileY < 0 || tileY >= map->rows) return;

    int cost = 0;
    SDL_Texture* previewTexture = nullptr;
    float previewRange = 0;

    switch (selectedBuildType) {
    case TowerType::BASIC:    cost = BASIC_COST; previewTexture = texTurretBasic; previewRange = BASIC_RANGE; break;
    case TowerType::PIERCING: cost = PIERCE_COST; previewTexture = texTurretPierce; previewRange = PIERCE_RANGE; break;
    case TowerType::MINIGUN:  cost = MINIGUN_COST; previewTexture = texTurretMinigun; previewRange = MINIGUN_RANGE; break;
    case TowerType::SNIPER:   cost = SNIPER_COST; previewTexture = texTurretSniper; previewRange = SNIPER_RANGE; break;
    default: return;
    }

    bool canAfford = playerMoney >= cost;
    bool isValidTile = (map->grid[tileY][tileX] == '*');
    bool isOccupied = false;
    for (const auto* tower : placedTowers) {
        if (tower->getTilePos().x == tileX && tower->getTilePos().y == tileY) { isOccupied = true; break; }
    }

    bool canPlace = canAfford && isValidTile && !isOccupied;

    float previewPixelX = tileX * TILE_W + TILE_W / 2.0f;
    float previewPixelY = tileY * TILE_H + TILE_H / 2.0f;

    //Vẽ tầm bắn
    if (previewRange > 0) {
        SDL_Color rangeColor = canPlace ? SDL_Color{ 0, 255, 0, 50 } : SDL_Color{ 255, 0, 0, 50 };
        SDL_SetRenderDrawColor(renderer, rangeColor.r, rangeColor.g, rangeColor.b, rangeColor.a);
        int radius = static_cast<int>(previewRange);
        int centerX = static_cast<int>(previewPixelX);
        int centerY = static_cast<int>(previewPixelY);
        //**
        for (int dy = -radius; dy <= radius; dy++) {
            int y = centerY + dy;
            //dùng phuongw trình đường tròn
            int dx = static_cast<int>(std::sqrt(radius * radius - dy * dy));
            int x1 = centerX - dx;
            int x2 = centerX + dx;
            SDL_RenderDrawLine(renderer, x1, y, x2, y);
        }
    }

    //vẽ preview tower
    if (previewTexture) {
        int texW, texH;
        SDL_QueryTexture(previewTexture, nullptr, nullptr, &texW, &texH);
        SDL_Rect dest = { static_cast<int>(previewPixelX - texW / 2.0f), static_cast<int>(previewPixelY - texH / 2.0f), texW, texH };
        Uint8 alpha = canPlace ? 180 : 100;
        SDL_SetTextureAlphaMod(previewTexture, alpha);
        SDL_RenderCopy(renderer, previewTexture, nullptr, &dest);
        SDL_SetTextureAlphaMod(previewTexture, 255);
    }
}

void gameLoop() {
    bool running = true;
    Uint64 lastFrameTime = SDL_GetTicks64(); //**
    int mouseX = 0, mouseY = 0;
    int mapPixelWidth = 0, toolbarStartX = 0, toolbarWidth = 0;

    while (running) {
        Uint64 currentFrameTime = SDL_GetTicks64();
        float deltaTime = (currentFrameTime - lastFrameTime) / 1000.0f;
        lastFrameTime = currentFrameTime;
        const float MAX_DELTA_TIME = 0.1f;
        if (deltaTime > MAX_DELTA_TIME) deltaTime = MAX_DELTA_TIME;

        SDL_GetMouseState(&mouseX, &mouseY);

        if (map) {
            mapPixelWidth = map->cols * TILE_W;
            toolbarStartX = mapPixelWidth;
            toolbarWidth = SCREEN_WIDTH - mapPixelWidth;
        }
        else {
            mapPixelWidth = SCREEN_WIDTH; toolbarStartX = SCREEN_WIDTH; toolbarWidth = 0;
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            switch (currentState) {
            case GameState::MENU:  //**
                if (event.type == SDL_KEYDOWN) {
                    switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        selectedOption = static_cast<MenuOption>(((int)selectedOption - 1 + (int)MenuOption::TOTAL) % (int)MenuOption::TOTAL);
                        break;
                    case SDLK_DOWN:
                        selectedOption = static_cast<MenuOption>(((int)selectedOption + 1) % (int)MenuOption::TOTAL);
                        break;
                    case SDLK_RETURN:
                    case SDLK_SPACE:
                        if (selectedOption == MenuOption::START) {
                            currentState = GameState::CHOOSE_STAGE;
                        }
                        else if (selectedOption == MenuOption::OPTIONS) {
                            currentState = GameState::OPTIONS;
                            showOptions();
                        }
                        else if (selectedOption == MenuOption::EXIT) {
                            running = false;
                        }
                        break;
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                    }
                }
                else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                    int clickX = event.button.x;
                    int clickY = event.button.y;
                    if (clickY >= 320 && clickY < 320 + startGameHeight && clickX >= startGameX && clickX < startGameX + startGameWidth) {
                        currentState = GameState::CHOOSE_STAGE;
                    }
                    else if (clickY >= 400 && clickY < 400 + optionsHeight && clickX >= optionsX && clickX < optionsX + optionsWidth) {
                        currentState = GameState::OPTIONS;
                        showOptions();
                    }
                    else if (clickY >= 480 && clickY < 480 + exitHeight && clickX >= exitX && clickX < exitX + exitWidth) {
                        running = false;
                    }
                }
                break;

            case GameState::CHOOSE_STAGE:
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                    currentState = GameState::MENU;
                }
                break;

            case GameState::PLAYING:
                if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        if (showBuildPreview) showBuildPreview = false;
                        else { currentState = GameState::MENU; resetGameState(); }
                    }
                    else if (event.key.keysym.sym == SDLK_1) { SoundManager::getInstance().playSelect(); selectedBuildType = TowerType::BASIC; showBuildPreview = true; }
                    else if (event.key.keysym.sym == SDLK_2) { SoundManager::getInstance().playSelect(); selectedBuildType = TowerType::PIERCING; showBuildPreview = true; }
                    else if (event.key.keysym.sym == SDLK_3) { SoundManager::getInstance().playSelect(); selectedBuildType = TowerType::MINIGUN; showBuildPreview = true; }
                    else if (event.key.keysym.sym == SDLK_4) { SoundManager::getInstance().playSelect(); selectedBuildType = TowerType::SNIPER; showBuildPreview = true; }
                    else if (event.key.keysym.sym == SDLK_m) { playerMoney += 500; }
                    else if (event.key.keysym.sym == SDLK_e) {
                        // Xóa trụ tại vị trí chuột (nếu có)
                        int tileX = mouseX / TILE_W;
                        int tileY = mouseY / TILE_H;
                        if (map && tileX >= 0 && tileX < map->cols && tileY >= 0 && tileY < map->rows) {
                            // Tìm trụ tại vị trí này
                            auto it = std::find_if(placedTowers.begin(), placedTowers.end(), [&](Tower* t) {
                                auto pos = t->getTilePos();
                                return pos.x == tileX && pos.y == tileY;
                            });
                            if (it != placedTowers.end()) {
                                delete *it;
                                placedTowers.erase(it);
                                map->grid[tileY][tileX] = '*'; // Đánh dấu lại ô này có thể đặt trụ
                            }
                        }
                    }
                    else if (event.key.keysym.sym == SDLK_u) {
                        // Nâng cấp trụ tại vị trí chuột (nếu có)
                        int tileX = mouseX / TILE_W;
                        int tileY = mouseY / TILE_H;
                        if (map && tileX >= 0 && tileX < map->cols && tileY >= 0 && tileY < map->rows) {
                            auto it = std::find_if(placedTowers.begin(), placedTowers.end(), [&](Tower* t) {
                                auto pos = t->getTilePos();
                                return pos.x == tileX && pos.y == tileY;
                            });
                            if (it != placedTowers.end()) {
                                Tower* tower = *it;
                                int upgradeCost = 50 * tower->getLevel(); //giá nâng cấp tăng dần
                                if (tower->getLevel() < 3 && playerMoney >= upgradeCost) {
                                    tower->upgrade();
                                    playerMoney -= upgradeCost;
                                }
                            }
                        }
                    }
                }
                else if (event.type == SDL_MOUSEBUTTONDOWN) {
                    int clickX = event.button.x; int clickY = event.button.y;

                    if (event.button.button == SDL_BUTTON_LEFT) {
                        bool clickedUI = false;
                        if (clickX >= toolbarStartX) {
                            clickedUI = true;
                            int yPos = TOOLBAR_PADDING + 175;
                            SDL_Rect iconRects[4];
                            int iconX = toolbarStartX + (toolbarWidth - ICON_SIZE) / 2;
                            for (int i = 0; i < 4; ++i) {
                                iconRects[i] = { iconX, yPos, ICON_SIZE, ICON_SIZE };
                                yPos += ICON_SIZE + ICON_SPACING + 25;
                            }
                            if (clickY >= iconRects[0].y && clickY < iconRects[0].y + ICON_SIZE) { SoundManager::getInstance().playSelect(); selectedBuildType = TowerType::BASIC; showBuildPreview = true; }
                            else if (clickY >= iconRects[1].y && clickY < iconRects[1].y + ICON_SIZE) { SoundManager::getInstance().playSelect(); selectedBuildType = TowerType::PIERCING; showBuildPreview = true; }
                            else if (clickY >= iconRects[2].y && clickY < iconRects[2].y + ICON_SIZE) { SoundManager::getInstance().playSelect(); selectedBuildType = TowerType::MINIGUN; showBuildPreview = true; }
                            else if (clickY >= iconRects[3].y && clickY < iconRects[3].y + ICON_SIZE) { SoundManager::getInstance().playSelect(); selectedBuildType = TowerType::SNIPER; showBuildPreview = true; }
                            else { clickedUI = false; }
                        }

                        if (!clickedUI && showBuildPreview && map) {
                            int tileX = clickX / TILE_W; int tileY = clickY / TILE_H;
                            if (tileX >= 0 && tileX < map->cols && tileY >= 0 && tileY < map->rows) {
                                bool isValidTile = (map->grid[tileY][tileX] == '*');
                                bool isOccupied = false;
                                for (const auto* tower : placedTowers) {
                                    if (tower->getTilePos().x == tileX && tower->getTilePos().y == tileY) { isOccupied = true; break; }
                                }
                                int cost = 0; SDL_Texture* projTex = nullptr, * turretTex = nullptr;
                                switch (selectedBuildType) {
                                case TowerType::BASIC:    cost = BASIC_COST; projTex = texProjectileBasic; turretTex = texTurretBasic; break;
                                case TowerType::PIERCING: cost = PIERCE_COST; projTex = texProjectilePierce; turretTex = texTurretPierce; break;
                                case TowerType::MINIGUN:  cost = MINIGUN_COST; projTex = texProjectileMinigun; turretTex = texTurretMinigun; break;
                                case TowerType::SNIPER:   cost = SNIPER_COST; projTex = texProjectileSniper; turretTex = texTurretSniper; break;
                                default: cost = 99999;
                                }
                                bool canAfford = (playerMoney >= cost);
                                if (isValidTile && !isOccupied && canAfford) {
                                    Tower* newTower = nullptr;
                                    switch (selectedBuildType) {
                                    case TowerType::BASIC:    newTower = new BasicTurret(tileX, tileY, TILE_W, TILE_H, projTex); break;
                                    case TowerType::PIERCING: newTower = new PiercingTurret(tileX, tileY, TILE_W, TILE_H, projTex); break;
                                    case TowerType::MINIGUN:  newTower = new MinigunTurret(tileX, tileY, TILE_W, TILE_H, projTex); break;
                                    case TowerType::SNIPER:   newTower = new SniperTurret(tileX, tileY, TILE_W, TILE_H, projTex); break;
                                    default: break;
                                    }
                                    if (newTower) {
                                        newTower->setTexture(turretTex);
                                        placedTowers.push_back(newTower);
                                        playerMoney -= newTower->getCost();
                                        showBuildPreview = false;
                                        map->grid[tileY][tileX] = 'T'; //đổi ô tường thành ô tower
                                    }
                                }
                            }
                        }
                    }
                    else if (event.button.button == SDL_BUTTON_RIGHT) {
                        showBuildPreview = false;
                    }
                }
                break;

            case GameState::GAME_OVER:
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) { currentState = GameState::MENU; resetGameState(); }
                break;
            case GameState::OPTIONS:
                break;
            case GameState::GAME_WON:
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
                    currentState = GameState::MENU;
                    resetGameState();
                }
                break;
            }
        }

        switch (currentState) {
        case GameState::MENU: break;
        case GameState::CHOOSE_STAGE:
        {
            int selectedStage = chooseStage();
            if (selectedStage != -1) {
                resetGameState();
                std::string filename = "stage" + std::to_string(selectedStage) + ".txt";
                try {
                    map->loadMap(filename);
                }
                catch (const std::exception& e) {
                    std::cerr << "map load fail "<< std::endl; currentState = GameState::MENU; break;
                }

                currentState = GameState::PLAYING;
            }
            else {
                if (currentState == GameState::CHOOSE_STAGE) currentState = GameState::MENU;
            }
        }
        break;

        case GameState::PLAYING:
        {
            int enemiesThisWave = enemiesPerWaveBase + currentWave * enemiesPerWaveScaling + (currentWave * currentWave) / 3;

            if (!waveActive) {
                waveDelayTimer += deltaTime;
                if (waveDelayTimer >= waveDelay) {
                    if (currentWave < MAX_WAVES) {
                        waveActive = true;
                        currentWave++;
                        waveEnemyCount = 0;
                        spawnTimer = 0.0f;
                        std::cout << "wave " << currentWave << " " << enemiesThisWave << " enemies" << std::endl; //**

                        if (currentWave >= MAX_WAVES) {
                            currentState = GameState::GAME_WON;
                        }
                    }
                }
            }

            int bossCount = getBossCountForWave(currentWave);
            int totalEnemiesThisWave = enemiesThisWave + bossCount;
            if (waveActive && waveEnemyCount < totalEnemiesThisWave && map && !map->getPath().empty()) {
                spawnTimer += deltaTime;
                if (spawnTimer >= spawnInterval) {
                    spawnTimer -= spawnInterval;
                    if (bossCount > 0 && waveEnemyCount < bossCount) {
                        // Spawn boss
                        int bossHP = 650 + 500 * (currentWave / 3);
                        float bossSpeed = 45.0f;
                        int bossReward = 300 + 100 * (currentWave / 3);
                        int bossW = 80, bossH = 80;
                        SDL_Point startPosPixels = map->getStartPixelPosition();
                        Enemy* boss = new Enemy(static_cast<float>(startPosPixels.x), static_cast<float>(startPosPixels.y), bossHP, bossSpeed, bossReward, bossW, bossH);
                        boss->setTexture(monsterTextureBoss);
                        boss->setPath(map->getPath());
                        activeEnemies.push_back(boss);
                    } else {
                        // Spawn quái thường như cũ
                        EnemyType typeToSpawn;
                        int typeChoice = rand() % 10;
                        if (typeChoice < 5) typeToSpawn = EnemyType::MONSTER_NORMAL;
                        else if (typeChoice < 8) typeToSpawn = EnemyType::MONSTER_TOUGH;
                        else typeToSpawn = EnemyType::MONSTER_FAST;

                        SDL_Texture* selectedTexture = nullptr;
                        int hp = 0;
                        float speed = 0;
                        int reward = 0;
                        int w = 50, h = 50;

                        switch (typeToSpawn) {
                        case EnemyType::MONSTER_NORMAL:
                            selectedTexture = monsterTexture1;
                            hp = 50 + currentWave * 18 + (int)(50 * currentWave * 0.28f);
                            speed = 70.0f;
                            reward = 8 + currentWave * 1;
                            w = 48; h = 48;
                            break;
                        case EnemyType::MONSTER_TOUGH:
                            selectedTexture = monsterTexture2;
                            hp = 100 + currentWave * 28 + (int)(100 * currentWave * 0.34f);
                            speed = 50.0f;
                            reward = 10 + currentWave * 2;
                            w = 55; h = 55;
                            break;
                        case EnemyType::MONSTER_FAST:
                            selectedTexture = monsterTexture3;
                            hp = 35 + currentWave * 13 + (int)(35 * currentWave * 0.22f);
                            speed = 100.0f;
                            reward = 6 + currentWave * 1;
                            w = 45; h = 45;
                            break;
                        }

                        SDL_Point startPosPixels = map->getStartPixelPosition();
                        if (startPosPixels.x != -1) {
                            Enemy* newEnemy = new Enemy(static_cast<float>(startPosPixels.x), static_cast<float>(startPosPixels.y), hp, speed, reward, w, h);
                            newEnemy->setTexture(selectedTexture);
                            newEnemy->setPath(map->getPath());
                            activeEnemies.push_back(newEnemy);
                        }
                        else {
                            waveActive = false;
                        }
                    }
                    waveEnemyCount++;
                }
            }

            for (Tower* t : placedTowers) t->update(deltaTime, activeEnemies, activeProjectiles);
            for (Enemy* e : activeEnemies) e->update(deltaTime);
            for (Projectile* p : activeProjectiles) p->update(deltaTime);
            for (Projectile* p : activeProjectiles) if (p->isActive()) p->checkCollision(activeEnemies);
            activeEnemies.erase(std::remove_if(activeEnemies.begin(), activeEnemies.end(), [&](Enemy* e) {
                bool r = false;
                if (e->hasReachedEnd()) { playerHealth--; r = true; }
                else if (e->isDead()) { playerMoney += e->getReward(); r = true; }
                if (r) { delete e; return true; }
                return false;
                }), activeEnemies.end());
            activeProjectiles.erase(std::remove_if(activeProjectiles.begin(), activeProjectiles.end(), [](Projectile* p) {
                if (!p->isActive()) { delete p; return true; }
                return false;
                }), activeProjectiles.end());

            if (waveActive && activeEnemies.empty() && waveEnemyCount >= totalEnemiesThisWave) {
                waveActive = false;
                waveDelayTimer = 0.0f;
                int reward = WAVE_COMPLETION_REWARD_BASE + (currentWave * WAVE_COMPLETION_REWARD_SCALING);
                playerMoney += reward;
                std::cout << "wave " << currentWave << " cleared +" << reward << " money " << playerMoney << std::endl; //**

                if (currentWave >= MAX_WAVES) {
                    currentState = GameState::GAME_WON;
                }
            }

            if (playerHealth <= 0) {
                playerHealth = 0;
                currentState = GameState::GAME_OVER;
            }
        }
        break;

        case GameState::GAME_OVER: break;
        case GameState::OPTIONS: break;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); SDL_RenderClear(renderer);
        switch (currentState) {
        case GameState::MENU: renderMenu(); break;
        case GameState::CHOOSE_STAGE: break;
        case GameState::PLAYING:
            if (map) map->render(renderer);
            for (Tower* t : placedTowers) t->render(renderer);
            for (Enemy* e : activeEnemies) e->render(renderer);
            for (Projectile* p : activeProjectiles) p->render(renderer);
            renderPlacementPreview(mouseX, mouseY);
            renderToolbar(toolbarStartX, toolbarWidth);
            SDL_RenderPresent(renderer);
            break;
        case GameState::GAME_OVER:
            renderGameOver();
            break;
        case GameState::OPTIONS:
            break;
        case GameState::GAME_WON:
            renderGameWon();
            break;
        }
    }
}

void close() {
    resetGameState();
    delete map;
    map = nullptr;
    for (int i = 0; i < MAX_STAGES; ++i) if (stagePreviewTextures[i]) SDL_DestroyTexture(stagePreviewTextures[i]);
    if (monsterTexture1) SDL_DestroyTexture(monsterTexture1);
    if (monsterTexture2) SDL_DestroyTexture(monsterTexture2);
    if (monsterTexture3) SDL_DestroyTexture(monsterTexture3);
    if (monsterTextureBoss) SDL_DestroyTexture(monsterTextureBoss);
    if (texTurretBasic) SDL_DestroyTexture(texTurretBasic);
    if (texTurretPierce) SDL_DestroyTexture(texTurretPierce);
    if (texTurretMinigun) SDL_DestroyTexture(texTurretMinigun);
    if (texTurretSniper) SDL_DestroyTexture(texTurretSniper);
    if (texProjectileBasic) SDL_DestroyTexture(texProjectileBasic);
    if (texProjectilePierce) SDL_DestroyTexture(texProjectilePierce);
    if (texProjectileMinigun) SDL_DestroyTexture(texProjectileMinigun);
    if (texProjectileSniper) SDL_DestroyTexture(texProjectileSniper);
    if (texIconBasic) SDL_DestroyTexture(texIconBasic);
    if (texIconPierce) SDL_DestroyTexture(texIconPierce);
    if (texIconMinigun) SDL_DestroyTexture(texIconMinigun);
    if (texIconSniper) SDL_DestroyTexture(texIconSniper);
    if (font) TTF_CloseFont(font);
    if (logoFont) TTF_CloseFont(logoFont);
    if (uiFont) TTF_CloseFont(uiFont);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SoundManager::getInstance().close();
    IMG_Quit();
    TTF_Quit(); 
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    srand(time(0));
    if (!init()) { 
        return -1; 
    }
    gameLoop();
    close();
    return 0;
}