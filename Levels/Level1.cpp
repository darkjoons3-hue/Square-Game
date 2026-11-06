#include "Level1.h"
#include <cmath>

using namespace Gdiplus;

const float LEVEL_WIDTH = 2500.0f;
const float LEVEL_HEIGHT = 1000.0f;
float cameraX = 0;
float cameraY = 0;

std::vector<Platform> platforms;
std::vector<Spike> spikes;
Player player;

KeyItem keyItem;
LockDoor lockDoor;
FinishFlag finishFlag;

bool hasKey = false;
bool isFallingToDeath = false;
float deathTimer = 0;

Image* playerImg = nullptr;
Image* blockImg = nullptr;

void RespawnPlayer() {
    player.x = 50;
    player.y = 600; 
    player.vx = 0;
    player.vy = 0;
    cameraX = 0;
    cameraY = 200; 
    isFallingToDeath = false;
    deathTimer = 0;
    hasKey = false;
    keyItem.collected = false;
}

void InitLevel1() {
    platforms.clear();
    spikes.clear();

    if (!playerImg) playerImg = new Image(L"assets\\Player.png");
    if (!blockImg) blockImg = new Image(L"assets\\block.png");

    // --- КАРТА ПО СКЕТЧУ ---

    // 1. Стартовая длинная земля (как на рисунке)
    platforms.push_back({0, 650, 500, 50}); 
    
    // Маленький блок, который стоит рядом со стартом (на скетче он есть)
    platforms.push_back({100, 600, 50, 50});

    // 2. ОГРОМНАЯ ПРОПАСТЬ (от 500 до 1200 пикселей - это 14 блоков пустоты!)

    // 3. Лестница (начинается далеко справа)
    platforms.push_back({1200, 650, 50, 50});
    platforms.push_back({1250, 600, 50, 50});
    platforms.push_back({1300, 550, 50, 50});
    platforms.push_back({1350, 500, 50, 50});
    platforms.push_back({1400, 450, 50, 50});
    platforms.push_back({1450, 400, 50, 50});

    // Верхняя площадка лестницы (для теста)
    platforms.push_back({1500, 400, 300, 50});

    // --- ОБЪЕКТЫ (Временные, чтобы не мешали) ---
    keyItem.x = 1550;
    keyItem.y = 350;

    lockDoor.x = 1700;
    lockDoor.y = 250;
    lockDoor.w = 50;
    lockDoor.h = 150;

    finishFlag.x = 100;
    finishFlag.y = 100;

    RespawnPlayer();
}

