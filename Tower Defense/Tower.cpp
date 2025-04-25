#include "Tower.h"
#include "Projectile.h"
#include "Enemy.h"
#include <cmath>
#include <limits>
#include "SoundManager.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//tower cơ sở
float Tower::distanceSq(float x1, float y1, float x2, float y2) {
    float dx = x1 - x2;
    float dy = y1 - y2;
    return dx * dx + dy * dy;
}

Tower::Tower(int tx, int ty, int TILE_WIDTH, int TILE_HEIGHT, TowerType t, int c, float rng, float rate)
    : tileX(tx), tileY(ty),
    pixelX(tx* TILE_WIDTH + TILE_WIDTH / 2.0f),
    pixelY(ty* TILE_HEIGHT + TILE_HEIGHT / 2.0f),
    cost(c), range(rng), fireRate(rate > 0 ? rate : 0.1f),
    fireCooldownTimer(0.0f), //để tính firerate
    angle(0.0f), //hướng nòng súng
    currentTarget(nullptr),
    turretTexture(nullptr),
    type(t)
{
}

Tower::~Tower() {
}

void Tower::setTexture(SDL_Texture* turretTex) {
    turretTexture = turretTex;
}

void Tower::update(float deltaTime, std::vector<Enemy*>& enemies, std::vector<Projectile*>& projectiles) {
    //cooldown timer
    if (fireCooldownTimer > 0.0f) {
        fireCooldownTimer -= deltaTime;
    }
    if (currentTarget) {
        //check xem quái còn trong tầm bắn
        if (!currentTarget->isActive() || distanceSq(pixelX, pixelY, currentTarget->x, currentTarget->y) > range * range) {
            currentTarget = nullptr; //0 có
        }
    }
    if (!currentTarget) {
        findTarget(enemies);
    }
    if (currentTarget) {
        aimAtTarget(deltaTime);

        if (fireCooldownTimer <= 0.0f) {
            fire(projectiles);
            fireCooldownTimer = 1.0f / fireRate;
        }
    }
}

void Tower::render(SDL_Renderer* renderer) {
    if (!renderer) return;

    int texW = 0, texH = 0; //biến giữ width height texture

    //render phần xoay
    if (turretTexture) {
        if (SDL_QueryTexture(turretTexture, nullptr, nullptr, &texW, &texH) == 0) {
            SDL_Rect turretDestRect = { static_cast<int>(pixelX - texW / 2.0f), static_cast<int>(pixelY - texH / 2.0f), texW, texH };
            //đổi radian sang độ
            double angleDegrees = angle * (180.0 / M_PI) + 90.0;

            SDL_RenderCopyEx(renderer, turretTexture, nullptr, &turretDestRect, angleDegrees, nullptr, SDL_FLIP_NONE);
        }
    }
}

int Tower::getCost() const { return cost; }
SDL_Point Tower::getTilePos() const { return { tileX, tileY }; }
TowerType Tower::getType() const { return type; }

void Tower::findTarget(std::vector<Enemy*>& enemies) {
    Enemy* closestEnemy = nullptr;
    float minDistanceSq = range * range;

    for (Enemy* enemy : enemies) {
        if (enemy && enemy->isActive()) {
            float distSq = distanceSq(pixelX, pixelY, enemy->x, enemy->y);
            if (distSq < minDistanceSq) {
                minDistanceSq = distSq;
                closestEnemy = enemy;
            }
        }
    }
    currentTarget = closestEnemy;
}

void Tower::aimAtTarget(float deltaTime) {
    if (!currentTarget) return;
    float targetAngle = atan2(currentTarget->y - pixelY, currentTarget->x - pixelX);
    angle = targetAngle;
}

BasicTurret::BasicTurret(int tx, int ty, int TILE_WIDTH, int TILE_HEIGHT, SDL_Texture* projTex)
    : Tower(tx, ty, TILE_WIDTH, TILE_HEIGHT, TowerType::BASIC, BASIC_COST, BASIC_RANGE, BASIC_FIRE_RATE),
    projectileTexture(projTex)
{
}

