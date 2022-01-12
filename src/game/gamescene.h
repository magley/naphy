#pragma once

#include "csprite.h"
#include "cdrifter.h"

#include "utility/input.h"
#include "naphy/physscene.h"
#include "gui/gui.h"

#include <set>

struct GameScene {
    PhysScene* physscene;
    GUI* gui;

    CDrifter drifter;
    std::multiset<CSprite> spr_queue; // Larger depth goes in front

    // All should be on heap.
    GameScene(PhysScene* physscene, GUI* gui);
    ~GameScene();
};