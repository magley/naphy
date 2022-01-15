#include "gamescene.h"

GameScene::GameScene(PhysScene* physscene, GUI* gui) {
    this->physscene = physscene;
    this->gui = gui;
    this->gui->scene = this;
}

GameScene::~GameScene() {
    delete physscene;
    delete gui;
}

void GameScene::draw_sprite(const SpriteDrawContext& ctx) {
    spr_ctx.insert(ctx);
}

void GameScene::flush_sprites() {
    if (spr_ctx.size() == 0)
        return;

    Image* img = spr[0].img; // TODO: don't assume that sprites share the same Image
    SDL_Texture* tex = img->img; // it's assumed now because it makes resetting color/alpha mod easy

    uint8_t r_, g_, b_, a_;
	SDL_GetTextureColorMod(tex, &r_, &g_, &b_);
	SDL_GetTextureAlphaMod(tex, &a_);

    for (const SpriteDrawContext& ctx : spr_ctx) {
        SDL_SetTextureColorMod(tex, ctx.r, ctx.g, ctx.b);
        SDL_SetTextureAlphaMod(tex, ctx.a);
        img->draw(ctx.pos.x, ctx.pos.y, ctx.subx, ctx.suby, ctx.subw, ctx.subh, ctx.flip);
    }
    spr_ctx.clear();
    
    SDL_SetTextureColorMod(tex, r_, g_, b_);
    SDL_SetTextureAlphaMod(tex, a_);
}