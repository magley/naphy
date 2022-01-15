#include "csprite.h"

//-------------------------------------------------------------------------------------------------
// Sprite resources

Sprite spr[10];
void sprites_init(Image* img_atlas) {
	// Down
	spr[SPR_DRIFTER_DOWN_STAND] 	= Sprite(img_atlas, {16, 32}, {0 * 16, 0}, 	1);
	spr[SPR_DRIFTER_DOWN_WALK] 		= Sprite(img_atlas, {16, 32}, {1 * 16, 0}, 	8);
	spr[SPR_DRIFTER_DOWN_DRIFT] 	= Sprite(img_atlas, {16, 32}, {9 * 16, 0}, 	8);
	// Right
	spr[SPR_DRIFTER_RIGHT_STAND] 	= Sprite(img_atlas, {16, 32}, {0 * 16, 32}, 1);
	spr[SPR_DRIFTER_RIGHT_WALK] 	= Sprite(img_atlas, {16, 32}, {1 * 16, 32}, 8);
	spr[SPR_DRIFTER_RIGHT_DRIFT] 	= Sprite(img_atlas, {32, 32}, {9 * 16, 32}, 8);
	// Up
	spr[SPR_DRIFTER_UP_STAND] 		= Sprite(img_atlas, {16, 32}, {0 * 16, 64}, 1);
	spr[SPR_DRIFTER_UP_WALK] 		= Sprite(img_atlas, {16, 32}, {1 * 16, 64}, 8);
	spr[SPR_DRIFTER_UP_DRIFT] 		= Sprite(img_atlas, {16, 32}, {9 * 16, 64}, 8);

	spr[SPR_BG_FLOOR]				= Sprite(img_atlas, {480, 270}, {0, 96}, 	1);
}

//-------------------------------------------------------------------------------------------------
// Sprite

Sprite::Sprite() { ; }

Sprite::Sprite(Image* img, Vec2 size, Vec2 pos, unsigned frames) {
	this->img = img;
	this->size = size;
	this->pos = pos;
	this->frames = frames;
}

//-------------------------------------------------------------------------------------------------
// CSprite

CSprite::CSprite() { ; }

CSprite::CSprite(unsigned sprite_index, double image_speed) {
	this->sprite_index = sprite_index;
	this->image_index = 0;
	this->image_speed = image_speed;
	this->repeat = 1;
	this->sdl_flip = SDL_FLIP_NONE;
}

void CSprite::update() {
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
	const SpriteDrawContext ctx = make_ctx(pos);
	Sprite* s = &spr[ctx.sprite_index];
	s->img->draw(ctx.pos.x, ctx.pos.y, ctx.subx, ctx.suby, ctx.subw, ctx.subh, ctx.flip);
}

SpriteDrawContext CSprite::make_ctx(Vec2 pos, uint8_t r, uint8_t g, uint8_t b, uint8_t a) const {
	const Sprite* const s = &spr[sprite_index];
	const SpriteDrawContext ctx = {
		.sprite_index 	= sprite_index,
		.image_index 	= (unsigned)image_index,
		.flip 			= sdl_flip,
		.depth 			= depth,
		.pos 			= pos,
		.subx 			= (int)(s->pos.x + s->size.x * ctx.image_index),
		.suby 			= (int)s->pos.y,
		.subw 			= (int)s->size.x,
		.subh 			= (int)s->size.y,
		.r 				= r,
		.g 				= g,
		.b 				= b,
		.a 				= a,
	};

	return ctx;
}