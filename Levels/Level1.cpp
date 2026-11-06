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

bool isFallingToDeath = false;
float deathTimer = 0;

Image* playerImg = nullptr;
Image* blockImg = nullptr;

// ОПТИМИЗАЦИЯ: Создаем шрифт 1 раз и храним глобально
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
    g_levelStartTime = GetTickCount(); 
}

void InitLevel1() {
    platforms.clear();
    spikes.clear();

    if (!playerImg) playerImg = new Image(L"assets\\Player.png");
    if (!blockImg) blockImg = new Image(L"assets\\block.png");

    // Создаем шрифт для HUD только один раз
    if (!g_hudFont) {
        g_hudFont = CreateFontW(24, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Monodetermination");
    }

    // 1. Стена слева (ОГРОМНАЯ, чтобы закрыть пустоту внизу)
    platforms.push_back({0, 400, 50, 2000}); // Высота 2000 вместо 250

    // 2. Потолок (РОВНО 5 блоков = 250px)
    platforms.push_back({50, 400, 250, 50});

    // 3. Пол 1 (10 блоков = 500px, высота вниз 2000, чтобы был "остров")
    platforms.push_back({50, 650, 500, 2000});

    // 5. Пол 2 (4 блока = 200px, высота 2000)
    platforms.push_back({650, 650, 200, 2000});

    // 7. Пол 3 (7 блоков = 350px, высота 2000)
    platforms.push_back({950, 650, 350, 2000});

    RespawnPlayer();
}

void UpdateLevel1() {
    if (isFallingToDeath) {
        deathTimer += 1.0f;
        if (deathTimer > 180.0f) { 
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

    for (const auto& p : platforms) {
        if (player.x < p.x + p.w && player.x + player.size > p.x &&
            player.y < p.y + p.h && player.y + player.size > p.y) {
            if (player.vx > 0) player.x = p.x - player.size;
            if (player.vx < 0) player.x = p.x + p.w;
        }
    }

    if (player.x < 0) player.x = 0;

    player.y += player.vy;
    player.onGround = false;

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

    if (player.y > LEVEL_HEIGHT) {
        isFallingToDeath = true; 
    }

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
    
    // МАГИЯ: Отключаем сглаживание, чтобы не было ореолов у блоков!
    graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);
    
    SolidBrush bg(Color(255, 255, 255, 255));
    graphics.FillRectangle(&bg, 0, 0, 800, 600);

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

    if (playerImg && playerImg->GetLastStatus() == Ok) {
        graphics.DrawImage(playerImg, (INT)(player.x - cameraX), (INT)(player.y - cameraY), player.size, player.size);
    } else {
        SolidBrush playerBrush(Color(255, 100, 100, 100)); 
        graphics.FillRectangle(&playerBrush, (INT)(player.x - cameraX), (INT)(player.y - cameraY), player.size, player.size);
    }

    // --- ОТРИСОВКА HUD ---
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0)); 
    
    HFONT oldFont = (HFONT)SelectObject(hdc, g_hudFont);

    TextOutW(hdc, 20, 20, L"LEVEL 1", 7);

    DWORD elapsed = GetTickCount() - g_levelStartTime;
    int ms = elapsed % 1000;
    int totalSec = elapsed / 1000;
    int sec = totalSec % 60;
    int min = totalSec / 60;
    
    wchar_t timeStr[32];
    swprintf_s(timeStr, L"%02d:%02d:%03d", min, sec, ms);
    TextOutW(hdc, 20, 50, timeStr, wcslen(timeStr));

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
        return;
    }

    Graphics graphics(hdc);
    SolidBrush bg(Color(255, 255, 255, 255)); 
    graphics.FillRectangle(&bg, 0, 0, 800, 600);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));
    HFONT oldFont = (HFONT)SelectObject(hdc, g_hudFont);

    RECT textRect = { 0, 0, 800, 600 };
    DrawTextW(hdc, L"GAME OVER!", -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    
    SelectObject(hdc, oldFont);
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
