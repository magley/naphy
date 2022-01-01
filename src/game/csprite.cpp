#include "csprite.h"

Sprite sprites[];


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


void CSprite::update(Sprite* spr_array) {
	// TODO: What if image_speed > spr->frames?

	const Sprite* spr = &spr_array[sprite_index];
	if ((unsigned)(image_index + image_speed) > spr->frames) {
		image_index = image_speed;
	} else {
		image_index += image_speed;
	}
}


void CSprite::draw(Vec2 pos, Sprite* spr_array) {
	const Sprite* spr = &spr_array[sprite_index];
	const unsigned img_index = (unsigned)image_index;

	spr->img->draw(
		pos.x, 
		pos.y,
		spr->pos.x * spr->size.x * img_index,
		spr->pos.y,
		spr->size.x,
		spr->size.y
	);
}