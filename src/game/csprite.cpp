#include "csprite.h"

Sprite spr[10];
void sprites_init(Image* img_drifter) {
	spr[SPR_DRIFTER_DOWN_STAND] = Sprite(img_drifter, Vec2(16, 32), Vec2(0, 0), 1);
	spr[SPR_DRIFTER_DOWN_WALK] = Sprite(img_drifter, Vec2(16, 32), Vec2(16, 0), 8);
	spr[SPR_DRIFTER_DOWN_DRIFT] = Sprite(img_drifter, Vec2(16, 32), Vec2(9*16, 0), 8);

	spr[SPR_DRIFTER_RIGHT_STAND] = Sprite(img_drifter, Vec2(16, 32), Vec2(0, 32), 1);
	spr[SPR_DRIFTER_RIGHT_WALK] = Sprite(img_drifter, Vec2(16, 32), Vec2(16, 32), 8);
	spr[SPR_DRIFTER_RIGHT_DRIFT] = Sprite(img_drifter, Vec2(32, 32), Vec2(9*16, 32), 8);

	spr[SPR_DRIFTER_UP_STAND] = Sprite(img_drifter, Vec2(16, 32), Vec2(0, 64), 1);
	spr[SPR_DRIFTER_UP_WALK] = Sprite(img_drifter, Vec2(16, 32), Vec2(16, 64), 8);
	spr[SPR_DRIFTER_UP_DRIFT] = Sprite(img_drifter, Vec2(16, 32), Vec2(9*16, 64), 8);
}




Sprite::Sprite() {;}

Sprite::Sprite(Image* img, Vec2 size, Vec2 pos, unsigned frames) {
	this->img = img;
	this->size = size;
	this->pos = pos;
	this->frames = frames;
}

CSprite::CSprite() {;}

CSprite::CSprite(unsigned sprite_index, double image_speed) {
	this->sprite_index = sprite_index;
	this->image_index = 0;
	this->image_speed = image_speed;
	this->repeat = 1;
}


void CSprite::update() {
	// TODO: What if image_speed > s->frames?

	const Sprite* s = &spr[sprite_index];
	if ((unsigned)(image_index + image_speed) > s->frames - 1) {
		if (repeat)
			image_index = image_speed;
		else
			image_index = s->frames - 1;
	} else {
		image_index += image_speed;
	}
}

void CSprite::set(unsigned spr_index, unsigned reset_if_diff, unsigned reset_if_same) {
	if (sprite_index == spr_index && reset_if_same)
		image_index = 0;
	if (sprite_index != spr_index && reset_if_diff)
		image_index = 0;
	sprite_index = spr_index;
}


void CSprite::draw(Vec2 pos) {
	Sprite* s = &spr[sprite_index];
	const unsigned img_index = (unsigned)image_index;

	s->img->draw_ex(
		pos.x, 
		pos.y, 
		s->pos.x + s->size.x * img_index,
		s->pos.y,
		s->size.x,
		s->size.y,
		sdl_flip
	);
}