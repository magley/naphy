#include "gamescene.h"
#include "stdio.h"

GameScene::GameScene(PhysScene* physscene, GUI* gui) {
    this->physscene = physscene;
    this->gui = gui;
    this->gui->scene = this;
}

GameScene::~GameScene() {
    delete physscene;
    delete gui;
}

void GameScene::draw_sprite(const SpriteContext& ctx) {
    spr_ctx.insert(ctx);
}

void GameScene::flush_sprites() {
    if (spr_ctx.size() == 0)
        return;

    // TODO: don't assume sprites share the same Image (it's assumed now for easy color/alpha reset)
    SDL_Texture* tex = spr[0].img->tex;

    uint8_t r_, g_, b_, a_;
	SDL_GetTextureColorMod(tex, &r_, &g_, &b_);
	SDL_GetTextureAlphaMod(tex, &a_);

    for (const SpriteContext& ctx : spr_ctx) {
        const Sprite* s = &spr[ctx.sprite_index];
        const Image* img = s->img;

        SDL_SetTextureColorMod(tex, ctx.r, ctx.g, ctx.b);
        SDL_SetTextureAlphaMod(tex, ctx.a);
        img->draw(
            ctx.x,
            ctx.y,
            s->x + ctx.image_index * s->w,
            s->y,
            s->w,
            s->h,
            ctx.flip
        );
    }
    spr_ctx.clear();
    
    SDL_SetTextureColorMod(tex, r_, g_, b_);
    SDL_SetTextureAlphaMod(tex, a_);
}