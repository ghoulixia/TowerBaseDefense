#include "Enemy.h"
#include <SDL.h>
#include <iostream>
#include <cmath>

//constructor
Enemy::Enemy(float startX, float startY, int hp, float spd, int rwd, int w, int h)
    : x(startX), y(startY), //khởi tạo vị trí (x, y) là tâm của enemy
    speed(spd > 0 ? spd : 0), //speed ko âm
    health(hp), maxHealth(hp),
    reward(rwd),
    width(w), height(h),
    active(true),
    reachedEnd(false),
    texture(nullptr),
    pathIndex(0) 
{
}

//deconstructor
Enemy::~Enemy() {
}

void Enemy::setTexture(SDL_Texture* tex) {
    texture = tex;
}

void Enemy::setPath(const std::vector<SDL_Point>& newPath) {
    path = newPath;
    pathIndex = 0;
}

void Enemy::update(float deltaTime) {
    if (!active || path.empty() || pathIndex >= path.size()) {
        return;
    }

    SDL_Point targetPoint = path[pathIndex];
    float targetX = static_cast<float>(targetPoint.x);
    float targetY = static_cast<float>(targetPoint.y);

    float deltaX = targetX - x;
    float deltaY = targetY - y;
    float distSq = deltaX * deltaX + deltaY * deltaY;

    float moveDistance = speed * deltaTime;
    if (distSq <= (moveDistance * moveDistance) || distSq < 1.0f) {
        x = targetX;
        y = targetY;
        pathIndex++;
        if (pathIndex >= path.size()) {
            if (!reachedEnd) {
                reachedEnd = true;
                active = false;
            }
        }
    }
    else {
        float distance = std::sqrt(distSq);
        x += (deltaX / distance) * moveDistance;
        y += (deltaY / distance) * moveDistance;
    }
}

void Enemy::render(SDL_Renderer* renderer) {
    if (!active || !texture) {
        return;
    }
    SDL_Rect destRect = getBoundingBox();
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);

    if (health < maxHealth && health > 0) {
        SDL_Rect healthBarBgRect;
        healthBarBgRect.x = destRect.x;
        healthBarBgRect.y = destRect.y - 8;
        if (healthBarBgRect.y < 0) healthBarBgRect.y = 0;
        healthBarBgRect.w = width;
        healthBarBgRect.h = 5;

        SDL_Rect healthBarFgRect = healthBarBgRect;
        //tính toán chiều rộng của thanh máu còn lại dựa trên tỷ lệ phần trăm máu
        //ép kiểu sang float để phép chia không bị làm tròn thành 0.
        healthBarFgRect.w = static_cast<int>(width * (static_cast<float>(health) / maxHealth));

        //vẽ nền
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &healthBarBgRect);

        //vẽ phần máu còn lại
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &healthBarFgRect); 
    }
}


void Enemy::takeDamage(int damage) {
    if (!active) return;

    health -= damage;
    if (health <= 0) {
        health = 0; //máu không bị âm.
        active = false;
    }
}
bool Enemy::isDead() const {
    return health <= 0;
}
bool Enemy::hasReachedEnd() const {
    return reachedEnd;
}
bool Enemy::isActive() const {
    return active;
}
int Enemy::getReward() const {
    return reward;
}
SDL_FPoint Enemy::getPosition() const {
    return { x, y }; //trả về cấu trúc {x, y} sử dụng initializer list
}

//lấy hình chữ nhật bao quanh dùng để vẽ và kiểm tra va chạm
SDL_Rect Enemy::getBoundingBox() const {
    SDL_Rect box;
    box.x = static_cast<int>(x - width / 2.0f);
    box.y = static_cast<int>(y - height / 2.0f);
    box.w = width;
    box.h = height;
    return box;
}