#pragma once

#include "naphy/physbody.h"
#include "utility/input.h"
#include "rend/image.h"

#include "csprite.h"


struct CDrifter {
	int drift_time;
	int drift_combo; // How many combos you're on.

	unsigned trail_i; // Index of next trail to update. Usually is > array size, so we divide.
	unsigned trail_cnt; // 0 when you don't display trails, trail.size() when you do.
	std::vector<Vec2> trail; // Where each trail should be drawn (body->pos).

	CSprite sprite;

	PhysBody* body; // Physbody instance I'm attached to.

	CDrifter();
	CDrifter(PhysBody* body);

	void update(const Input* input); // Basic movement. 
	void draw(const Image* img) const; // Draw player.
};