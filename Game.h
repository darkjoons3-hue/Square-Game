#pragma once
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include <string>

enum GameState {
    STATE_DISCLAIMER,
    STATE_MENU,
    STATE_OPTIONS,
    STATE_STORY,
    STATE_PLAYING,
    STATE_PAUSED,      // НОВОЕ: Пауза
    STATE_GAME_OVER,
    STATE_COMPLETE
};

extern GameState g_state;
extern HINSTANCE g_hInst;
extern HWND g_hWnd;
extern bool g_running;

// Переменные для экрана истории
extern int g_storyPage;
extern int g_charIndex;      
extern DWORD g_typeTimer;    
extern std::wstring g_currentText; 

// Переменные для таймера игры
extern DWORD g_levelStartTime;
extern DWORD g_gameOverTimer;

void InitMenu();
void UpdateMenu();
void RenderMenu(HDC hdc);

void InitLevel1();
void UpdateLevel1();
void RenderLevel1(HDC hdc);

void RenderGameOver(HDC hdc);
void RenderComplete(HDC hdc);
