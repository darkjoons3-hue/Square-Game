#pragma execution_character_set("utf-8")
#include "Menu.h"
#include <cmath>

using namespace Gdiplus;

float g_disclaimerAlpha = 0.0f;
float g_menuAlpha = 0.0f;
POINT g_mousePos = { 400, 300 };
POINT g_lastMousePos = { -1, -1 };
float g_cursorAngle = 0.0f;

// Глобальная коллекция для нашего шрифта
PrivateFontCollection g_pfc;
FontFamily* g_fontFamily = nullptr;

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

// Универсальная функция отрисовки текста НАШИМ шрифтом
void DrawTextCustom(HDC hdc, const wchar_t* text, RECT rc, int size, Color color, bool center = true) {
    Graphics graphics(hdc);
    // Этот параметр делает шрифт пиксельным и четким (без размытия)
    graphics.SetTextRenderingHint(TextRenderingHintSingleBitPerPixelGridFit);
    
    Gdiplus::Font* font;
    if (g_fontFamily && g_fontFamily->IsAvailable()) {
        font = new Gdiplus::Font(g_fontFamily, (REAL)size, FontStyleRegular, UnitPixel);
    } else {
        font = new Gdiplus::Font(L"Arial", (REAL)size, FontStyleRegular, UnitPixel);
    }
    
    SolidBrush brush(color);
    StringFormat format;
    
    if (center) {
        format.SetAlignment(StringAlignmentCenter);
        format.SetLineAlignment(StringAlignmentCenter);
    } else {
        format.SetAlignment(StringAlignmentNear);
        format.SetLineAlignment(StringAlignmentNear);
    }
    
    graphics.DrawString(text, -1, font, RectF((float)rc.left, (float)rc.top, (float)(rc.right - rc.left), (float)(rc.bottom - rc.top)), &format, &brush);
    delete font;
}

void InitMenu() {
    // Загружаем шрифт напрямую из файла
    if (g_pfc.GetFamilyCount() == 0) {
        g_pfc.AddFontFile(L"assets\\Monodetermination.ttf");
        
        INT numFound = 0;
        g_pfc.GetFamilyCount(&numFound);
        if (numFound > 0) {
            FontFamily* families = new FontFamily[numFound];
            g_pfc.GetFamilies(numFound, families, &numFound);
            if (numFound > 0) {
                WCHAR familyName[LF_FACESIZE];
                families[0].GetFamilyName(familyName);
                // Создаем FontFamily, привязанный к приватной коллекции
                g_fontFamily = new FontFamily(familyName, &g_pfc);
            }
            delete[] families;
        }
    }
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
    } 
    else if (g_state == STATE_MENU) {
        if (g_menuAlpha < 1.0f) g_menuAlpha += 0.02f;
    }
    else if (g_state == STATE_STORY) {
        if (g_charIndex < (int)g_currentText.length()) {
            if (GetTickCount() - g_typeTimer > 50) { 
                g_charIndex++;
                g_typeTimer = GetTickCount();
            }
        }
    }
}

