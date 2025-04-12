#include "GameMap.h"
#include <SDL.h>
#include <fstream>
#include <stdexcept>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

//constructor
GameMap::GameMap(int r, int c, int tw, int th)
    : rows(r), cols(c), tileWidth(tw), tileHeight(th),
    startTileCoords{ -1, -1 },
    endTileCoords{ -1, -1 }
{


    //cấp phát bộ nhớ cho vector lưới
    try {
        //khởi tạo tất cả các ô là ' '
        grid.resize(rows, std::vector<char>(cols, ' '));
    }
    catch (const std::bad_alloc& e) {
    }
}

//destructor
GameMap::~GameMap() {
    clearTextures();
}

//xoa tất cả các texture được lưu trữ trong map tileTextures
void GameMap::clearTextures() {
    tileTextures.clear();
}

void GameMap::loadTextures(SDL_Renderer* rend) {
    clearTextures();

    //nhận vào key là ký tự của ô, đường dẫn
    auto loadAndStoreTexture = [&](char key, const char* path) {
        SDL_Surface* surface = SDL_LoadBMP(path);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(rend, surface);
        tileTextures[key] = texture;

        SDL_FreeSurface(surface);
    };

    loadAndStoreTexture('#', "Data/tiles_empty.bmp"); //ô đường đi
    loadAndStoreTexture('*', "Data/wall.bmp");  //Ô tường (để xây
    loadAndStoreTexture('T', "Data/wall.bmp");  //Ô đã đặt tơwer
    loadAndStoreTexture('S', "Data/spawn.bmp");  //Ô điểm xuất hiện của quái
    loadAndStoreTexture('E', "Data/base.bmp");  //Ô căn cứ của quái
}

void GameMap::loadMap(const std::string& filename) {
    std::string fullPath = "Data/" + filename;
    std::ifstream file(fullPath);

    startTileCoords = { -1, -1 }; //tọa độ bắt đầu
    endTileCoords = { -1, -1 };
    enemyPath.clear();
   
    for (int i = 0; i < rows; ++i) { //i = hàng
        for (int j = 0; j < cols; ++j) { // j = cột
            if (!(file >> grid[i][j])) {
                return;
            } else {
                char currentTile = grid[i][j];
                if (currentTile == 'S') {
                    startTileCoords = { j, i };
                } else if (currentTile == 'E') {
                    endTileCoords = { j, i };
                }
            }
        }
    }
    file.close();

    enemyPath = findPath();
}


//vẽ grid
void GameMap::render(SDL_Renderer* rend) {
    for (int i = 0; i < rows; ++i) { //i == hàng
        for (int j = 0; j < cols; ++j) { //j = cột
            //ký tự = loại ô tại vị trí này
            char tileChar = grid[i][j];

            //find() trả về một iterator đến phần tử
            auto it = tileTextures.find(tileChar);

            SDL_Texture* tex = nullptr;
            if (it != tileTextures.end()) {
                //lấy con trỏ texture
                tex = it->second;
            }
            if (tex) {
                //tính hình chữ nhật đích trên màn hình
                //(cột * chiều rộng ô, hàng * chiều cao ô)
                SDL_Rect destRect = { j * tileWidth, i * tileHeight, tileWidth, tileHeight };
                SDL_RenderCopy(rend, tex, nullptr, &destRect);
            }
        }
    }

}

//hàm chuyển tọa độ ô (cột, hàng) sang tọa độ pixel (tâm của ô)
SDL_Point GameMap::tileToPixelCenter(SDL_Point tileCoords) const {
    return {
        (tileCoords.x * tileWidth) + tileWidth / 2,
        (tileCoords.y * tileHeight) + tileHeight / 2 
    };
}

bool GameMap::isWalkable(int r, int c) const {
    if (r < 0 || r >= rows || c < 0 || c >= cols) {
        return false; //nằm ngoài bản đồ
    }

    char tile = grid[r][c];
    return tile == '#' || tile == 'E';
}

//trả về một vector các SDL_Point đại diện cho tâm của các ô dọc theo đường đi
std::vector<SDL_Point> GameMap::findPath() {
    std::vector<SDL_Point> pathPixels;
    //khởi tạo ctdl BFS
    std::queue<SDL_Point> q; //queue để lưu trữ tọa độ các ô cần đến
    //sử dụng PointHash và PointEqual tùy chỉnh (GameMap.h) để cho phép SDL_Point làm key
    std::unordered_map<SDL_Point, SDL_Point, PointHash, PointEqual> came_from; //lưu trữ thông tin để tái tạo đường đi
    std::unordered_set<SDL_Point, PointHash, PointEqual> visited;// theo dõi các ô đã được thêm vào hàng đợi hoặc đã được xử lý  

    //thêm tọa độ ô bắt đầu vào hàng đợi và tập hợp visited.
    q.push(startTileCoords);
    visited.insert(startTileCoords);
    bool found = false; //flag tìm thấy

    while (!q.empty()) {
        // Lấy ô tiếp theo để xử lý từ đầu hàng đợi.
        SDL_Point current = q.front();
        q.pop(); //loại bỏ khỏi q

        if (current.x == endTileCoords.x && current.y == endTileCoords.y) {
            found = true; 
            break;
        }
        //định nghĩa tọa độ tương đối cho các ô lân cận (lên xuống trái phải)
        int dx[] = { 0, 0, -1, 1 };
        int dy[] = { -1, 1, 0, 0 };

        //lặp qua 4 ô lân cận
        for (int i = 0; i < 4; ++i) {
            int nextCol = current.x + dx[i];
            int nextRow = current.y + dy[i];
            SDL_Point neighbor = { nextCol, nextRow };
            if (isWalkable(nextRow, nextCol) && visited.find(neighbor) == visited.end())
            {
                visited.insert(neighbor);   //đánh dấu là đã đến
                came_from[neighbor] = current;   //đến neighbor từ current
                q.push(neighbor);
            }
        }
    }

    if (found) {
        std::vector<SDL_Point> pathTiles; //vector tạm thời để lưu đường đi bằng tọa độ ô
        SDL_Point current = endTileCoords; //bắt đầu quay lui từ ô kết thúc.

        //lui bằng cách sử dụng map came_from cho đến khi về đến ô bắt đầu
        while (!(current.x == startTileCoords.x && current.y == startTileCoords.y)) {
            pathTiles.push_back(current); //thêm ô hiện tại vào đường đi
            if (came_from.count(current)) {
                //di chuyển đến ô trước đó trong đường đi
                current = came_from[current];
            } else {
                return {};
            }
        }
        //thêm chính ô bắt đầu vào đường đi
        pathTiles.push_back(startTileCoords);

        //đảo ngược đường đi
        std::reverse(pathTiles.begin(), pathTiles.end());

        //chuyển đổi đường đi từ tọa độ ô sang tọa độ pixel (tâm của các ô)
        pathPixels.reserve(pathTiles.size()); //cấp phát trước không gian, tăng hiệu quả
        for (const auto& tileCoord : pathTiles) {
            pathPixels.push_back(tileToPixelCenter(tileCoord));
        }
    }
    return pathPixels;
}

const std::vector<SDL_Point>& GameMap::getPath() const {
    return enemyPath;
}


SDL_Point GameMap::getStartPixelPosition() const {
    if (!enemyPath.empty()) {
        return enemyPath[0];
    } else {
        return { -1, -1 }; //0 hợp lệ
    }
}

