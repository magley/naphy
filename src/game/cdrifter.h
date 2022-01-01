#pragma once

#include "naphy/physbody.h"
#include "utility/input.h"
#include "rend/image.h"

#include "csprite.h"


struct CDrifter {
	int drift_time;
	int drift_combo; // How many combos you're on.

	CSprite sprite;

	PhysBody* body; // Physbody instance I'm attached to.

	CDrifter();
	CDrifter(PhysBody* body);

	void update(const Input* input); // Basic movement. 
	void draw(const Image* img) const; // Draw player.
};