void RenderMenu(HDC hdc) {
    Graphics graphics(hdc);

    if (g_state == STATE_DISCLAIMER) {
        SolidBrush bg(Color(255, 15, 15, 15));
        graphics.FillRectangle(&bg, 0, 0, 800, 600);

        RECT textRect = { 50, 100, 750, 450 };
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

        int alpha = (int)(255 * g_disclaimerAlpha);
        DrawTextCustom(hdc, disclaimer, textRect, 24, Color(alpha, 200, 200, 200));

        if (g_disclaimerAlpha > 0.5f) {
            RECT btnRect = { 300, 500, 500, 550 };
            bool hover = (g_mousePos.x >= btnRect.left && g_mousePos.x <= btnRect.right && g_mousePos.y >= btnRect.top && g_mousePos.y <= btnRect.bottom);
            SolidBrush btnBrush(hover ? Color(255, 50, 50, 50) : Color(255, 30, 30, 30));
            graphics.FillRectangle(&btnBrush, btnRect.left, btnRect.top, 200, 50);
            DrawTextCustom(hdc, L"\u042f \u0421\u041e\u0413\u041b\u0410\u0421\u0415\u041d", btnRect, 16, Color(255, 255, 255, 255));
        }
    } 
    else if (g_state == STATE_MENU || g_state == STATE_OPTIONS) {
        SolidBrush bg(Color(255, 255, 255, 255)); 
        graphics.FillRectangle(&bg, 0, 0, 800, 600);

        SolidBrush logoBrush(Color(255, 0, 0, 0));
        graphics.FillRectangle(&logoBrush, 360, 150, 80, 80);

        const wchar_t* buttons[] = { L"PLAY", L"ONITION", L"EXIT" };
        for (int i = 0; i < 3; i++) {
            RECT btnRect = { 300, 350 + (i * 50), 500, 390 + (i * 50) };
            bool hover = (g_mousePos.x >= btnRect.left && g_mousePos.x <= btnRect.right && g_mousePos.y >= btnRect.top && g_mousePos.y <= btnRect.bottom);
            
            Color textColor = (hover && g_state == STATE_MENU) ? Color(255, 0, 180, 0) : Color(255, 0, 0, 0);
            DrawTextCustom(hdc, buttons[i], btnRect, 32, textColor);
        }

        RECT verRect = { 650, 560, 780, 590 };
        DrawTextCustom(hdc, L"version 1.0", verRect, 14, Color(255, 100, 100, 100), false);

        if (g_state == STATE_OPTIONS) {
            SolidBrush darkOverlay(Color(150, 0, 0, 0));
            graphics.FillRectangle(&darkOverlay, 0, 0, 800, 600);

            RECT modalRect = { 200, 100, 600, 500 };
            SolidBrush modalBg(Color(255, 240, 240, 240));
            graphics.FillRectangle(&modalBg, modalRect.left, modalRect.top, 400, 400);
            
            Pen borderPen(Color(255, 0, 0, 0), 3);
            graphics.DrawRectangle(&borderPen, modalRect.left, modalRect.top, 400, 400);

            RECT titleRect = { 200, 120, 600, 170 };
            DrawTextCustom(hdc, L"OPTIONS", titleRect, 32, Color(255, 0, 0, 0));

            RECT musicRect = { 250, 220, 400, 250 };
            DrawTextCustom(hdc, L"Music", musicRect, 20, Color(255, 0, 0, 0), false);
            
            RECT musicBar = { 400, 225, 550, 245 };
            SolidBrush barBg(Color(255, 150, 150, 150));
            graphics.FillRectangle(&barBg, musicBar.left, musicBar.top, 150, 20);
            SolidBrush barFill(Color(255, 0, 180, 0));
            graphics.FillRectangle(&barFill, musicBar.left, musicBar.top, 80, 20);

            RECT soundRect = { 250, 300, 400, 330 };
            DrawTextCustom(hdc, L"Sound", soundRect, 20, Color(255, 0, 0, 0), false);
            
            RECT soundBar = { 400, 305, 550, 325 };
            graphics.FillRectangle(&barBg, soundBar.left, soundBar.top, 150, 20);
            graphics.FillRectangle(&barFill, soundBar.left, soundBar.top, 120, 20); 

            RECT backBtn = { 500, 380, 580, 420 };
            bool hoverBack = (g_mousePos.x >= backBtn.left && g_mousePos.x <= backBtn.right && g_mousePos.y >= backBtn.top && g_mousePos.y <= backBtn.bottom);
            
            SolidBrush btnBrush(hoverBack ? Color(255, 200, 200, 200) : Color(255, 180, 180, 180));
            graphics.FillRectangle(&btnBrush, backBtn.left, backBtn.top, 80, 40);
            Pen btnBorder(Color(255, 0, 0, 0), 2);
            graphics.DrawRectangle(&btnBorder, backBtn.left, backBtn.top, 80, 40);

            DrawTextCustom(hdc, L"BACK", backBtn, 16, Color(255, 0, 0, 0));
        }
    }
    else if (g_state == STATE_STORY) {
        SolidBrush bg(Color(255, 15, 15, 15)); 
        graphics.FillRectangle(&bg, 0, 0, 800, 600);

        if (g_charIndex > 0 && g_charIndex <= (int)g_currentText.length()) {
            std::wstring visibleText = g_currentText.substr(0, g_charIndex);
            RECT textRect = { 100, 100, 700, 450 };
            DrawTextCustom(hdc, visibleText.c_str(), textRect, 24, Color(255, 255, 255, 255));
        }

        if (g_charIndex >= (int)g_currentText.length()) {
            RECT hintRect = { 0, 500, 800, 550 };
            DrawTextCustom(hdc, L"[ \u041d\u0430\u0436\u043c\u0438\u0442\u0435 Z \u0438\u043b\u0438 \u043a\u043b\u0438\u043a\u043d\u0438\u0442\u0435, \u0447\u0442\u043e\u0431\u044b \u043f\u0440\u043e\u0434\u043e\u043b\u0436\u0438\u0442\u044c ]", hintRect, 14, Color(255, 100, 100, 100));
        }
    }

    DrawCustomCursor(hdc);
}
