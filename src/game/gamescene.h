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

    // This doesn't actually draw anything on the screen, but it's what you call when you want a
    // sprite to be drawn. To actualy display the sprite(s), flush_sprites() is invoked.
    void draw_sprite(const SpriteContext& ctx);
    // Draw all sprites onto the screen and clear the sprites from the container.
    // This should be called once per game update.
    void flush_sprites();
};