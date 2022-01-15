#pragma once

#include "csprite.h"
#include "cdrifter.h"

#include "utility/input.h"
#include "naphy/physscene.h"
#include "gui/gui.h"
#include "rend/image.h"

#include "entity.h"

#include <vector>
#include <set>

struct GameScene {
    PhysScene* physscene;
    GUI* gui;

    std::vector<Entity> entity;
    std::multiset<SpriteDrawContext> spr_ctx;

    // All should be on heap.
    GameScene(PhysScene* physscene, GUI* gui);
    ~GameScene();

    void draw_sprite(const SpriteDrawContext& ctx);
    void flush_sprites();
};