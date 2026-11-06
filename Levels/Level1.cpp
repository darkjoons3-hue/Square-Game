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

    // --- ПЛАТФОРМЫ (по сетке 50х50) ---
    
    // Стартовый пол
    platforms.push_back({0, 650, 400, 50});
    
    // Парящие блоки (змейка)
    platforms.push_back({500, 600, 50, 50});
    platforms.push_back({550, 550, 50, 50});
    platforms.push_back({600, 500, 50, 50});
    platforms.push_back({650, 450, 50, 50});
    platforms.push_back({700, 400, 50, 50});

    // Платформа с шипами (по центру)
    platforms.push_back({800, 650, 300, 50});
    spikes.push_back({850, 625, 100, 25, false}); // Шипы на полу

    // Длинный пол (где стоит Замок)
    platforms.push_back({1200, 650, 400, 50});

    // Лестница вверх (справа)
    platforms.push_back({1600, 650, 50, 50});
    platforms.push_back({1650, 600, 50, 50});
    platforms.push_back({1700, 550, 50, 50});
    platforms.push_back({1750, 500, 50, 50});
    platforms.push_back({1800, 450, 50, 50});
    platforms.push_back({1850, 400, 50, 50});

    // Огромный потолок
    platforms.push_back({0, 50, LEVEL_WIDTH, 50});
    // Шипы на потолке
    spikes.push_back({800, 100, 100, 25, true}); 
    spikes.push_back({1500, 100, 150, 25, true});

    // --- ОБЪЕКТЫ ---
    
    // Ключ (лежит перед замком на земле)
    keyItem.x = 1100;
    keyItem.y = 600;

    // Замок (высота 3 блока = 150, ширина 1 блок = 50)
    lockDoor.x = 1300;
    lockDoor.y = 500;
    lockDoor.w = 50;
    lockDoor.h = 150;

    // Финиш (в левом верхнем углу на потолке)
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

    // Управление
    if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000) player.vx = -6;
    else if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000) player.vx = 6;
    else player.vx = 0;

    // Прыжок
    if ((GetAsyncKeyState(VK_SPACE) & 0x8000 || GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000) && player.onGround) {
        player.vy = -15.0f; 
        player.onGround = false;
    }

    // Физика
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

    // Проверка шипов
    for (const auto& s : spikes) {
        float sX = s.x + 5;
        float sY = s.pointingDown ? s.y : s.y + s.h - 15;
        
        if (player.x < sX + 40 && player.x + player.size > sX &&
            player.y < sY + 15 && player.y + player.size > sY) {
            RespawnPlayer(); 
            break;
        }
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

    // 2. Шипы (Рисуем GDI+)
    SolidBrush spikeBrush(Color(255, 200, 50, 50)); 
    for (const auto& s : spikes) {
        if (s.x + s.w > cameraX && s.x < cameraX + 800 && s.y + s.h > cameraY && s.y < cameraY + 600) {
            int count = (int)(s.w / 50);
            for (int i = 0; i < count; i++) {
                PointF points[3];
                float drawX = s.x - cameraX + (i * 50);
                float drawY = s.y - cameraY;

                if (s.pointingDown) { 
                    points[0] = PointF(drawX, drawY);
                    points[1] = PointF(drawX + 50, drawY);
                    points[2] = PointF(drawX + 25, drawY + 25);
                } else { 
                    points[0] = PointF(drawX, drawY + 25);
                    points[1] = PointF(drawX + 50, drawY + 25);
                    points[2] = PointF(drawX + 25, drawY);
                }
                graphics.FillPolygon(&spikeBrush, points, 3);
            }
        }
    }

    // 3. Ключ (Рисуем GDI+)
    if (!keyItem.collected) {
        if (keyItem.x + 50 > cameraX && keyItem.x < cameraX + 800 && keyItem.y + 50 > cameraY && keyItem.y < cameraY + 600) {
            float drawX = keyItem.x - cameraX + 25;
            float drawY = keyItem.y - cameraY + 25;
            SolidBrush keyBrush(Color(255, 255, 215, 0)); 
            
            // Принудительно приводим типы к INT
            graphics.FillEllipse(&keyBrush, (INT)(drawX - 10), (INT)(drawY - 10), 20, 20);
            graphics.FillRectangle(&keyBrush, (INT)drawX, (INT)(drawY - 3), 20, 6);
            graphics.FillRectangle(&keyBrush, (INT)(drawX + 12), (INT)(drawY + 3), 4, 6);
            graphics.FillRectangle(&keyBrush, (INT)(drawX + 18), (INT)(drawY + 3), 4, 8);
        }
    }

    // 4. Замок (Рисуем GDI+)
    if (!hasKey) {
        if (lockDoor.x + lockDoor.w > cameraX && lockDoor.x < cameraX + 800 && lockDoor.y + lockDoor.h > cameraY && lockDoor.y < cameraY + 600) {
            float drawX = lockDoor.x - cameraX;
            float drawY = lockDoor.y - cameraY;
            SolidBrush lockBrush(Color(255, 80, 80, 80)); 
            graphics.FillRectangle(&lockBrush, (INT)drawX, (INT)drawY, (INT)lockDoor.w, (INT)lockDoor.h);
            
            SolidBrush holeBrush(Color(255, 0, 0, 0));
            graphics.FillEllipse(&holeBrush, (INT)(drawX + 15), (INT)(drawY + 50), 20, 20);
            graphics.FillRectangle(&holeBrush, (INT)(drawX + 20), (INT)(drawY + 65), 10, 20);
        }
    }

    // 5. Финиш (Рисуем GDI+)
    if (finishFlag.x + 50 > cameraX && finishFlag.x < cameraX + 800 && finishFlag.y + 50 > cameraY && finishFlag.y < cameraY + 600) {
        float drawX = finishFlag.x - cameraX + 25;
        float drawY = finishFlag.y - cameraY + 25;
        Pen finishPen(Color(255, 0, 150, 0), 6); 
        graphics.DrawLine(&finishPen, (INT)drawX, (INT)(drawY - 15), (INT)(drawX - 15), (INT)drawY);
        graphics.DrawLine(&finishPen, (INT)(drawX - 15), (INT)drawY, (INT)drawX, (INT)(drawY + 15));
        
        graphics.DrawLine(&finishPen, (INT)(drawX + 15), (INT)(drawY - 15), (INT)drawX, (INT)drawY);
        graphics.DrawLine(&finishPen, (INT)drawX, (INT)drawY, (INT)(drawX + 15), (INT)(drawY + 15));
    }

    // 6. Игрок (player.png)
    if (playerImg && playerImg->GetLastStatus() == Ok) {
        graphics.DrawImage(playerImg, (INT)(player.x - cameraX), (INT)(player.y - cameraY), player.size, player.size);
    } else {
        SolidBrush playerBrush(Color(255, 100, 100, 100)); 
        graphics.FillRectangle(&playerBrush, (INT)(player.x - cameraX), (INT)(player.y - cameraY), player.size, player.size);
    }
}

