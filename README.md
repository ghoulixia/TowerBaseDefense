# Tower Defense - NHM 

* Thử nghiệm game: https://youtu.be/RhEtWx7RWhA
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

* Khi mở game ta sẽ thấy màn hình như sau:
![image](Tower%20Defense/Preview/menu.png)

* Có 3 lựa chọn START OPTIONS EXIT.
   * Để thoát game, ta ấn vào EXIT.
  
   * Nếu muốn chỉnh cài đặt, ấn vào OPTIONS.
   ![image](Tower%20Defense/Preview/option.png)

   * Sử dụng nút mũi tên trái, phải để tăng, giảm âm lượng game.


* Để bắt đầu chơi ta click nút START.
   * Vào màn hình chọn màn chơi từ 1 -> 10.
  ![image](Tower%20Defense/Preview/stage.png)
   * Độ khó từng màn sẽ tăng dần từ 1 đến 10 bằng cách giảm đường đi, giảm tường có thể đặt trụ.
   * Chọn một màn


## Trong màn chơi
* Khi vào game ta sẽ thấy một thanh công cụ bên phải.
   * <img src="Tower%20Defense/Preview/toolbar.png" width="100"/>

* Người chơi bắt đầu với:
  * 250 vàng
  * 20 máu
* Wave tượng chưng số đợt quái, bắt đầu với đợt 1.
* Có thể click vào trụ để chọn trụ và click vào Ô Tường để xây <img src="Tower%20Defense/Data/wall.bmp" width="25"/>
  * Khi đang chọn trụ, nếu muốn hủy có thể ấn ESC
* Ấn E để xóa những trụ đã đặt mà mình mong muốn phá bỏ (lưu ý không được hoàn lại tiền mà chỉ xóa trụ)
* Ấn U vào trụ đã xây để nâng cấp:
  * Lvl 1: lvl bắt đầu khi trụ được xây
  * Lvl 2: 50 vàng, trụ được tăng tầm bắn và tốc độ ra đạn, sprite trụ cũng sẽ to hơn để hiển thị
  * Lvl 3: 100 vàng, cấp tối đa, tăng tầm bắn, tốc độ ra đạn, sprite trụ to hơn

* Màn chơi có 9 đợt, số lượng quái sẽ tăng dần qua mỗi đợt. Ở đợt 3, 6, 9 sẽ sinh ra quái Boss
* Mỗi màn sẽ có một Ô sinh quái <img src="Tower%20Defense/Data/spawn.bmp" width="25"/> và quái sẽ di chuyển đến Ô căn cứ <img src="Tower%20Defense/Data/base.bmp" width="25"/>

## Thông tin
1. Các loại trụ
  
|                                                                             | Tên trụ	     | Thông tin                                                                                                                   |
|-----------------------------------------------------------------------------|--------------|------------------------------------------------------------------------------------------------------------------------------|
| <img src="Tower%20Defense/Preview/icon_basic.bmp" width="50"/>              | Basic	       | 50 vàng, trụ cơ bản                                                                                                          |
| <img src="Tower%20Defense/Preview/icon_pierce.bmp" width="50"/>             | Pierce	      | 150 vàng, trụ bắn xuyên, có thể xuyên qua kẻ địch, nên đặt ở chỗ kẻ địch thẳng hàng                                          |
| <img src="Tower%20Defense/Preview/icon_minigun.bmp" width="50"/>            | Minigun	     | 300 vàng, trụ bắn nhanh, có thể gây nhiều sát thương trong tầm gần                                                           |
| <img src="Tower%20Defense/Preview/icon_sniper.bmp" width="50"/>             | Sniper	      | 200 vàng, trụ bắn xa, có thể bắn từ rất xa với sát thương cao nhưng tốc độ ra đạn chậm                                       |

2. Các loại quái

|                                                                             | Loại quái	   | Thông tin                                                                                                                    |
|-----------------------------------------------------------------------------|--------------|------------------------------------------------------------------------------------------------------------------------------|
| <img src="Tower%20Defense/Preview/monster1.bmp" width="50"/>                | Normal       | quái cơ bản, thưởng vàng = 8 + đợt hiện tại * 1                                                                              |
| <img src="Tower%20Defense/Preview/monster2.bmp" width="50"/>                | Tough	       | quái trâu, nhiều máu, đi chậm, thưởng vàng = 10 + đợt hiện tại * 2                                                           |
| <img src="Tower%20Defense/Preview/monster3.bmp" width="50"/>                | Fast	        | quái di chuyển nhanh, thưởng vàng = 6 + đợt hiện tại * 1                                                                     |
| <img src="Tower%20Defense/Preview/monster_boss1.bmp" width="50"/>           | Boss	        | quái boss, rất trâu, di chuyển nhanh hơn normal, thưởng vàng = 300 + 100 * (đợt hiện tại / 3)                                |