void UpdateLevel1() {
    if (isFallingToDeath) {
        deathTimer += 1.0f;
        if (deathTimer > 60.0f) { 
            g_state = STATE_GAME_OVER;
        }
        return; 
    }

    // --- УВЕЛИЧЕННАЯ СКОРОСТЬ ---
    float moveSpeed = 10.0f; // Было 6, стало 10 (куб будет бегать быстрее)

    if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000) player.vx = -moveSpeed;
    else if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000) player.vx = moveSpeed;
    else player.vx = 0;

    // Прыжок (оставляем прежним для 1.5 блоков)
    if ((GetAsyncKeyState(VK_SPACE) & 0x8000 || GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000) && player.onGround) {
        player.vy = -15.0f; 
        player.onGround = false;
    }

    player.vy += 1.5f; 
    player.x += player.vx;

    // Коллизия по X (с платформами)
    for (const auto& p : platforms) {
        if (player.x < p.x + p.w && player.x + player.size > p.x &&
            player.y < p.y + p.h && player.y + player.size > p.y) {
            if (player.vx > 0) player.x = p.x - player.size;
            if (player.vx < 0) player.x = p.x + p.w;
        }
    }

    // Коллизия по X (с Замком, если нет ключа)
    if (!hasKey) {
        if (player.x < lockDoor.x + lockDoor.w && player.x + player.size > lockDoor.x &&
            player.y < lockDoor.y + lockDoor.h && player.y + player.size > lockDoor.y) {
            if (player.vx > 0) player.x = lockDoor.x - player.size;
            if (player.vx < 0) player.x = lockDoor.x + lockDoor.w;
        }
    }

    if (player.x < 0) player.x = 0;
    if (player.x + player.size > LEVEL_WIDTH) player.x = LEVEL_WIDTH - player.size;

    player.y += player.vy;
    player.onGround = false;

    // Коллизия по Y (с платформами)
    for (const auto& p : platforms) {
        if (player.x < p.x + p.w && player.x + player.size > p.x &&
            player.y < p.y + p.h && player.y + player.size > p.y) {
            if (player.vy > 0) { 
                player.y = p.y - player.size; 
                player.vy = 0; 
                player.onGround = true; 
            }
            else if (player.vy < 0) { 
                player.y = p.y + p.h; 
                player.vy = 0; 
            }
        }
    }

    // Коллизия по Y (с Замком)
    if (!hasKey) {
        if (player.x < lockDoor.x + lockDoor.w && player.x + player.size > lockDoor.x &&
            player.y < lockDoor.y + lockDoor.h && player.y + player.size > lockDoor.y) {
            if (player.vy > 0) { 
                player.y = lockDoor.y - player.size; 
                player.vy = 0; 
                player.onGround = true; 
            }
            else if (player.vy < 0) { 
                player.y = lockDoor.y + lockDoor.h; 
                player.vy = 0; 
            }
        }
    }

    // Проверка подбора Ключа
    if (!keyItem.collected) {
        if (player.x < keyItem.x + 50 && player.x + player.size > keyItem.x &&
            player.y < keyItem.y + 50 && player.y + player.size > keyItem.y) {
            keyItem.collected = true;
            hasKey = true;
        }
    }

    // Проверка Финиша
    if (player.x < finishFlag.x + 50 && player.x + player.size > finishFlag.x &&
        player.y < finishFlag.y + 50 && player.y + player.size > finishFlag.y) {
        g_state = STATE_COMPLETE;
    }

    // Падение в пропасть
    if (player.y > LEVEL_HEIGHT) {
        isFallingToDeath = true; 
    }

    // Камера
    if (!isFallingToDeath) {
        float targetCamX = player.x + player.size / 2 - 400; 
        float targetCamY = player.y + player.size / 2 - 300; 
        cameraX += (targetCamX - cameraX) * 0.1f;
        cameraY += (targetCamY - cameraY) * 0.1f;
    } else {
        float targetCamX = player.x + player.size / 2 - 400; 
        cameraX += (targetCamX - cameraX) * 0.1f; 
    }

    if (cameraX < 0) cameraX = 0;
    if (cameraX > LEVEL_WIDTH - 800) cameraX = LEVEL_WIDTH - 800;
    if (cameraY < 0) cameraY = 0;
    if (cameraY > LEVEL_HEIGHT - 600) cameraY = LEVEL_HEIGHT - 600;
}

void RenderLevel1(HDC hdc) {
    Graphics graphics(hdc);
    
    // Белый фон
    SolidBrush bg(Color(255, 255, 255, 255));
    graphics.FillRectangle(&bg, 0, 0, 800, 600);

    // 1. Платформы (block.png)
    for (const auto& p : platforms) {
        if (p.x + p.w > cameraX && p.x < cameraX + 800 && p.y + p.h > cameraY && p.y < cameraY + 600) {
            if (blockImg && blockImg->GetLastStatus() == Ok) {
                graphics.DrawImage(blockImg, (INT)(p.x - cameraX), (INT)(p.y - cameraY), (INT)p.w, (INT)p.h);
            } else {
                SolidBrush platBrush(Color(255, 0, 0, 0));
                graphics.FillRectangle(&platBrush, (INT)(p.x - cameraX), (INT)(p.y - cameraY), (INT)p.w, (INT)p.h);
            }
        }
    }

    // (Шипы, Ключ и Замок временно закомментированы или сдвинуты, чтобы не мешать тесту прыжков)
    // Когда скажешь, мы вернем их на место.

    // Игрок (player.png)
    if (playerImg && playerImg->GetLastStatus() == Ok) {
        graphics.DrawImage(playerImg, (INT)(player.x - cameraX), (INT)(player.y - cameraY), player.size, player.size);
    } else {
        SolidBrush playerBrush(Color(255, 100, 100, 100)); 
        graphics.FillRectangle(&playerBrush, (INT)(player.x - cameraX), (INT)(player.y - cameraY), player.size, player.size);
    }
}

// Заглушки для экранов, чтобы не было ошибок
void RenderGameOver(HDC hdc) {}
void RenderComplete(HDC hdc) {}
