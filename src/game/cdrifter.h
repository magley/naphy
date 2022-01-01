#pragma once

#include "naphy/physbody.h"
#include "utility/input.h"
#include "rend/image.h"

#include "csprite.h"


enum SprDrifter { // x offset 
	SPR_DRIFTER_STAND = 0,
	SPR_DRIFTER_WALK = 1,
};

enum SprDrifterSubimageY { // y offset
	SPR_DRIFTER_SUBIMG_DOWN = 0,
	SPR_DRIFTER_SUBIMG_RIGHT = 1,
	SPR_DRIFTER_SUBIMG_LEFT = 2,
	SPR_DRIFTER_SUBIMG_UP = 3,
}; 


struct CDrifter {
	int drift_time;
	int drift_combo; // How many combos you're on.

	unsigned dir; // SprDrifterSubimageY
	CSprite sprite;

	PhysBody* body; // Physbody instance I'm attached to.

	CDrifter();
	CDrifter(PhysBody* body);

	void update(const Input* input); // Basic movement. 
	void draw(const Image* img) const; // Draw player.
};