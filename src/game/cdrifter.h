#pragma once

#include "csprite.h"
#include "naphy/physbody.h"
#include "rend/image.h"
#include "utility/input.h"
#include <list>

struct Scene;

enum {
	DRIFTER_STATE_STAND,		// Standing
	DRIFTER_STATE_WALK,			// Walking
	DRIFTER_STATE_STOPWALK,		// Walkting to a stop (no move key held but not standing)
	DRIFTER_STATE_DRIFTSTART,	// First frame of a drift (combo drift included)
	DRIFTER_STATE_DRIFT			// All frames of a drift EXCEPT the first one
	// TODO: Use multiple bits for state. STATE_DRIFT and STATE_DRIFTSTART should not be exclusive.
} DrifterState;

enum {
	DRIFTER_DOWN,
	DRIFTER_RIGHT,
	DRIFTER_UP,
	DRIFTER_LEFT
} DrifterDir;

struct CDrifter {
	int drift_time;
	int drift_combo;  // How many combos you're on.

	CSprite sprite;

	std::list<Vec2> trails;

	PhysBody* body;  // Physbody instance I'm attached to.

	CDrifter();
	CDrifter(PhysBody* body);

	void update(const Input* input, Scene* scene);
	void update_sprite(const Input* input);
	void draw(const Image* img) const; 

	int state;    // DrifterState enum
	int movedir;  // DrifterDir enum
};