#pragma once

#include "csprite.h"
#include "cdrifter.h"

#include "naphy/scene.h"
#include "gui/gui.h"

#include <set>

struct GameScene {
    Scene* scene;
    GUI* gui;

    CDrifter drifter;
    std::multiset<CSprite> spr_queue; // Larger depth goes in front

    GameScene(Scene* scene, GUI* gui);
};