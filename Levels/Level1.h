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
    int w, h;
    bool pointingDown; 
};
