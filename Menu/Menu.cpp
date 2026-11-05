#pragma execution_character_set("utf-8")
#include "Menu.h"
#include <cmath>

using namespace Gdiplus;

float g_disclaimerAlpha = 0.0f;
float g_menuAlpha = 0.0f;
POINT g_mousePos = { 400, 300 };
POINT g_lastMousePos = { -1, -1 };
float g_cursorAngle = 0.0f;

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
    }
}

void RenderMenu(HDC hdc) {
    Graphics graphics(hdc);

    HFONT fontBig = CreateFontW(32, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Monodetermination");
    HFONT fontNormal = CreateFontW(20, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Monodetermination");
    HFONT fontSmall = CreateFontW(16, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Monodetermination");
    HFONT fontTiny = CreateFontW(14, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Monodetermination");

    if (g_state == STATE_DISCLAIMER) {
        SolidBrush bg(Color(255, 15, 15, 15));
        graphics.FillRectangle(&bg, 0, 0, 800, 600);

        SetBkMode(hdc, TRANSPARENT);
        int colorVal = (int)(200 * g_disclaimerAlpha);
        SetTextColor(hdc, RGB(colorVal, colorVal, colorVal)); 

        HFONT oldFont = (HFONT)SelectObject(hdc, fontNormal);
        
        RECT textRect = { 50, 100, 750, 450 };
        // ПОЛНЫЙ ТЕКСТ В ЮНИКОДЕ (чтобы не было кракозябр)
        const wchar_t* disclaimer = 
            L"\u0412\u041d\u0418\u041c\u0410\u041d\u0418\u0415\n\n"
            L"\u0414\u0430\u043d\u043d\u044b\u0439 \u043f\u0440\u043e\u0433\u0440\u0430\u043c\u043c\u043d\u044b\u0439 \u043f\u0440\u043e\u0434\u0443\u043a\u0442 \u043d\u0435 \u044f\u0432\u043b\u044f\u0435\u0442\u0441\u044f \u0432\u0438\u0440\u0443\u0441\u043e\u043c\n"
            L"\u0438 \u043d\u0435 \u043d\u0430\u043d\u0435\u0441\u0435\u0442 \u0432\u0440\u0435\u0434\u0430 \u0432\u0430\u0448\u0435\u0439 \u0441\u0438\u0441\u0442\u0435\u043c\u0435.\n\n"
            L"\u041e\u0434\u043d\u0430\u043a\u043e \u0438\u0433\u0440\u0430 \u0441\u043e\u0434\u0435\u0440\u0436\u0438\u0442 \u044d\u043b\u0435\u043c\u0435\u043d\u0442\u044b \u043f\u0441\u0438\u0445\u043e\u043b\u043e\u0433\u0438\u0447\u0435\u0441\u043a\u043e\u0433\u043e \u0443\u0436\u0430\u0441\u0430,\n"
            L"\u0440\u0435\u0437\u043a\u0438\u0435 \u0437\u0432\u0443\u043a\u0438, \u043c\u0435\u0440\u0446\u0430\u043d\u0438\u044f \u044d\u043a\u0440\u0430\u043d\u0430 \u0438 \u0432\u0437\u0430\u0438\u043c\u043e\u0434\u0435\u0439\u0441\u0442\u0432\u0438\u0435\n"
            L"\u0441 \u0432\u0430\u0448\u0438\u043c \u0440\u0430\u0431\u043e\u0447\u0438\u043c \u0441\u0442\u043e\u043b\u043e\u043c.\n\n"
            L"\u0415\u0441\u043b\u0438 \u0443 \u0432\u0430\u0441 \u0441\u043b\u0430\u0431\u0430\u044f \u043d\u0435\u0440\u0432\u043d\u0430\u044f \u0441\u0438\u0441\u0442\u0435\u043c\u0430 \u0438\u043b\u0438 \u043f\u0440\u043e\u0431\u043b\u0435\u043c\u044b \u0441 \u0441\u0435\u0440\u0434\u0446\u0435\u043c,\n"
            L"\u043d\u0435\u043c\u0435\u0434\u043b\u0435\u043d\u043d\u043e \u0437\u0430\u043a\u0440\u043e\u0439\u0442\u0435 \u043f\u0440\u0438\u043b\u043e\u0436\u0435\u043d\u0438\u0435.\n\n"
            L"\u041d\u0430\u0436\u0438\u043c\u0430\u044f \u00ab\u042f \u0421\u041e\u0413\u041b\u0410\u0421\u0415\u041d\u00bb, \u0432\u044b \u043f\u043e\u0434\u0442\u0432\u0435\u0440\u0436\u0434\u0430\u0435\u0442\u0435,\n"
            L"\u0447\u0442\u043e \u0433\u043e\u0442\u043e\u0432\u044b \u043a \u043f\u043e\u043b\u043d\u043e\u043c\u0443 \u043f\u043e\u0433\u0440\u0443\u0436\u0435\u043d\u0438\u044e.";

        DrawTextW(hdc, disclaimer, -1, &textRect, DT_CENTER | DT_TOP | DT_WORDBREAK);

        if (g_disclaimerAlpha > 0.5f) {
            RECT btnRect = { 300, 500, 500, 550 };
            bool hover = (g_mousePos.x >= btnRect.left && g_mousePos.x <= btnRect.right && g_mousePos.y >= btnRect.top && g_mousePos.y <= btnRect.bottom);
            SolidBrush btnBrush(hover ? Color(255, 50, 50, 50) : Color(255, 30, 30, 30));
            graphics.FillRectangle(&btnBrush, btnRect.left, btnRect.top, 200, 50);
            
            SetTextColor(hdc, RGB(255, 255, 255));
            SelectObject(hdc, fontSmall);
            DrawTextW(hdc, L"\u042f \u0421\u041e\u0413\u041b\u0410\u0421\u0415\u041d", -1, &btnRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
        SelectObject(hdc, oldFont);
    } 
    else if (g_state == STATE_MENU || g_state == STATE_OPTIONS) {
        SolidBrush bg(Color(255, 255, 255, 255)); 
        graphics.FillRectangle(&bg, 0, 0, 800, 600);

        SolidBrush logoBrush(Color(255, 0, 0, 0));
        graphics.FillRectangle(&logoBrush, 325, 150, 150, 150);

        SetBkMode(hdc, TRANSPARENT);
        const wchar_t* buttons[] = { L"PLAY", L"ONITION", L"EXIT" };
        for (int i = 0; i < 3; i++) {
            RECT btnRect = { 300, 350 + (i * 50), 500, 390 + (i * 50) };
            bool hover = (g_mousePos.x >= btnRect.left && g_mousePos.x <= btnRect.right && g_mousePos.y >= btnRect.top && g_mousePos.y <= btnRect.bottom);
            
            if (hover && g_state == STATE_MENU) SetTextColor(hdc, RGB(0, 180, 0));
            else SetTextColor(hdc, RGB(0, 0, 0));

            SelectObject(hdc, fontBig);
            DrawTextW(hdc, buttons[i], -1, &btnRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }

        // Версия в углу
        SetTextColor(hdc, RGB(100, 100, 100));
        SelectObject(hdc, fontTiny);
        RECT verRect = { 650, 560, 780, 590 };
        DrawTextW(hdc, L"version 1.0", -1, &verRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

        if (g_state == STATE_OPTIONS) {
            SolidBrush darkOverlay(Color(150, 0, 0, 0));
            graphics.FillRectangle(&darkOverlay, 0, 0, 800, 600);

            RECT modalRect = { 200, 100, 600, 500 };
            SolidBrush modalBg(Color(255, 240, 240, 240));
            graphics.FillRectangle(&modalBg, modalRect.left, modalRect.top, 400, 400);
            
            Pen borderPen(Color(255, 0, 0, 0), 3);
            graphics.DrawRectangle(&borderPen, modalRect.left, modalRect.top, 400, 400);

            SetTextColor(hdc, RGB(0, 0, 0));
            SelectObject(hdc, fontBig);
            RECT titleRect = { 200, 120, 600, 170 };
            DrawTextW(hdc, L"OPTIONS", -1, &titleRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            SelectObject(hdc, fontNormal);
            RECT musicRect = { 250, 220, 400, 250 };
            DrawTextW(hdc, L"Music", -1, &musicRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            
            RECT musicBar = { 400, 225, 550, 245 };
            SolidBrush barBg(Color(255, 150, 150, 150));
            graphics.FillRectangle(&barBg, musicBar.left, musicBar.top, 150, 20);
            SolidBrush barFill(Color(255, 0, 180, 0));
            graphics.FillRectangle(&barFill, musicBar.left, musicBar.top, 80, 20);

            RECT soundRect = { 250, 300, 400, 330 };
            DrawTextW(hdc, L"Sound", -1, &soundRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            
            RECT soundBar = { 400, 305, 550, 325 };
            graphics.FillRectangle(&barBg, soundBar.left, soundBar.top, 150, 20);
            graphics.FillRectangle(&barFill, soundBar.left, soundBar.top, 120, 20); 

            RECT backBtn = { 500, 380, 580, 420 };
            bool hoverBack = (g_mousePos.x >= backBtn.left && g_mousePos.x <= backBtn.right && g_mousePos.y >= backBtn.top && g_mousePos.y <= backBtn.bottom);
            
            SolidBrush btnBrush(hoverBack ? Color(255, 200, 200, 200) : Color(255, 180, 180, 180));
            graphics.FillRectangle(&btnBrush, backBtn.left, backBtn.top, 80, 40);
            Pen btnBorder(Color(255, 0, 0, 0), 2);
            graphics.DrawRectangle(&btnBorder, backBtn.left, backBtn.top, 80, 40);

            SetTextColor(hdc, RGB(0, 0, 0));
            SelectObject(hdc, fontSmall);
            DrawTextW(hdc, L"BACK", -1, &backBtn, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
    }

    DeleteObject(fontBig);
    DeleteObject(fontNormal);
    DeleteObject(fontSmall);
    DeleteObject(fontTiny);

    DrawCustomCursor(hdc);
}
