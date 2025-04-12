#pragma once
#ifndef TOWER_H
#define TOWER_H

#include <SDL.h>
#include <vector>
#include <string>

class Enemy;
class Projectile;


const int BASIC_COST = 50;
const float BASIC_RANGE = 160.0f;
const float BASIC_FIRE_RATE = 2.0f;
const int BASIC_PROJ_DAMAGE = 12;
const float BASIC_PROJ_SPEED = 350.0f;
const float BASIC_PROJ_LIFETIME = 0.5f;

const int PIERCE_COST = 150;
const float PIERCE_RANGE = 200.0f;
const float PIERCE_FIRE_RATE = 0.6f;
const int PIERCE_PROJ_DAMAGE = 25;
const float PIERCE_PROJ_SPEED = 280.0f;
const float PIERCE_PROJ_LIFETIME = 0.7f;
const int PIERCE_AMOUNT = 3;

const int MINIGUN_COST = 300;
const float MINIGUN_RANGE = 170.0f;
const float MINIGUN_FIRE_RATE = 8.0f;
const int MINIGUN_PROJ_DAMAGE = 5;
const float MINIGUN_PROJ_SPEED = 450.0f;
const float MINIGUN_PROJ_LIFETIME = 0.35f;

const int SNIPER_COST = 200;
const float SNIPER_RANGE = 300.0f;
const float SNIPER_FIRE_RATE = 0.5f;
const int SNIPER_PROJ_DAMAGE = 50;
const float SNIPER_PROJ_SPEED = 500.0f;
const float SNIPER_PROJ_LIFETIME = 1.0f;

enum class TowerType {
    UNKNOWN, //mặc định (chưa chọn)
    BASIC,
    PIERCING,
    MINIGUN,
    SNIPER
};

class Tower {
public:
    int tileX, tileY; 
    float pixelX, pixelY;
    int cost;
    float range;
    float fireRate;
    float fireCooldownTimer;
    float angle;
    Enemy* currentTarget;

protected:
    SDL_Texture* turretTexture;
    TowerType type;

    static float distanceSq(float x1, float y1, float x2, float y2);

public:
    Tower(int tx, int ty, int TILE_WIDTH, int TILE_HEIGHT, TowerType t, int c, float rng, float rate);
    virtual ~Tower(); //virtual destructor cho lớp tower base

    void setTexture(SDL_Texture* turretTex);

    virtual void update(float deltaTime, std::vector<Enemy*>& enemies, std::vector<Projectile*>& projectiles);
    virtual void render(SDL_Renderer* renderer);

    int getCost() const;
    SDL_Point getTilePos() const;
    TowerType getType() const;

protected:
    virtual void findTarget(std::vector<Enemy*>& enemies);
    virtual void aimAtTarget(float deltaTime);
    virtual void fire(std::vector<Projectile*>& projectiles) = 0;
};

class BasicTurret : public Tower {
private:
    SDL_Texture* projectileTexture;
public:
    BasicTurret(int tx, int ty, int TILE_WIDTH, int TILE_HEIGHT, SDL_Texture* projTex);
    virtual ~BasicTurret();
    void setProjectileTexture(SDL_Texture* tex);
    virtual void fire(std::vector<Projectile*>& projectiles) override;
};

class PiercingTurret : public Tower {
private:
    SDL_Texture* projectileTexture;
    int projectilePierceAmount;
public:
    PiercingTurret(int tx, int ty, int TILE_WIDTH, int TILE_HEIGHT, SDL_Texture* projTex);
    virtual ~PiercingTurret();
    void setProjectileTexture(SDL_Texture* tex);
    virtual void fire(std::vector<Projectile*>& projectiles) override;
};

class MinigunTurret : public Tower {
private:
    SDL_Texture* projectileTexture;
public:
    MinigunTurret(int tx, int ty, int TILE_WIDTH, int TILE_HEIGHT, SDL_Texture* projTex);
    virtual ~MinigunTurret();
    void setProjectileTexture(SDL_Texture* tex);
    virtual void fire(std::vector<Projectile*>& projectiles) override;
};

class SniperTurret : public Tower {
private:
    SDL_Texture* projectileTexture;
public:
    SniperTurret(int tx, int ty, int TILE_WIDTH, int TILE_HEIGHT, SDL_Texture* projTex);
    virtual ~SniperTurret();
    void setProjectileTexture(SDL_Texture* tex);
    virtual void fire(std::vector<Projectile*>& projectiles) override;
};

#endif