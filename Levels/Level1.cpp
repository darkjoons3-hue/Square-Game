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
DWORD deathStartTime = 0; 

Image* playerImg = nullptr;
Image* blockImg = nullptr;

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
}

void InitLevel1() {
    platforms.clear();
    spikes.clear();

    if (!playerImg) playerImg = new Image(L"assets\\Player.png");
    if (!blockImg) blockImg = new Image(L"assets\\block.png");

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

    // 5. Пол 3 (7 блоков) -> ОСТРОВ (ПРОДЛЕН)
    platforms.push_back({950, 650, 450, 750});

    // === ЛЕСЕНКА В СТИЛЕ GEOMETRY DASH ===
    
    // 8. Первый бугор (добавляем 2 пикселя в высоту для нахлеста)
    platforms.push_back({1100, 600, 50, 52}); 

    // 9. Второй бугор (высота 2 блока)
    platforms.push_back({1200, 550, 50, 102}); 

    // 10. Высокая стена (ширина 2 блока, высота 6 блоков + 2 пикселя)
    platforms.push_back({1300, 500, 100, 302}); 

    // 11. НОВАЯ ПЛАТФОРМА (8 блоков в длину, 2 в высоту), крепится к верхушке стены
    // Начинается на X=1400, заканчивается на 1800. Y=500.
    platforms.push_back({1400, 500, 400, 102}); 

    // === ШИПЫ ===
    // 12. Шип на 7-м блоке (X=1700)
    spikes.push_back({1700, 475, 50, 25, false});

    // 13. Шип на позиции 7.5, прямо у края (X=1775)
    spikes.push_back({1775, 475, 50, 25, false});

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

    // Падение в пропасть (вызывает Game Over)
    if (player.y > 800) {
        isFallingToDeath = true; 
        deathStartTime = GetTickCount();
    }

    // === ЧЕСТНЫЙ ХИТБОКС ШИПОВ (Телепорт без Game Over) ===
    for (const auto& s : spikes) {
        float hitboxW = s.w - 10; 
        float hitboxH = 10;       
        float hitboxX = s.x + 5;
        float hitboxY = s.pointingDown ? s.y : s.y + s.h - hitboxH;

        if (player.x < hitboxX + hitboxW && player.x + player.size > hitboxX &&
            player.y < hitboxY + hitboxH && player.y + player.size > hitboxY) {
            RespawnPlayer(); // Мгновенный телепорт на старт
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

    // ОТРИСОВКА ШИПОВ
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

    if (playerImg && playerImg->GetLastStatus() == Ok) {
        graphics.DrawImage(playerImg, (INT)(player.x - cameraX), (INT)(player.y - cameraY), player.size, player.size);
    } else {
        SolidBrush playerBrush(Color(255, 100, 100, 100)); 
        graphics.FillRectangle(&playerBrush, (INT)(player.x - cameraX), (INT)(player.y - cameraY), player.size, player.size);
    }

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
