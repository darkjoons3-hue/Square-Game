#include "Game.h"

GameState g_state = STATE_DISCLAIMER;
HINSTANCE g_hInst = nullptr;
HWND g_hWnd = nullptr;
bool g_running = true;

int g_storyPage = 0;
int g_charIndex = 0;
DWORD g_typeTimer = 0;
std::wstring g_currentText = L"";

DWORD g_levelStartTime = 0;
DWORD g_gameOverTimer = 0;
