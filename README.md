# Tower Defense

## Giới thiệu

Một tựa game xây trụ bảo vệ căn cứ khỏi quái vật qua từng đợt quái, được truyền cảm hứng phong cách qua tựa game Diep.io

## Khởi đầu

### Cần thiết

* SDL2_TTF, SDL2_IMAGE, SDL2, SDL2_MIXER
* Visual Studio Code 2022 để compile dễ dàng nhất (link hết lib vả include của SDL)

### Cài đặt
(Sẽ được thêm chi tiết sau)
* Tải toàn bộ trên repo về
* Vào file .exe qua x64/Release/Tower Defense.exe

### Cách chơi
(Sẽ được thêm chi tiết sau)
* Sau khi compile hoặc chạy file .exe trong release
* Ấn vào START
* Chọn màn từ 1-10
* Bên phải là thanh công cụ giúp người chơi biết được Máu, Tiền, có thể chọn các trụ bằng cách click chuột hoặc dùng bàn phím (1-4)
* Người chơi có 20 máu, nếu 20 con quái đi vào căn cứ thì người chơi sẽ thua. Ngược lại nếu phòng thủ được đến cuối thì người chơi thắng.

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

## Cảm hứng

* [Agar.io](https://agar.io)
  * Cảm hứng Artstyle
* [Funneractic](https://www.youtube.com/@funneractic)
  * Lấy Assets các loại Ô, các loại quái, ý tưởng làm game 
