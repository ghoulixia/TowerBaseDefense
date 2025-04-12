#include "Projectile.h"
#include "Enemy.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//constructors
Projectile::Projectile(float startX, float startY, float targetX, float targetY, float spd, int dmg, float life, SDL_Texture* tex, int pierce)
    : x(startX), y(startY), speed(spd), damage(dmg), active(true), lifetime(life), texture(tex), width(0), height(0), pierceCount(pierce > 0 ? pierce : 1)
{
    float deltaX = targetX - startX;
    float deltaY = targetY - startY;
    float distance = std::sqrt(deltaX * deltaX + deltaY * deltaY);

    if (distance > 0) {
        velocityX = (deltaX / distance) * speed;
        velocityY = (deltaY / distance) * speed;
    }
    updateDimensions();
}

Projectile::Projectile(float startX, float startY, float angleRad, float spd, int dmg, float life, SDL_Texture* tex, int pierce)
    : x(startX), y(startY), speed(spd), damage(dmg), active(true), lifetime(life),
    texture(tex), width(0), height(0), pierceCount(pierce > 0 ? pierce : 1)
{
    velocityX = cos(angleRad) * speed;
    velocityY = sin(angleRad) * speed;
    updateDimensions();
}

Projectile::~Projectile() {
}

void Projectile::updateDimensions() {
    if (texture) {
        if (SDL_QueryTexture(texture, nullptr, nullptr, &width, &height) != 0) {
            width = 5; height = 5;
        }
        if (width <= 0) width = 5;
        if (height <= 0) height = 5;
    }
    else {
        width = 5; height = 5; //size default
    }
}

void Projectile::update(float deltaTime) {
    if (!active) return;

    x += velocityX * deltaTime;
    y += velocityY * deltaTime;

    lifetime -= deltaTime;
    if (lifetime <= 0.0f) {
        active = false;
        return;
    }
}

void Projectile::render(SDL_Renderer* renderer) {
    if (!active || !texture || !renderer) return;

    SDL_Rect destRect = {
        static_cast<int>(x - width / 2.0f),
        static_cast<int>(y - height / 2.0f),
        width,
        height
    };

    //rotate đạn để theo kịp nòng
    float angleDeg = atan2(velocityY, velocityX) * 180.0f / M_PI;

    SDL_RenderCopyEx(renderer, texture, nullptr, &destRect, angleDeg, nullptr, SDL_FLIP_NONE);
}

void Projectile::checkCollision(std::vector<Enemy*>& enemies) {
    if (!active || pierceCount <= 0) return; 

    SDL_Rect projRect = { static_cast<int>(x - width / 2.0f), static_cast<int>(y - height / 2.0f), width, height };

    for (Enemy* enemy : enemies) {
        if (!enemy || !enemy->isActive()) continue;
        bool alreadyHit = false;
        for (const Enemy* hit : hitEnemies) {
            if (hit == enemy) {
                alreadyHit = true;
                break;
            }
        }
        if (alreadyHit) continue;

        //aabb collision check
        SDL_Rect enemyRect = enemy->getBoundingBox();
        if (SDL_HasIntersection(&projRect, &enemyRect)) {
            onHit(enemy);
            if (pierceCount <= 0) {
                active = false;
                break;
            }
        }
    }
}

void Projectile::onHit(Enemy* enemy) {
    if (!enemy) return;
    enemy->takeDamage(damage);
    hitEnemies.push_back(enemy);
    pierceCount--;
    if (pierceCount <= 0) {
        active = false;
    }
}

bool Projectile::isActive() const {
    return active;
}

void Projectile::setActive(bool state) {
    active = state;
}