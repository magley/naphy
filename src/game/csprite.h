#pragma once

#include "utility/mathutil.h"
#include "rend/image.h"


struct Sprite {
	Image* img;
	Vec2 size;
	Vec2 pos; // Pos of first image, in pixels, relative to main image.
	unsigned frames;

	Sprite();
	Sprite(Image* img, Vec2 size, Vec2 pos, unsigned frames);
};


extern Sprite spr[10];
void sprites_init(Image* img_drifter);
enum SpriteNames {
	SPR_DRIFTER_DOWN_STAND,
	SPR_DRIFTER_DOWN_WALK,

	SPR_DRIFTER_RIGHT_STAND,
	SPR_DRIFTER_RIGHT_WALK,

	SPR_DRIFTER_LEFT_STAND,
	SPR_DRIFTER_LEFT_WALK,

	SPR_DRIFTER_UP_STAND,
	SPR_DRIFTER_UP_WALK
};



struct CSprite {
	double image_index;
	double image_speed;
	unsigned sprite_index; // Index of array of all Sprite objects.

	CSprite();
	CSprite(unsigned sprite_index, double image_speed);
	void update();
	void draw(Vec2 pos);
};

