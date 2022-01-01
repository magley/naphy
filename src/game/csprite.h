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


extern Sprite sprites[10];


struct CSprite {
	double image_index;
	double image_speed;
	unsigned sprite_index; // Index of array of all Sprite objects.

	CSprite();
	CSprite(unsigned sprite_index, double image_speed);
	void update(Sprite* spr_array);
	void draw(Vec2 pos, Sprite* spr_array);
};

