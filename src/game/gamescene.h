#pragma once

#include "entity.h"

#include "utility/input.h"
#include "naphy/physscene.h"
#include "gui/gui.h"
#include "rend/image.h"

#include <vector>
#include <set>

struct GameScene {
    PhysScene* physscene;
    GUI* gui;
    std::vector<Entity> entity;
    std::multiset<SpriteContext> spr_ctx;

    GameScene(PhysScene* physscene, GUI* gui);
    ~GameScene();

    // This doesn't actually draw anything. To preview changes, use flush_sprites().
    void draw_sprite(const SpriteContext& ctx);
    // This should be called once per game update.
    void flush_sprites();
};