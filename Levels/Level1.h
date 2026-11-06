#pragma once
#include "../Game.h"
#include <vector>

struct Player {
    float x, y;
    float vx, vy;
    int size = 50; 
    bool onGround = false;
};

struct Platform {
    float x, y, w, h;
};

struct Spike {
    float x, y;
    float w, h;
    bool pointingDown; 
};

struct KeyItem {
    float x, y;
    bool collected = false;
};

struct LockDoor {
    float x, y, w, h;
};

struct FinishFlag {
    float x, y;
};

void InitLevel1();
void UpdateLevel1();
void RenderLevel1(HDC hdc);
