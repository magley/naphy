#pragma once

#include "cdrifter.h"
#include "csprite.h"
#include "cphys.h"
#include "cjumpnrun.h"

struct Input;
struct GameScene;
struct Image;

enum {
    C_NONE      = 0,
    C_DRIFTER   = 1 << 0,
    C_SPRITE    = 1 << 1,
    C_PHYS      = 1 << 2,
    C_JUMPNRUN  = 1 << 3,
};

struct Entity {
    CDrifter cdrifter;
    CSprite csprite;
    CPhys cphys;
    CJumpNRun cjumpnrun;

    unsigned flag; // use flag_a() and flag_c()

    inline void flag_a(unsigned f) { flag |= f; }
    inline unsigned flag_c(unsigned f) const { return (flag & f) != 0; }
 
    Entity();
    Entity(unsigned flag);

    void update(const Input* input);
    void draw(GameScene* scene, Image* img_atlas);
};