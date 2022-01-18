#include "csprite.h"


CSprite::CSprite() { 
}

CSprite::CSprite(unsigned sprite_index, double image_speed) {
	this->sprite_index = sprite_index;
	this->image_index = 0;
	this->image_speed = image_speed;
	this->repeat = 1;
	this->sdl_flip = SDL_FLIP_NONE;
}

void CSprite::update_animation() {
	const Sprite* s = &spr[sprite_index];

	if ((unsigned)(image_index + image_speed) > s->frames - 1) {
		if (repeat)
			image_index = fmod(image_index + image_speed, s->frames);
		else
			image_index = s->frames - 1;
	} else {
		image_index += image_speed;
	}
}

void CSprite::set(unsigned spr_index, unsigned reset_if_diff, unsigned reset_if_same) {
	if ((sprite_index == spr_index && reset_if_same)
	|| (sprite_index != spr_index && reset_if_diff))
		image_index = 0;
	
	sprite_index = spr_index;
}

void CSprite::draw(Vec2 pos) const {
	const SpriteContext ctx = make_ctx(pos);
	Sprite* s = &spr[ctx.sprite_index];
	s->img->draw(
		ctx.x,
		ctx.y,
		s->x + ctx.image_index * s->w,
		s->y,
		s->w,
		s->h,
		ctx.flip
	);
}

SpriteContext CSprite::make_ctx(Vec2 pos, uint8_t r, uint8_t g, uint8_t b, uint8_t a) const {
	const SpriteContext ctx = {
		.sprite_index 	= sprite_index,
		.image_index 	= (unsigned)image_index,
		.flip 			= sdl_flip,
		.depth 			= depth,
		.x 				= pos.x,
		.y				= pos.y,
		.r 				= r,
		.g 				= g,
		.b 				= b,
		.a 				= a,
	};

	return ctx;
}