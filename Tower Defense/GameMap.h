#pragma once
#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <SDL.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set> 
#include <queue>
#include <functional> //hashing
#include <algorithm>

struct PointHash {
    std::size_t operator()(const SDL_Point& p) const {
        auto hash1 = std::hash<int>{}(p.x);
        auto hash2 = std::hash<int>{}(p.y);
        return hash1 ^ (hash2 << 1);
    }
};

struct PointEqual {
    bool operator()(const SDL_Point& p1, const SDL_Point& p2) const {
        return p1.x == p2.x && p1.y == p2.y;
    }
};


class GameMap {
public:
    std::vector<std::vector<char>> grid;
    int rows, cols;
    int tileWidth, tileHeight;
    std::unordered_map<char, SDL_Texture*> tileTextures;
    std::vector<SDL_Point> enemyPath;
    SDL_Point startTileCoords = { -1, -1 };
    SDL_Point endTileCoords = { -1, -1 };

    GameMap(int r, int c, int tw, int th);
    ~GameMap();

    void loadTextures(SDL_Renderer* rend);
    void loadMap(const std::string& filename);
    void render(SDL_Renderer* rend);
    std::vector<SDL_Point> findPath();

    const std::vector<SDL_Point>& getPath() const;
    SDL_Point getStartPixelPosition() const;

private:
    SDL_Point tileToPixelCenter(SDL_Point tileCoords) const;
    bool isWalkable(int r, int c) const;
    void clearTextures();
};

#endif