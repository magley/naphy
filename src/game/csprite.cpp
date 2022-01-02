#include "csprite.h"

Sprite spr[10];
void sprites_init(Image* img_drifter) {
	spr[SPR_DRIFTER_DOWN_STAND] = Sprite(img_drifter, Vec2(16, 32), Vec2(0, 0), 1);
	spr[SPR_DRIFTER_DOWN_WALK] = Sprite(img_drifter, Vec2(16, 32), Vec2(16, 0), 8);

	spr[SPR_DRIFTER_RIGHT_STAND] = Sprite(img_drifter, Vec2(16, 32), Vec2(0, 32), 1);
	spr[SPR_DRIFTER_RIGHT_WALK] = Sprite(img_drifter, Vec2(16, 32), Vec2(16, 32), 8);

	spr[SPR_DRIFTER_LEFT_STAND] = Sprite(img_drifter, Vec2(16, 32), Vec2(0, 64), 1);
	spr[SPR_DRIFTER_LEFT_WALK] = Sprite(img_drifter, Vec2(16, 32), Vec2(16, 64), 8);

	spr[SPR_DRIFTER_UP_STAND] = Sprite(img_drifter, Vec2(16, 32), Vec2(0, 96), 1);
	spr[SPR_DRIFTER_UP_WALK] = Sprite(img_drifter, Vec2(16, 32), Vec2(16, 96), 8);
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
}


void CSprite::update() {
	// TODO: What if image_speed > s->frames?

	const Sprite* s = &spr[sprite_index];
	if ((unsigned)(image_index + image_speed) > s->frames - 1) {
		image_index = image_speed;
	} else {
		image_index += image_speed;
	}
}


void CSprite::draw(Vec2 pos) {
	Sprite* s = &spr[sprite_index];
	const unsigned img_index = (unsigned)image_index;

	s->img->draw(
		pos.x, 
		pos.y,
		s->pos.x + s->size.x * img_index,
		s->pos.y,
		s->size.x,
		s->size.y
	);
}