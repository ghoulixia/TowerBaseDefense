#pragma once
#include <SDL_mixer.h>
#include <string>

class SoundManager {
public:
    static SoundManager& getInstance();
    bool init();
    void close();
    void playShoot();
    void playWinning();
    void playLosing();
    void playSelect();
    void setVolume(int volume); // volume: 0-128
    int getVolume() const;
private:
    SoundManager();
    ~SoundManager();
    Mix_Chunk* shootSound = nullptr;
    Mix_Chunk* winningSound = nullptr;
    Mix_Chunk* losingSound = nullptr;
    Mix_Chunk* selectSound = nullptr;
    bool initialized = false;
    int currentVolume = 96; // default volume
};
