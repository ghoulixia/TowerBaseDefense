# Tower Defense - NHM 

* Thử nghiệm game: 
* Phá đảo game: Chưa có

## Giới thiệu

Một tựa game xây trụ bảo vệ căn cứ khỏi quái vật qua từng đợt quái, được truyền cảm hứng phong cách qua tựa game Diep.io

## Cách tải Game

### 1. Không bao gồm code
* Tải game (được nén thành .zip) tại link sau: https://github.com/ghoulixia/TowerBaseDefense/releases/tag/1.4
* Giải nén file .zip và chạy Tower Defense.exe để chơi

### 2. Tự Build lại bằng IDE

* Một IDE để compile c++
* Cần tải các thư viện SDL cần thiết [SDL_TTF](https://github.com/libsdl-org/SDL_ttf/releases/tag/release-2.24.0), [SDL_IMAGE](https://github.com/libsdl-org/SDL_image/releases/tag/release-2.8.8), [SDL2](https://github.com/libsdl-org/SDL/releases/tag/release-2.32.4), [SDL_MIXER](https://github.com/libsdl-org/SDL_mixer/releases/tag/release-2.8.1)
* Setup project với các thư viện SDL
* Tải Code trên repo về và rebuild
 ![image](Tower%20Defense/Preview/download.png)


## Cách chơi

* Khi mở game ta sẽ thấy màn hình như sau
![image](Tower%20Defense/Preview/main.png)

* Có 3 lựa chọn START OPTIONS EXIT
   * Để thoát game, ta ấn vào EXIT
  
   * Nếu muốn chỉnh cài đặt, ấn vào OPTIONS
   ![image](Tower%20Defense/Preview/option.png)

      *Sử dụng nút mũi tên trái, phải để tăng, giảm âm lượng game


   * Để bắt đầu chơi ta click nút START

## Tiến trình (Update log)

* 0.1 (15/3/2025)
    * Hoàn thiện game menu
* 1.0 (13/4/2025)
    * Hoàn thiện khá nhiều core gameplay
    * GameMap
      * Quản lý lưới ô bản đồ (grid)
      * Đọc dữ liệu bản đồ từ file .txt, tải texture tương ứng với từng loại ô
      * Tìm đường đi cho quái (BFS pathfinding)
    * Enemy
      * Cơ chế di chuyển và xử lý trạng thái của quái
      * 3 loại quái: Normal, Fast, Tough
    * Projectile
      * Logic đạn xuyên và tương tác mục tiêu (axis-aligned bounding boxes collision check)
    * Tower
      * Quản lý và xử lý hành vi trụ
      * Tạo các loại trụ khác nhau: Basic, Piercing, Minigun, Sniper (kế thừa từ lớp Tower)
    * Thêm folder Data chứa assets của game (icon, tower, font chữ, quái, ...)
    * Hiện tại có 3 màn: Stage 1, Stage 2, Stage 5
    * Tính năng muốn thêm trong những bản tiếp theo:
      * Làm game cân bằng hơn, hiện tại game đang rất dễ
      * Thêm cơ chế quái Boss
      * Thêm nốt file stage
      * Hoàn thiện phần Options
      * Thêm Sound Effects cho game (quan trọng)
      * Có thể sẽ thêm chức năng phá trụ
      * Có thể thêm loại quái, loại trụ
* 1.1
   * Thêm SoundManager và các Sound Effects
* 1.2
   * Thêm Sound trong phần Settings
   * Thêm cơ chế upgrade level của trụ
* 1.3
   * Thêm cơ chế Boss
   * Giảm max waves từ 15 -> 9
   * Balance lại game, tăng độ khó đáng kể
* 1.4
   * Rework/Thêm stage 1 -> 10 

## Cảm hứng

* [Agar.io](https://agar.io)
* [Diep.io](https://diep.io/)
  * Cảm hứng Artstyle
* [Funneractic](https://www.youtube.com/@funneractic)
  * Lấy Assets các loại Ô, các loại quái, ý tưởng làm game 