## Chiến thắng và thất bại
* Người chơi sẽ chiến thắng nếu thành công phòng thủ căn cứ được sau 9 đợt quái.

![image](Tower%20Defense/Preview/win.png)

* Người chơi sẽ thua nếu quái tràn vào căn cứ và lượng máu còn lại là 0.

![image](Tower%20Defense/Preview/lose.png)


## Về đồ họa và sound effects của game

* Sử dụng [GIMP](https://www.gimp.org/) để vẽ assets:
  * Quái được vẽ lại theo video của [Funneractic](https://www.youtube.com/watch?v=4OJ4fHXgf5M&t=3s), quái boss tự vẽ
  * Các nút stages tự vẽ
  * Basic turret cảm hứng từ [Diep.io](https://diep.io/)
  * Pierce, Minigun, Sniper turret và các loại đạn tự vẽ lại giống basic turret và basic projectiles
* Sound effect:
  * Tiếng bắn: [Funneractic](https://www.youtube.com/watch?v=j9T1KkP-IFg)
  * Tiếng chọn trụ: https://pixabay.com/sound-effects/retro-select-236670/
  * Tiếng Win, Lose:  https://mixkit.co/free-sound-effects/game/
* Font chữ:
  * font ui: [Verdana-bold](https://fontzone.net/font-details/verdana-bold)
  * font logo: [MightySouly](https://www.1001fonts.com/mighty-souly-font.html)
  * font menu: [NintendoBoldRM8E](https://www.fontspace.com/category/gaming)
 
## Về source code

* GameMap.h / GameMap.cpp
  * BFS Pathfinding: Tìm đường đi ngắn nhất cho Enemy trên lưới bản đồ bằng thuật toán BFS (Breadth-First Search).
  * Kiểm tra ô hợp lệ: Kiểm tra xem một ô có thể đặt tháp hoặc Enemy có thể đi qua không (isWalkable). 
* Tower.h / Tower.cpp
  * Tìm mục tiêu (Linear Search): Duyệt qua danh sách Enemy để tìm mục tiêu gần nhất trong phạm vi bắn.
  * Quản lý thời gian hồi chiêu: Sử dụng biến timer/cooldown để kiểm soát tốc độ bắn của tháp.
  * Tính toán góc xoay: Sử dụng hàm lượng giác để xoay tháp về phía mục tiêu.
* Enemy.h / Enemy.cpp
  * Di chuyển theo đường đi: Enemy di chuyển theo danh sách các điểm (path) đã được tìm bằng BFS.
  * Kiểm tra trạng thái sống/chết: Xử lý khi Enemy bị tiêu diệt hoặc đến đích.
* Projectile.h / Projectile.cpp
  * Kiểm tra va chạm (Circle Collision): Tính khoảng cách giữa Projectile và Enemy để xác định va chạm.
  * Quản lý thời gian sống: Đạn có thời gian tồn tại, tự động hủy khi hết thời gian hoặc va chạm.
* SoundManager.h / SoundManager.cpp
  * Quản lý phát âm thanh: Sử dụng SDL_mixer để load và phát hiệu ứng âm thanh khi có sự kiện (bắn, thắng, thua...). 
  * Điều chỉnh âm lượng: Cho phép tăng/giảm âm lượng hoặc tắt/mở âm thanh.
* main.cpp
  * Tải tài nguyên: Load ảnh, âm thanh, font chữ khi khởi tạo game.
  * Game Loop: Vòng lặp chính cập nhật trạng thái game, nhận sự kiện, vẽ màn hình.
  * Xử lý sự kiện người dùng: Nhận và xử lý sự kiện chuột, bàn phím để đặt/xóa tháp, bắt đầu game, pause, ...
  * Quản lý đối tượng động: Thêm/xóa Enemy, Projectile, Tower khỏi danh sách khi cần thiết.
  * Kiểm tra điều kiện thắng/thua: Kiểm tra số máu căn cứ, số Enemy còn lại, số wave đã vượt qua để xác định trạng thái game. 


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
* 1.1  (25/4/2025)
   * Thêm SoundManager và các Sound Effects
* 1.2
   * Thêm Sound trong phần Settings
   * Thêm cơ chế upgrade level của trụ
* 1.3  (26/4/2025)
   * Thêm cơ chế Boss
   * Giảm max waves từ 15 -> 9
   * Balance lại game, tăng độ khó đáng kể
* 1.4
   * Rework/Thêm stage 1 -> 10 
