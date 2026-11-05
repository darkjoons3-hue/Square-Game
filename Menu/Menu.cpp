#pragma execution_character_set("utf-8")
#include "Menu.h"
#include <cmath>

using namespace Gdiplus;

float g_disclaimerAlpha = 0.0f;
float g_menuAlpha = 0.0f;
POINT g_mousePos = { 400, 300 };
POINT g_lastMousePos = { -1, -1 };
float g_cursorAngle = 0.0f;
float g_glitchTimer = 0.0f;
RECT g_glitchRect = { 0, 0, 0, 0 };

void DrawCustomCursor(HDC hdc) {
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    float size = 15.0f;
    PointF p1(g_mousePos.x + cos(g_cursorAngle) * size, g_mousePos.y + sin(g_cursorAngle) * size);
    PointF p2(g_mousePos.x + cos(g_cursorAngle + 2.5f) * size, g_mousePos.y + sin(g_cursorAngle + 2.5f) * size);
    PointF p3(g_mousePos.x + cos(g_cursorAngle - 2.5f) * size, g_mousePos.y + sin(g_cursorAngle - 2.5f) * size);
    PointF points[3] = { p1, p2, p3 };
    SolidBrush fillBrush(Color(255, 0, 0, 0));
    graphics.FillPolygon(&fillBrush, points, 3);
    Pen outlinePen(Color(255, 255, 255, 255), 1.5f);
    graphics.DrawPolygon(&outlinePen, points, 3);
}

void InitMenu() {
    g_disclaimerAlpha = 0.0f;
    g_menuAlpha = 0.0f;
}

void UpdateMenu() {
    POINT currentMouse;
    GetCursorPos(&currentMouse);
    ScreenToClient(g_hWnd, &currentMouse);

    if (currentMouse.x != g_lastMousePos.x || currentMouse.y != g_lastMousePos.y) {
        g_cursorAngle = atan2((float)(currentMouse.y - g_lastMousePos.y), (float)(currentMouse.x - g_lastMousePos.x));
        g_lastMousePos = currentMouse;
        g_mousePos = currentMouse;
    }

    if (g_state == STATE_DISCLAIMER) {
        if (g_disclaimerAlpha < 1.0f) g_disclaimerAlpha += 0.01f;
    } else if (g_state == STATE_MENU) {
        if (g_menuAlpha < 1.0f) g_menuAlpha += 0.02f;
        
        g_glitchTimer += 1.0f;
        if (g_glitchTimer > 15.0f) {
            g_glitchTimer = 0;
            int boxX = (800 - 150) / 2;
            int boxY = 150;
            int gx = boxX + 10 + rand() % 130;
            int gy = boxY + 10 + rand() % 130;
            g_glitchRect = { gx, gy, gx + 8, gy + 8 };
        }
    }
}

