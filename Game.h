#pragma once
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

enum GameState {
    STATE_DISCLAIMER,
    STATE_MENU,
    STATE_OPTIONS,     // Новое состояние для настроек
    STATE_PLAYING,
    STATE_GAME_OVER,
    STATE_COMPLETE
};

extern GameState g_state;
extern HINSTANCE g_hInst;
extern HWND g_hWnd;
extern bool g_running;

void InitMenu();
void UpdateMenu();
void RenderMenu(HDC hdc);

void InitLevel1();
void UpdateLevel1();
void RenderLevel1(HDC hdc);

void RenderGameOver(HDC hdc);
void RenderComplete(HDC hdc);
