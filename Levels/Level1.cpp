#include "Level1.h"
#include <cmath>
#include <cstdio>

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
bool hasKey = false;

bool isFallingToDeath = false;
float deathTimer = 0;
DWORD deathStartTime = 0; 

Image* playerImg = nullptr;
Image* blockImg = nullptr;
Image* keyImg = nullptr;   // Для key.png
Image* lockImg = nullptr;  // Для Lock.png

HFONT g_hudFont = nullptr;

void RespawnPlayer() {
    player.x = 100;
    player.y = 600; 
    player.vx = 0;
    player.vy = 0;
    cameraX = 0;
    cameraY = 200; 
    isFallingToDeath = false;
    deathTimer = 0;
    g_gameOverTimer = 0;
    hasKey = false;
    keyItem.collected = false;
}

void InitLevel1() {
    platforms.clear();
    spikes.clear();

    if (!playerImg) playerImg = new Image(L"assets\\Player.png");
    if (!blockImg) blockImg = new Image(L"assets\\block.png");
    if (!keyImg) keyImg = new Image(L"assets\\key.png");    // Загружаем ключ
    if (!lockImg) lockImg = new Image(L"assets\\Lock.png");  // Загружаем замок

    if (!g_hudFont) {
        g_hudFont = CreateFontW(24, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Monodetermination");
    }

    // 1. Стена слева (15 блоков)
    platforms.push_back({0, 400, 50, 750}); 

    // 2. Потолок (5 блоков)
    platforms.push_back({50, 400, 250, 50});

    // 3. Пол 1 (10 блоков)
    platforms.push_back({50, 650, 500, 750});

    // 4. Пол 2 (4 блока)
    platforms.push_back({650, 650, 200, 750});

    // 5. Пол 3 (ПРОДЛЕН ДО 18 БЛОКОВ = 900px)
    // Начинается с 950, заканчивается на 1850
    platforms.push_back({950, 650, 900, 750});

    // === ЛЕСЕНКА В СТИЛЕ GEOMETRY DASH ===
    platforms.push_back({1100, 600, 50, 52}); 
    platforms.push_back({1200, 550, 50, 102}); 
    platforms.push_back({1300, 500, 100, 302}); 

    // 11. ТОНКАЯ ПЛАТФОРМА (8 блоков в длину, 1 в высоту)
    platforms.push_back({1397, 500, 403, 52}); 

    // === ШИПЫ НА ПЛАТФОРМЕ ===
    spikes.push_back({1500, 475, 50, 25, false}); // 3-й блок (2 шт)
    spikes.push_back({1550, 475, 50, 25, false}); // 4-й блок (2 шт)
    spikes.push_back({1775, 475, 25, 25, false}); // 8-й блок (1 шт)

    // === ШИПЫ ВНИЗУ (ПОД ПЛАТФОРМОЙ) ===
    spikes.push_back({1500, 625, 25, 25, false}); // Под 3-м блоком
    spikes.push_back({1650, 625, 25, 25, false}); // Под 6-м блоком

    // === КЛЮЧ ===
    // Лежит внизу под первым блоком платформы
    keyItem.x = 1400;
    keyItem.y = 600;
    keyItem.collected = false;

    // === ПРОПАСТЬ 2 БЛОКА (1850 - 1950) ===

    // === НОВЫЙ ОСТРОВ (6 блоков = 300px) ===
    platforms.push_back({1950, 650, 300, 750});

    // === ЗАМОК ===
    // Стоит на 1-м блоке нового острова (X=1950). Высота 3 блока (150px).
    lockDoor.x = 1950;
    lockDoor.y = 500;
    lockDoor.w = 50;
    lockDoor.h = 150;

    RespawnPlayer();
}

void UpdateLevel1() {
    if (isFallingToDeath) {
        deathTimer += 1.0f;
        if (deathTimer > 30.0f) { 
            g_state = STATE_GAME_OVER;
            g_gameOverTimer = GetTickCount();
        }
        return; 
    }

    float moveSpeed = 10.0f; 

    if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000) player.vx = -moveSpeed;
    else if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000) player.vx = moveSpeed;
    else player.vx = 0;

    if ((GetAsyncKeyState(VK_SPACE) & 0x8000 || GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000) && player.onGround) {
        player.vy = -15.0f; 
        player.onGround = false;
    }

    player.vy += 1.5f; 
    player.x += player.vx;

    // Коллизия по X (платформы)
    for (const auto& p : platforms) {
        if (player.x < p.x + p.w && player.x + player.size > p.x &&
            player.y < p.y + p.h && player.y + player.size > p.y) {
            if (player.vx > 0) player.x = p.x - player.size;
            if (player.vx < 0) player.x = p.x + p.w;
        }
    }

    // Коллизия по X (Замок)
    if (!hasKey) {
        if (player.x < lockDoor.x + lockDoor.w && player.x + player.size > lockDoor.x &&
            player.y < lockDoor.y + lockDoor.h && player.y + player.size > lockDoor.y) {
            if (player.vx > 0) player.x = lockDoor.x - player.size;
            if (player.vx < 0) player.x = lockDoor.x + lockDoor.w;
        }
    }

    if (player.x < 0) player.x = 0;

    player.y += player.vy;
    player.onGround = false;

    // Коллизия по Y (платформы)
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

    // Коллизия по Y (Замок)
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

    // ПОДБОР КЛЮЧА
    if (!keyItem.collected) {
        if (player.x < keyItem.x + 50 && player.x + player.size > keyItem.x &&
            player.y < keyItem.y + 50 && player.y + player.size > keyItem.y) {
            keyItem.collected = true;
            hasKey = true;
        }
    }

    // Падение в пропасть
    if (player.y > 800) {
        isFallingToDeath = true; 
        deathStartTime = GetTickCount();
    }

    // Шипы (Мгновенный респавн)
    for (const auto& s : spikes) {
        float hitboxW = s.w - 10; 
        float hitboxH = 10;       
        float hitboxX = s.x + 5;
        float hitboxY = s.pointingDown ? s.y : s.y + s.h - hitboxH;

        if (player.x < hitboxX + hitboxW && player.x + player.size > hitboxX &&
            player.y < hitboxY + hitboxH && player.y + player.size > hitboxY) {
            RespawnPlayer(); 
            break;
        }
    }

    if (!isFallingToDeath) {
        float targetCamX = player.x + player.size / 2 - 400; 
        float targetCamY = player.y + player.size / 2 - 475; 
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
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
    
    SolidBrush bg(Color(255, 255, 255, 255));
    graphics.FillRectangle(&bg, 0, 0, 800, 600);

    // Отрисовка платформ
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

    // Отрисовка шипов
    SolidBrush spikeBrush(Color(255, 200, 50, 50)); 
    for (const auto& s : spikes) {
        if (s.x + s.w > cameraX && s.x < cameraX + 800 && s.y + s.h > cameraY && s.y < cameraY + 600) {
            int spikeCount = (int)(s.w / 25); 
            for (int i = 0; i < spikeCount; i++) {
                PointF points[3];
                float drawX = s.x - cameraX + (i * 25);
                float drawY = s.y - cameraY;

                if (s.pointingDown) { 
                    points[0] = PointF(drawX, drawY);
                    points[1] = PointF(drawX + 25, drawY);
                    points[2] = PointF(drawX + 12.5f, drawY + 25);
                } else { 
                    points[0] = PointF(drawX, drawY + 25);
                    points[1] = PointF(drawX + 25, drawY + 25);
                    points[2] = PointF(drawX + 12.5f, drawY);
                }
                graphics.FillPolygon(&spikeBrush, points, 3);
            }
        }
    }

    // ОТРИСОВКА КЛЮЧА
    if (!keyItem.collected) {
        if (keyItem.x + 50 > cameraX && keyItem.x < cameraX + 800 && keyItem.y + 50 > cameraY && keyItem.y < cameraY + 600) {
            if (keyImg && keyImg->GetLastStatus() == Ok) {
                graphics.DrawImage(keyImg, (INT)(keyItem.x - cameraX), (INT)(keyItem.y - cameraY), 50, 50);
            } else {
                // Заглушка, если картинки нет
                SolidBrush keyBrush(Color(255, 255, 215, 0)); 
                graphics.FillEllipse(&keyBrush, (INT)(keyItem.x - cameraX + 15), (INT)(keyItem.y - cameraY + 15), 20, 20);
                graphics.FillRectangle(&keyBrush, (INT)(keyItem.x - cameraX + 25), (INT)(keyItem.y - cameraY + 22), 20, 6);
            }
        }
    }

    // ОТРИСОВКА ЗАМКА
    if (!hasKey) {
        if (lockDoor.x + lockDoor.w > cameraX && lockDoor.x < cameraX + 800 && lockDoor.y + lockDoor.h > cameraY && lockDoor.y < cameraY + 600) {
            if (lockImg && lockImg->GetLastStatus() == Ok) {
                graphics.DrawImage(lockImg, (INT)(lockDoor.x - cameraX), (INT)(lockDoor.y - cameraY), (INT)lockDoor.w, (INT)lockDoor.h);
            } else {
                // Заглушка, если картинки нет
                SolidBrush lockBrush(Color(255, 80, 80, 80)); 
                graphics.FillRectangle(&lockBrush, (INT)(lockDoor.x - cameraX), (INT)(lockDoor.y - cameraY), (INT)lockDoor.w, (INT)lockDoor.h);
                SolidBrush holeBrush(Color(255, 0, 0, 0));
                graphics.FillEllipse(&holeBrush, (INT)(lockDoor.x - cameraX + 15), (INT)(lockDoor.y - cameraY + 50), 20, 20);
            }
        }
    }

    // Отрисовка игрока
    if (playerImg && playerImg->GetLastStatus() == Ok) {
        graphics.DrawImage(playerImg, (INT)(player.x - cameraX), (INT)(player.y - cameraY), player.size, player.size);
    } else {
        SolidBrush playerBrush(Color(255, 100, 100, 100)); 
        graphics.FillRectangle(&playerBrush, (INT)(player.x - cameraX), (INT)(player.y - cameraY), player.size, player.size);
    }

    // Отрисовка HUD
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0)); 
    HFONT oldFont = (HFONT)SelectObject(hdc, g_hudFont);

    TextOutW(hdc, 20, 20, L"LEVEL 1", 7);

    DWORD displayTime = isFallingToDeath ? (deathStartTime - g_levelStartTime) : (GetTickCount() - g_levelStartTime);
    int ms = displayTime % 1000;
    int totalSec = displayTime / 1000;
    int sec = totalSec % 60;
    int min = totalSec / 60;
    
    wchar_t timeStr[32];
    swprintf_s(timeStr, L"%02d:%02d:%03d", min, sec, ms);
    TextOutW(hdc, 20, 50, timeStr, wcslen(timeStr));

    if (g_showFps) {
        wchar_t fpsStr[32];
        swprintf_s(fpsStr, L"FPS: %d", g_fps);
        SetTextColor(hdc, RGB(100, 100, 100));
        TextOutW(hdc, 20, 80, fpsStr, wcslen(fpsStr));
        SetTextColor(hdc, RGB(0, 0, 0));
    }

    SolidBrush pauseBtnBg(Color(255, 230, 230, 230));
    graphics.FillRectangle(&pauseBtnBg, 750, 10, 40, 40);
    Pen borderPen(Color(255, 0, 0, 0), 2);
    graphics.DrawRectangle(&borderPen, 750, 10, 40, 40);

    SolidBrush pauseIcon(Color(255, 0, 0, 0));
    graphics.FillRectangle(&pauseIcon, 760, 20, 6, 20);
    graphics.FillRectangle(&pauseIcon, 774, 20, 6, 20);

    SelectObject(hdc, oldFont);
}

void RenderGameOver(HDC hdc) {
    if (GetTickCount() - g_gameOverTimer > 3000) {
        InitLevel1();
        g_state = STATE_PLAYING;
        g_levelStartTime = GetTickCount(); 
        return;
    }

    Graphics graphics(hdc);
    SolidBrush bg(Color(255, 255, 255, 255)); 
    graphics.FillRectangle(&bg, 0, 0, 800, 600);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));
    
    HFONT fontBig = CreateFontW(64, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Monodetermination");
    HFONT oldFont = (HFONT)SelectObject(hdc, fontBig);

    RECT textRect = { 0, 0, 800, 600 };
    DrawTextW(hdc, L"GAME OVER!", -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    
    SelectObject(hdc, oldFont);
    DeleteObject(fontBig);
}

void RenderComplete(HDC hdc) {
    Graphics graphics(hdc);
    SolidBrush bg(Color(255, 255, 255, 255)); 
    graphics.FillRectangle(&bg, 0, 0, 800, 600);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));
    HFONT oldFont = (HFONT)SelectObject(hdc, g_hudFont);

    RECT textRect = { 0, 0, 800, 600 };
    DrawTextW(hdc, L"COMPLETE!", -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    
    SelectObject(hdc, oldFont);
}