BasicTurret::~BasicTurret() {}

void BasicTurret::setProjectileTexture(SDL_Texture* tex) { projectileTexture = tex; }

void BasicTurret::fire(std::vector<Projectile*>& projectiles) {
    if (!currentTarget || !projectileTexture) return;
    projectiles.push_back(new Projectile(
        pixelX, pixelY,
        angle,
        BASIC_PROJ_SPEED,
        BASIC_PROJ_DAMAGE,
        BASIC_PROJ_LIFETIME,
        projectileTexture,
        1
    ));
    SoundManager::getInstance().playShoot();
}

PiercingTurret::PiercingTurret(int tx, int ty, int TILE_WIDTH, int TILE_HEIGHT, SDL_Texture* projTex)
    : Tower(tx, ty, TILE_WIDTH, TILE_HEIGHT, TowerType::PIERCING, PIERCE_COST, PIERCE_RANGE, PIERCE_FIRE_RATE),
    projectileTexture(projTex), projectilePierceAmount(PIERCE_AMOUNT)
{
}

PiercingTurret::~PiercingTurret() {}

void PiercingTurret::setProjectileTexture(SDL_Texture* tex) { projectileTexture = tex; }

void PiercingTurret::fire(std::vector<Projectile*>& projectiles) {
    if (!currentTarget || !projectileTexture) return;
    projectiles.push_back(new Projectile(
        pixelX, pixelY,
        angle,
        PIERCE_PROJ_SPEED,
        PIERCE_PROJ_DAMAGE,
        PIERCE_PROJ_LIFETIME,
        projectileTexture,
        projectilePierceAmount
    ));
    SoundManager::getInstance().playShoot();
}

MinigunTurret::MinigunTurret(int tx, int ty, int TILE_WIDTH, int TILE_HEIGHT, SDL_Texture* projTex)
    : Tower(tx, ty, TILE_WIDTH, TILE_HEIGHT, TowerType::MINIGUN, MINIGUN_COST, MINIGUN_RANGE, MINIGUN_FIRE_RATE),
    projectileTexture(projTex)
{
}

MinigunTurret::~MinigunTurret() {}

void MinigunTurret::setProjectileTexture(SDL_Texture* tex) { projectileTexture = tex; }

void MinigunTurret::fire(std::vector<Projectile*>& projectiles) {
    if (!currentTarget || !projectileTexture) return;

    projectiles.push_back(new Projectile(
        pixelX, pixelY,
        angle,
        MINIGUN_PROJ_SPEED,
        MINIGUN_PROJ_DAMAGE,
        MINIGUN_PROJ_LIFETIME,
        projectileTexture,
        1
    ));
    SoundManager::getInstance().playShoot();
}

SniperTurret::SniperTurret(int tx, int ty, int TILE_WIDTH, int TILE_HEIGHT, SDL_Texture* projTex)
    : Tower(tx, ty, TILE_WIDTH, TILE_HEIGHT, TowerType::SNIPER, SNIPER_COST, SNIPER_RANGE, SNIPER_FIRE_RATE),
    projectileTexture(projTex)
{
}

SniperTurret::~SniperTurret() {}

void SniperTurret::setProjectileTexture(SDL_Texture* tex) { projectileTexture = tex; }

void SniperTurret::fire(std::vector<Projectile*>& projectiles) {
    if (!currentTarget || !projectileTexture) return;
    projectiles.push_back(new Projectile(
        pixelX, pixelY,
        angle,
        SNIPER_PROJ_SPEED,
        SNIPER_PROJ_DAMAGE,
        SNIPER_PROJ_LIFETIME,
        projectileTexture,
        1
    ));
    SoundManager::getInstance().playShoot();
}