void RenderMenu(HDC hdc) {
    Graphics graphics(hdc);

    HFONT fontBig = CreateFontW(32, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Monodetermination");
    HFONT fontNormal = CreateFontW(20, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Monodetermination");
    HFONT fontSmall = CreateFontW(16, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Monodetermination");

    if (g_state == STATE_DISCLAIMER) {
        SolidBrush bg(Color(255, 15, 15, 15));
        graphics.FillRectangle(&bg, 0, 0, 800, 600);

        SetBkMode(hdc, TRANSPARENT);
        int colorVal = (int)(200 * g_disclaimerAlpha);
        SetTextColor(hdc, RGB(colorVal, colorVal, colorVal)); 

        HFONT oldFont = (HFONT)SelectObject(hdc, fontNormal);
        
        RECT textRect = { 50, 100, 750, 450 };
        const wchar_t* disclaimer = 
            L"ВНИМАНИЕ\n\n"
            L"Данный программный продукт не является вирусом\nи не нанесет вреда вашей системе.\n\n"
            L"Однако игра содержит элементы психологического ужаса,\nрезкие звуки, мерцания экрана и взаимодействие\nс вашим рабочим столом.\n\n"
            L"Если у вас слабая нервная система или проблемы с сердцем,\nнемедленно закройте приложение.\n\n"
            L"Нажимая «Я СОГЛАСЕН», вы подтверждаете,\nчто готовы к полному погружению.";

        DrawTextW(hdc, disclaimer, -1, &textRect, DT_CENTER | DT_TOP | DT_WORDBREAK);

        if (g_disclaimerAlpha > 0.5f) {
            RECT btnRect = { 300, 500, 500, 550 };
            bool hover = (g_mousePos.x >= btnRect.left && g_mousePos.x <= btnRect.right && g_mousePos.y >= btnRect.top && g_mousePos.y <= btnRect.bottom);
            SolidBrush btnBrush(hover ? Color(255, 50, 50, 50) : Color(255, 30, 30, 30));
            graphics.FillRectangle(&btnBrush, btnRect.left, btnRect.top, 200, 50);
            
            SetTextColor(hdc, RGB(255, 255, 255));
            SelectObject(hdc, fontSmall);
            DrawTextW(hdc, L"Я СОГЛАСЕН", -1, &btnRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
        SelectObject(hdc, oldFont);
    } 
    else if (g_state == STATE_MENU) {
        SolidBrush bg(Color(255, 255, 255, 255)); 
        graphics.FillRectangle(&bg, 0, 0, 800, 600);

        SolidBrush logoBrush(Color(255, 0, 0, 0));
        graphics.FillRectangle(&logoBrush, 325, 150, 150, 150);

        if (g_glitchRect.right > 0) {
            SolidBrush glitchBrush(Color(255, 0, 255, 0));
            graphics.FillRectangle(&glitchBrush, g_glitchRect.left, g_glitchRect.top, 8, 8);
        }

        SetBkMode(hdc, TRANSPARENT);
        const wchar_t* buttons[] = { L"PLAY", L"ONITION", L"EXIT" };
        for (int i = 0; i < 3; i++) {
            RECT btnRect = { 300, 350 + (i * 50), 500, 390 + (i * 50) };
            bool hover = (g_mousePos.x >= btnRect.left && g_mousePos.x <= btnRect.right && g_mousePos.y >= btnRect.top && g_mousePos.y <= btnRect.bottom);
            
            if (hover) SetTextColor(hdc, RGB(0, 180, 0));
            else SetTextColor(hdc, RGB(0, 0, 0));

            SelectObject(hdc, fontBig);
            DrawTextW(hdc, buttons[i], -1, &btnRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
    }
    else if (g_state == STATE_OPTIONS) {
        SolidBrush bg(Color(255, 20, 20, 20));
        graphics.FillRectangle(&bg, 0, 0, 800, 600);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 255, 255));
        
        RECT titleRect = { 0, 50, 800, 100 };
        SelectObject(hdc, fontBig);
        DrawTextW(hdc, L"OPTIONS", -1, &titleRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        SelectObject(hdc, fontNormal);
        
        RECT musicRect = { 200, 200, 600, 250 };
        DrawTextW(hdc, L"Music", -1, &musicRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        RECT musicBar = { 400, 210, 600, 230 };
        SolidBrush barBg(Color(255, 50, 50, 50));
        graphics.FillRectangle(&barBg, musicBar.left, musicBar.top, 200, 20);
        SolidBrush barFill(Color(255, 0, 180, 0));
        graphics.FillRectangle(&barFill, musicBar.left, musicBar.top, 100, 20);

        RECT soundRect = { 200, 300, 600, 350 };
        DrawTextW(hdc, L"Sound", -1, &soundRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        RECT soundBar = { 400, 310, 600, 330 };
        graphics.FillRectangle(&barBg, soundBar.left, soundBar.top, 200, 20);
        graphics.FillRectangle(&barFill, soundBar.left, soundBar.top, 150, 20); 

        RECT backBtn = { 600, 500, 750, 550 };
        bool hoverBack = (g_mousePos.x >= backBtn.left && g_mousePos.x <= backBtn.right && g_mousePos.y >= backBtn.top && g_mousePos.y <= backBtn.bottom);
        SolidBrush btnBrush(hoverBack ? Color(255, 100, 100, 100) : Color(255, 50, 50, 50));
        graphics.FillRectangle(&btnBrush, backBtn.left, backBtn.top, 150, 50);
        
        SetTextColor(hdc, RGB(255, 255, 255));
        DrawTextW(hdc, L"BACK", -1, &backBtn, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    DeleteObject(fontBig);
    DeleteObject(fontNormal);
    DeleteObject(fontSmall);

    DrawCustomCursor(hdc);
}
