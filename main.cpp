#pragma execution_character_set("utf-8")
#define _WIN32_WINNT 0x0601
#define WIN32_LEAN_AND_MEAN

#include "Game.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#pragma comment(linker, "/ENTRY:wWinMainCRTStartup")

using namespace Gdiplus;

const wchar_t* storyPages[] = {
    L"\u042d\u0442\u043e \u0438\u0441\u0442\u043e\u0440\u0438\u044f \u043f\u0440\u043e \u043e\u0431\u044b\u043a\u043d\u043e\u0432\u0435\u043d\u043d\u043e\u0433\u043e \u0447\u0435\u0440\u043d\u043e\u0433\u043e \u043a\u0443\u0431\u0438\u043a\u0430...",
    L"...\u043a\u043e\u0442\u043e\u0440\u044b\u0439 \u043f\u044b\u0442\u0430\u0435\u0442\u0441\u044f \u043d\u0430\u0439\u0442\u0438 \u0441\u043c\u044b\u0441\u043b \u0436\u0438\u0437\u043d\u0438...",
    L"...\u0432 \u044d\u0442\u043e\u043c \u043e\u0433\u0440\u043e\u043c\u043d\u043e\u043c, \u043e\u0431\u044b\u043a\u043d\u043e\u0432\u0435\u043d\u043d\u043e\u043c \u043c\u0438\u0440\u0435."
};
const int TOTAL_PAGES = 3;

void AdvanceStory() {
    if (g_state == STATE_STORY) {
        if (g_charIndex < (int)g_currentText.length()) {
            g_charIndex = (int)g_currentText.length();
        } else {
            g_storyPage++;
            if (g_storyPage >= TOTAL_PAGES) {
                g_state = STATE_LOADING; // Переход на экран загрузки
                g_loadingTimer = GetTickCount();
            } else {
                g_currentText = storyPages[g_storyPage];
                g_charIndex = 0;
                g_typeTimer = GetTickCount();
            }
        }
    }
}

