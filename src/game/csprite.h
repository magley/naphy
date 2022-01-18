#pragma once

#include "rend/image.h"
#include "utility/mathutil.h"
#include "resources.h"



struct CSprite {
	double image_index;
	double image_speed;
	unsigned sprite_index;  // Index of array of all Sprite objects.
	unsigned repeat;        // 0 :: don't repeat animation, 1 :: repeat animation.
	int sdl_flip;           // Can be any of SDL_FLIP_HORIZONTAL, SDL_FLIP_VERTICAL, SDL_FLIP_NONE.
	int depth;

	CSprite();
	CSprite(unsigned sprite_index, double image_speed);

	void update_animation();

	// You shouldn't use this method.
	// WHY: Delegate the drawing to someone else, like a GameScene.
	// HOW: Create a SpriteDrawContext using make_ctx. When you're ready to draw, obtain data from
	// the context.
	void draw(Vec2 pos) const;
	SpriteContext make_ctx(Vec2 pos, 
								uint8_t r = 255, 
								uint8_t g = 255, 
								uint8_t b = 255, 
								uint8_t a = 255) const;
	// @param spr_index Index of the sprite. See the SpriteNames enum.
	// @param reset_if_diff Reset the animation if the sprite is different? (1 = yes, 0 = no)
	// @param reset_if_same Reset the animation if the sprite is the same? (1 = yes, 0 = no)
	void set(unsigned spr_index, unsigned reset_if_diff, unsigned reset_if_same);
};

inline bool operator <(const CSprite& a, const CSprite& b) {
	return a.depth < b.depth;
}