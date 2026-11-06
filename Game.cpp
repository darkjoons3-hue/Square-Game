#include "Game.h"

GameState g_state = STATE_DISCLAIMER;
GameState g_prevState = STATE_MENU;
GameState g_nextState = STATE_MENU;
HINSTANCE g_hInst = nullptr;
HWND g_hWnd = nullptr;
bool g_running = true;

int g_storyPage = 0;
int g_charIndex = 0;
DWORD g_typeTimer = 0;
std::wstring g_currentText = L"";

DWORD g_levelStartTime = 0;
DWORD g_gameOverTimer = 0;
DWORD g_loadingTimer = 0;
bool g_hasSeenAdvice = false;

bool g_showFps = false;
int g_fps = 0;
DWORD g_lastFpsTime = 0;
int g_frameCount = 0;