void RenderGameOver(HDC hdc) {
    Graphics graphics(hdc);
    SolidBrush bg(Color(255, 255, 255, 255)); 
    graphics.FillRectangle(&bg, 0, 0, 800, 600);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));
    HFONT fontBig = CreateFontW(48, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Monodetermination");
    HFONT oldFont = (HFONT)SelectObject(hdc, fontBig);

    RECT textRect = { 0, 0, 800, 600 };
    DrawTextW(hdc, L"GAME OVER\n\n(\u041d\u0430\u0436\u043c\u0438 \u043c\u044b\u0448\u043a\u043e\u0439, \u0447\u0442\u043e\u0431\u044b \u0432\u044b\u0439\u0442\u0438 \u0432 \u043c\u0435\u043d\u044e)", -1, &textRect, DT_CENTER | DT_VCENTER | DT_WORDBREAK);
    
    SelectObject(hdc, oldFont);
    DeleteObject(fontBig);
}

void RenderComplete(HDC hdc) {
    Graphics graphics(hdc);
    SolidBrush bg(Color(255, 255, 255, 255)); 
    graphics.FillRectangle(&bg, 0, 0, 800, 600);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));
    HFONT fontBig = CreateFontW(48, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Monodetermination");
    HFONT oldFont = (HFONT)SelectObject(hdc, fontBig);

    RECT textRect = { 0, 0, 800, 600 };
    DrawTextW(hdc, L"COMPLETE\n\n(\u041d\u0430\u0436\u043c\u0438 \u043c\u044b\u0448\u043a\u043e\u0439, \u0447\u0442\u043e\u0431\u044b \u0432\u044b\u0439\u0442\u0438 \u0432 \u043c\u0435\u043d\u044e)", -1, &textRect, DT_CENTER | DT_VCENTER | DT_WORDBREAK);
    
    SelectObject(hdc, oldFont);
    DeleteObject(fontBig);
}
