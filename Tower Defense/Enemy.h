#pragma once
#ifndef ENEMY_H
#define ENEMY_H

#include <SDL.h>
#include <vector>
#include <string>
#include <cmath>

class Enemy {
public:
    float x, y; //tọa độ địch
    float speed;
    int health;
    int maxHealth;
    int reward;
    int width, height;
    bool active;
    bool reachedEnd;
    bool isBoss = false;

private:
    SDL_Texture* texture;
    std::vector<SDL_Point> path;
    size_t pathIndex;

public:
    Enemy(float startX, float startY, int hp, float spd, int rwd, int w, int h);
    ~Enemy();

    void setTexture(SDL_Texture* tex);
    void setPath(const std::vector<SDL_Point>& newPath);
    void update(float deltaTime);
    void render(SDL_Renderer* renderer);
    void takeDamage(int damage);
    bool isDead() const;
    bool hasReachedEnd() const;
    bool isActive() const;
    int getReward() const;
    SDL_FPoint getPosition() const;
    SDL_Rect getBoundingBox() const;
    void setBoss(bool boss) { isBoss = boss; }
    bool getBoss() const { return isBoss; }
};

#endif