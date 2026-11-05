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

float finishZoneX = 50, finishZoneY = 150, finishZoneW = 100, finishZoneH = 100;
bool isFallingToDeath = false;
float deathTimer = 0;

Image* playerImg = nullptr;
Image* blockImg = nullptr;

void RespawnPlayer() {
    player.x = 100;
    player.y = 850; 
    player.vx = 0;
    player.vy = 0;
    cameraX = 0;
    cameraY = 400; 
    isFallingToDeath = false;
    deathTimer = 0;
}

void InitLevel1() {
    platforms.clear();
    spikes.clear();

    if (!playerImg) playerImg = new Image(L"assets\\Player.png");
    if (!blockImg) blockImg = new Image(L"assets\\block.png");

    platforms.push_back({0, 900, 400, 100});
    for (int i = 0; i < 4; i++) spikes.push_back({200.0f + i * 40, 250, 40, 20, true});
    
    platforms.push_back({500, 750, 100, 30});
    platforms.push_back({650, 600, 100, 30});
    platforms.push_back({800, 750, 100, 30}); 

    platforms.push_back({1000, 900, 400, 100}); 
    for (int i = 0; i < 3; i++) spikes.push_back({1100.0f + i * 40, 880, 40, 20, false});

    platforms.push_back({1500, 900, 1000, 100}); 

    platforms.push_back({1300, 600, 300, 30}); 
    for (int i = 0; i < 4; i++) spikes.push_back({1350.0f + i * 40, 630, 40, 20, true}); 
    platforms.push_back({1350, 750, 200, 30}); 

    platforms.push_back({1900, 800, 100, 100});
    platforms.push_back({2000, 700, 100, 200});
    platforms.push_back({2100, 600, 100, 300});
    platforms.push_back({2200, 500, 100, 400}); 

    platforms.push_back({500, 200, 1700, 50});
    for (int i = 0; i < 5; i++) spikes.push_back({1000.0f + i * 40, 250, 40, 20, true});
    for (int i = 0; i < 4; i++) spikes.push_back({1500.0f + i * 40, 250, 40, 20, true});

    platforms.push_back({800, 350, 100, 30});
    platforms.push_back({600, 450, 100, 30});
    platforms.push_back({400, 350, 100, 30});
    platforms.push_back({200, 250, 100, 30}); 

    platforms.push_back({0, 50, 500, 50});

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

    if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000) player.vx = -6;
    else if (GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000) player.vx = 6;
    else player.vx = 0;

    if ((GetAsyncKeyState(VK_SPACE) & 0x8000 || GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000) && player.onGround) {
        player.vy = -17;
        player.onGround = false;
    }

    player.vy += 1.2f; 
    player.x += player.vx;

    for (const auto& p : platforms) {
        if (player.x < p.x + p.w && player.x + player.size > p.x &&
            player.y < p.y + p.h && player.y + player.size > p.y) {
            if (player.vx > 0) player.x = p.x - player.size;
            if (player.vx < 0) player.x = p.x + p.w;
        }
    }

    if (player.x < 0) player.x = 0;
    if (player.x + player.size > LEVEL_WIDTH) player.x = LEVEL_WIDTH - player.size;

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

    if (player.x < finishZoneX + finishZoneW && player.x + player.size > finishZoneX &&
        player.y < finishZoneY + finishZoneH && player.y + player.size > finishZoneY) {
        g_state = STATE_COMPLETE;
    }

    if (player.y > LEVEL_HEIGHT) {
        isFallingToDeath = true; 
    }

    for (const auto& s : spikes) {
        if (player.x < s.x + s.w && player.x + player.size > s.x &&
            player.y < s.y + s.h && player.y + player.size > s.y) {
            RespawnPlayer(); 
            break;
        }
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
    
    SolidBrush bg(Color(255, 255, 255, 255));
    graphics.FillRectangle(&bg, 0, 0, 800, 600);

    for (const auto& p : platforms) {
        if (p.x + p.w > cameraX && p.x < cameraX + 800 && p.y + p.h > cameraY && p.y < cameraY + 600) {
            if (blockImg && blockImg->GetLastStatus() == Ok) {
                graphics.DrawImage(blockImg, (INT)(p.x - cameraX), (INT)(p.y - cameraY), (INT)p.w, (INT)p.h);
            } else {
                SolidBrush platBrush(Color(255, 0, 0, 0));
                graphics.FillRectangle(&platBrush, (REAL)(p.x - cameraX), (REAL)(p.y - cameraY), (REAL)p.w, (REAL)p.h);
            }
        }
    }

    SolidBrush spikeBrush(Color(255, 200, 50, 50)); 
    for (const auto& s : spikes) {
        if (s.x + s.w > cameraX && s.x < cameraX + 800 && s.y + s.h > cameraY && s.y < cameraY + 600) {
            PointF points[3];
            float drawX = s.x - cameraX;
            float drawY = s.y - cameraY;

            if (s.pointingDown) { 
                points[0] = PointF(drawX, drawY);
                points[1] = PointF(drawX + s.w, drawY);
                points[2] = PointF(drawX + s.w / 2, drawY + s.h);
            } else { 
                points[0] = PointF(drawX, drawY + s.h);
                points[1] = PointF(drawX + s.w, drawY + s.h);
                points[2] = PointF(drawX + s.w / 2, drawY);
            }
            graphics.FillPolygon(&spikeBrush, points, 3);
        }
    }

    SolidBrush finishBrush(Color(255, 0, 0, 0));
    FontFamily fontFamily(L"Arial");
    Gdiplus::Font font(&fontFamily, 40, FontStyleBold);
    graphics.DrawString(L"<<", -1, &font, PointF(finishZoneX - cameraX, finishZoneY - cameraY), &finishBrush);

    if (playerImg && playerImg->GetLastStatus() == Ok) {
        graphics.DrawImage(playerImg, (INT)(player.x - cameraX), (INT)(player.y - cameraY), player.size, player.size);
    } else {
        SolidBrush playerBrush(Color(255, 100, 100, 100)); 
        graphics.FillRectangle(&playerBrush, (REAL)(player.x - cameraX), (REAL)(player.y - cameraY), (REAL)player.size, (REAL)player.size);
    }
}

