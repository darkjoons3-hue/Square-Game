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
Image* lockImg = nullptr;  

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
    if (!lockImg) lockImg = new Image(L"assets\\Lock.png");  

    if (!g_hudFont) {
        g_hudFont = CreateFontW(24, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Monodetermination");
    }

    platforms.push_back({0, 400, 50, 750}); 
    platforms.push_back({50, 400, 250, 50});
    platforms.push_back({50, 650, 500, 750});
    platforms.push_back({650, 650, 200, 750});
    platforms.push_back({950, 650, 900, 750});

    // Лесенка
    platforms.push_back({1100, 600, 50, 52}); 
    platforms.push_back({1200, 550, 50, 102}); 
    platforms.push_back({1300, 500, 100, 302}); 

    // Тонкая платформа
    platforms.push_back({1397, 500, 403, 52}); 

    // Шипы на платформе
    spikes.push_back({1500, 475, 50, 25, false}); 
    spikes.push_back({1550, 475, 50, 25, false}); 

    // Шипы внизу
    spikes.push_back({1500, 625, 25, 25, false}); 

    // Ключ
    keyItem.x = 1400;
    keyItem.y = 600;
    keyItem.collected = false;

    // Новый остров
    platforms.push_back({1950, 650, 300, 750});

    // Замок
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

    for (const auto& p : platforms) {
        if (player.x < p.x + p.w && player.x + player.size > p.x &&
            player.y < p.y + p.h && player.y + player.size > p.y) {
            if (player.vx > 0) player.x = p.x - player.size;
            if (player.vx < 0) player.x = p.x + p.w;
        }
    }

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

    if (!keyItem.collected) {
        if (player.x < keyItem.x + 50 && player.x + player.size > keyItem.x &&
            player.y < keyItem.y + 50 && player.y + player.size > keyItem.y) {
            keyItem.collected = true;
            hasKey = true;
        }
    }

    if (player.y > 800) {
        isFallingToDeath = true; 
        deathStartTime = GetTickCount();
    }

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
        cameraX += (targetCamX - cameraX) * 0.1f;

        float playerScreenY = player.y - cameraY;
        float targetCamY = cameraY; 

        if (playerScreenY < 200) {
            targetCamY = player.y - 200;
        } else if (playerScreenY + player.size > 450) {
            targetCamY = player.y + player.size - 450;
        }
        
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

    // КЛЮЧ (Черный с белой обводкой и анимацией)
    if (!keyItem.collected) {
        if (keyItem.x + 50 > cameraX && keyItem.x < cameraX + 800 && keyItem.y + 50 > cameraY && keyItem.y < cameraY + 600) {
            float bobOffset = sinf(GetTickCount() / 200.0f) * 5.0f;
            float drawX = keyItem.x - cameraX;
            float drawY = keyItem.y - cameraY + bobOffset;

            // Рисуем сам ключ (черный)
            SolidBrush keyBrush(Color(255, 0, 0, 0)); 
            graphics.FillEllipse(&keyBrush, (INT)(drawX + 10), (INT)(drawY + 10), 20, 20);
            graphics.FillRectangle(&keyBrush, (INT)(drawX + 20), (INT)(drawY + 17), 25, 6);
            graphics.FillRectangle(&keyBrush, (INT)(drawX + 35), (INT)(drawY + 23), 4, 8);
            graphics.FillRectangle(&keyBrush, (INT)(drawX + 42), (INT)(drawY + 23), 4, 6);

            // Белая пульсирующая обводка
            int alpha = 150 + (int)(100 * sinf(GetTickCount() / 200.0f));
            Pen whitePen(Color(alpha, 255, 255, 255), 3);
            graphics.DrawRectangle(&whitePen, (INT)drawX, (INT)drawY, 50, 50);
        }
    }

    if (!hasKey) {
        if (lockDoor.x + lockDoor.w > cameraX && lockDoor.x < cameraX + 800 && lockDoor.y + lockDoor.h > cameraY && lockDoor.y < cameraY + 600) {
            if (lockImg && lockImg->GetLastStatus() == Ok) {
                graphics.DrawImage(lockImg, (INT)(lockDoor.x - cameraX), (INT)(lockDoor.y - cameraY), (INT)lockDoor.w, (INT)lockDoor.h);
            } else {
                SolidBrush lockBrush(Color(255, 80, 80, 80)); 
                graphics.FillRectangle(&lockBrush, (INT)(lockDoor.x - cameraX), (INT)(lockDoor.y - cameraY), (INT)lockDoor.w, (INT)lockDoor.h);
                SolidBrush holeBrush(Color(255, 0, 0, 0));
                graphics.FillEllipse(&holeBrush, (INT)(lockDoor.x - cameraX + 15), (INT)(lockDoor.y - cameraY + 50), 20, 20);
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
