#pragma once

#include "rend/image.h"
#include "utility/mathutil.h"

struct Sprite {
	Image* img;
	Vec2 size;
	Vec2 pos;  // Pos of first sprite frame, in pixels, relative to main image.
	unsigned frames;

	Sprite();
	Sprite(Image* img, Vec2 size, Vec2 pos, unsigned frames);
};

extern Sprite spr[10];
void sprites_init(Image* img_drifter);
enum SpriteNames {
	SPR_DRIFTER_DOWN_STAND,
	SPR_DRIFTER_DOWN_WALK,
	SPR_DRIFTER_DOWN_DRIFT,

	SPR_DRIFTER_RIGHT_STAND,
	SPR_DRIFTER_RIGHT_WALK,
	SPR_DRIFTER_RIGHT_DRIFT,

	SPR_DRIFTER_UP_STAND,
	SPR_DRIFTER_UP_WALK,
	SPR_DRIFTER_UP_DRIFT,
};

struct CSprite {
	double image_index;
	double image_speed;
	unsigned sprite_index;  // Index of array of all Sprite objects.
	unsigned repeat;        // 0 :: don't repeat animation, 1 :: repeat animation.
	int sdl_flip;           // Can be any of SDL_FLIP_HORIZONTAL, SDL_FLIP_VERTICAL, SDL_FLIP_NONE.

	CSprite();
	CSprite(unsigned sprite_index, double image_speed);

	void update();
	void draw(Vec2 pos);
	// @param spr_index Index of the sprite. See the SpriteNames enum.
	// @param reset_if_diff Reset the animation if the sprite is different? (1 = yes, 0 = no)
	// @param reset_if_same Reset the animation if the sprite is the same? (1 = yes, 0 = no)
	void set(unsigned spr_index, unsigned reset_if_diff, unsigned reset_if_same);
};