void OnMouseClick(int x, int y) {
    if (g_state == STATE_DISCLAIMER) {
        RECT btnRect = { 300, 500, 500, 550 };
        if (x >= btnRect.left && x <= btnRect.right && y >= btnRect.top && y <= btnRect.bottom) {
            g_state = STATE_MENU;
        }
    } 
    else if (g_state == STATE_MENU) {
        for (int i = 0; i < 3; i++) {
            RECT btnRect = { 300, 350 + (i * 50), 500, 390 + (i * 50) };
            if (x >= btnRect.left && x <= btnRect.right && y >= btnRect.top && y <= btnRect.bottom) {
                if (i == 0) { 
                    g_state = STATE_STORY;
                    g_storyPage = 0;
                    g_currentText = storyPages[0];
                    g_charIndex = 0;
                    g_typeTimer = GetTickCount();
                } else if (i == 1) {
                    g_state = STATE_OPTIONS;
                } else if (i == 2) { 
                    g_running = false;
                }
            }
        }
    }
    else if (g_state == STATE_OPTIONS) {
        RECT btnBack = { 500, 380, 580, 420 };
        if (x >= btnBack.left && x <= btnBack.right && y >= btnBack.top && y <= btnBack.bottom) {
            g_state = STATE_MENU;
        }
    }
    else if (g_state == STATE_STORY) {
        AdvanceStory();
    }
    else if (g_state == STATE_ADVICE) {
        RECT btnOk = { 300, 450, 500, 500 };
        if (x >= btnOk.left && x <= btnOk.right && y >= btnOk.top && y <= btnOk.bottom) {
            if (g_adviceFromPause) {
                g_state = STATE_PAUSED;
            } else {
                g_state = STATE_PLAYING;
                g_hasSeenAdvice = true; // Запоминаем, что прочитали
                g_levelStartTime = GetTickCount(); // Запускаем таймер
            }
        }
    }
    else if (g_state == STATE_PLAYING) {
        RECT pauseBtn = { 750, 10, 790, 50 };
        if (x >= pauseBtn.left && x <= pauseBtn.right && y >= pauseBtn.top && y <= pauseBtn.bottom) {
            g_state = STATE_PAUSED;
        }
    }
    else if (g_state == STATE_PAUSED) {
        RECT restartBtn = { 300, 250, 500, 290 };
        RECT adviceBtn  = { 300, 300, 500, 340 };
        RECT optionsBtn = { 300, 350, 500, 390 };
        RECT exitBtn    = { 300, 400, 500, 440 };

        if (x >= restartBtn.left && x <= restartBtn.right && y >= restartBtn.top && y <= restartBtn.bottom) {
            InitLevel1();
            g_state = STATE_PLAYING;
            g_levelStartTime = GetTickCount();
        }
        else if (x >= adviceBtn.left && x <= adviceBtn.right && y >= adviceBtn.top && y <= adviceBtn.bottom) {
            g_state = STATE_ADVICE;
            g_adviceFromPause = true; // Запоминаем, что открыли из паузы
        }
        else if (x >= optionsBtn.left && x <= optionsBtn.right && y >= optionsBtn.top && y <= optionsBtn.bottom) {
            g_state = STATE_OPTIONS;
        }
        else if (x >= exitBtn.left && x <= exitBtn.right && y >= exitBtn.top && y <= exitBtn.bottom) {
            g_state = STATE_EXIT_CONFIRM; 
        }
    }
    else if (g_state == STATE_EXIT_CONFIRM) {
        RECT yesBtn = { 250, 350, 350, 400 };
        RECT noBtn  = { 450, 350, 550, 400 };

        if (x >= yesBtn.left && x <= yesBtn.right && y >= yesBtn.top && y <= yesBtn.bottom) {
            g_state = STATE_MENU; 
        }
        else if (x >= noBtn.left && x <= noBtn.right && y >= noBtn.top && y <= noBtn.bottom) {
            g_state = STATE_PAUSED; 
        }
    }
    else if (g_state == STATE_GAME_OVER || g_state == STATE_COMPLETE) {
        g_state = STATE_MENU;
        InitMenu();
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_LBUTTONDOWN: {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        OnMouseClick(pt.x, pt.y);
        return 0;
    }
    case WM_KEYDOWN:
        if (wParam == 'Z') { 
            AdvanceStory();
        }
        else if (wParam == VK_ESCAPE || wParam == VK_RETURN) { 
            if (g_state == STATE_PLAYING) {
                g_state = STATE_PAUSED;
            } else if (g_state == STATE_PAUSED) {
                g_state = STATE_PLAYING;
            } else if (g_state == STATE_EXIT_CONFIRM) {
                g_state = STATE_PAUSED; 
            }
        }
        return 0;
    case WM_SETCURSOR:
        SetCursor(nullptr);
        return TRUE;
    case WM_DESTROY:
        PostQuitMessage(0);
        g_running = false;
        return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, int) {
    g_hInst = hInst;

    AddFontResourceExW(L"assets\\Monodetermination.ttf", FR_PRIVATE, 0);

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    ShowCursor(FALSE);

    WNDCLASSW wc = {};
    wc.hInstance = hInst;
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = L"SquareGameWnd";
    RegisterClassW(&wc);

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    g_hWnd = CreateWindowExW(WS_EX_TOPMOST, L"SquareGameWnd", L"Square Black and Green", 
        WS_POPUP | WS_VISIBLE, 
        0, 0, screenW, screenH, 
        nullptr, nullptr, hInst, nullptr);

    SetForegroundWindow(g_hWnd);
    SetFocus(g_hWnd);

    InitMenu();

    HDC hdc = GetDC(g_hWnd);
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBmp = CreateCompatibleBitmap(hdc, 800, 600);
    HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, memBmp);

    MSG msg;
    while (g_running) {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) g_running = false;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        // Обновление курсора для всех меню и оверлеев
        if (g_state == STATE_DISCLAIMER || g_state == STATE_MENU || g_state == STATE_OPTIONS || 
            g_state == STATE_STORY || g_state == STATE_PAUSED || g_state == STATE_EXIT_CONFIRM || 
            g_state == STATE_ADVICE || g_state == STATE_LOADING) {
            UpdateMenu();
        }
        
        if (g_state == STATE_PLAYING) {
            UpdateLevel1();
        }
        
        // Автоматический переход с экрана загрузки на игру
        if (g_state == STATE_LOADING && GetTickCount() - g_loadingTimer > 2000) {
            InitLevel1();
            if (!g_hasSeenAdvice) {
                g_state = STATE_ADVICE;
                g_adviceFromPause = false;
            } else {
                g_state = STATE_PLAYING;
                g_levelStartTime = GetTickCount();
            }
        }

        // Отрисовка
        if (g_state == STATE_DISCLAIMER || g_state == STATE_MENU || g_state == STATE_OPTIONS || 
            g_state == STATE_STORY || g_state == STATE_LOADING || g_state == STATE_ADVICE) {
            RenderMenu(memDC);
        }
        else if (g_state == STATE_PLAYING) {
            RenderLevel1(memDC);
        }
        else if (g_state == STATE_PAUSED || g_state == STATE_EXIT_CONFIRM) {
            RenderLevel1(memDC); 
            RenderMenu(memDC);   
        }
        else if (g_state == STATE_GAME_OVER) {
            RenderGameOver(memDC); 
        }
        else if (g_state == STATE_COMPLETE) {
            RenderComplete(memDC);
        }

        BitBlt(hdc, 0, 0, 800, 600, memDC, 0, 0, SRCCOPY);
        Sleep(16); 
    }

    RemoveFontResourceExW(L"assets\\Monodetermination.ttf", FR_PRIVATE, 0);
    SelectObject(memDC, oldBmp);
    DeleteObject(memBmp);
    DeleteDC(memDC);
    ReleaseDC(g_hWnd, hdc);
    GdiplusShutdown(gdiplusToken);
    return 0;
}
