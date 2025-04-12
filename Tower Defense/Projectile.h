#pragma once
#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <SDL.h>
#include <vector>
#include <string>
class Enemy;

class Projectile {
public:
    float x, y;
    float velocityX, velocityY;
    float speed;
    int damage;
    bool active;
    float lifetime;
    SDL_Texture* texture;
    int width, height;

    int pierceCount;
    std::vector<Enemy*> hitEnemies;

public:
    //cnstructors
    Projectile(float startX, float startY, float targetX, float targetY, float spd, int dmg, float life, SDL_Texture* tex, int pierce = 1);
    //constructor lấy góc radian
    Projectile(float startX, float startY, float angleRad, float spd, int dmg, float life, SDL_Texture* tex, int pierce = 1);

    ~Projectile();

    void update(float deltaTime);
    void render(SDL_Renderer* renderer);

    void checkCollision(std::vector<Enemy*>& enemies);
    bool isActive() const;
    void setActive(bool state);

private:
    void onHit(Enemy* enemy);
    void updateDimensions();
};

#endif