void RenderGameOver(HDC hdc) {
    Graphics graphics(hdc);
    SolidBrush bg(Color(255, 255, 255, 255)); 
    graphics.FillRectangle(&bg, 0, 0, 800, 600);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));
    HFONT fontBig = CreateFontW(48, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Monodetermination");
    HFONT oldFont = (HFONT)SelectObject(hdc, fontBig);

    RECT textRect = { 0, 0, 800, 600 };
    // (\u041d\u0430\u0436\u043c\u0438 \u043c\u044b\u0448\u043a\u043e\u0439, \u0447\u0442\u043e\u0431\u044b \u0432\u044b\u0439\u0442\u0438 \u0432 \u043c\u0435\u043d\u044e) = (Нажми мышкой, чтобы выйти в меню)
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
    HFONT fontBig = CreateFontW(48, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Monodetermination");
    HFONT oldFont = (HFONT)SelectObject(hdc, fontBig);

    RECT textRect = { 0, 0, 800, 600 };
    DrawTextW(hdc, L"COMPLETE\n\n(\u041d\u0430\u0436\u043c\u0438 \u043c\u044b\u0448\u043a\u043e\u0439, \u0447\u0442\u043e\u0431\u044b \u0432\u044b\u0439\u0442\u0438 \u0432 \u043c\u0435\u043d\u044e)", -1, &textRect, DT_CENTER | DT_VCENTER | DT_WORDBREAK);
    
    SelectObject(hdc, oldFont);
    DeleteObject(fontBig);
}
