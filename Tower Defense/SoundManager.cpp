#include "SoundManager.h"
#include <iostream>

SoundManager& SoundManager::getInstance() {
    static SoundManager instance;
    return instance;
}

SoundManager::SoundManager() {}
SoundManager::~SoundManager() { close(); }

bool SoundManager::init() {
    if (initialized) return true;
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }
    shootSound = Mix_LoadWAV("Data/shoot_basic.wav");
    if (!shootSound) {
        std::cerr << "Failed to load shoot_basic.wav! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }
    winningSound = Mix_LoadWAV("Data/winning.wav");
    if (!winningSound) {
        std::cerr << "Failed to load winning.wav! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }
    losingSound = Mix_LoadWAV("Data/losing.wav");
    if (!losingSound) {
        std::cerr << "Failed to load losing.wav! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }
    selectSound = Mix_LoadWAV("Data/selecting.wav");
    if (!selectSound) {
        std::cerr << "Failed to load selecting.wav! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }
    initialized = true;
    return true;
}

void SoundManager::close() {
    if (shootSound) { Mix_FreeChunk(shootSound); shootSound = nullptr; }
    if (winningSound) { Mix_FreeChunk(winningSound); winningSound = nullptr; }
    if (losingSound) { Mix_FreeChunk(losingSound); losingSound = nullptr; }
    if (selectSound) { Mix_FreeChunk(selectSound); selectSound = nullptr; }
    if (initialized) {
        Mix_CloseAudio();
        initialized = false;
    }
}

void SoundManager::playShoot() {
    if (shootSound) { Mix_PlayChannel(-1, shootSound, 0); }
}
void SoundManager::playWinning() {
    if (winningSound) { Mix_PlayChannel(-1, winningSound, 0); }
}
void SoundManager::playLosing() {
    if (losingSound) { Mix_PlayChannel(-1, losingSound, 0); }
}
void SoundManager::playSelect() {
    if (selectSound) { Mix_PlayChannel(-1, selectSound, 0); }
}
