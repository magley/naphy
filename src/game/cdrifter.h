#pragma once

#include "csprite.h"
#include "naphy/physbody.h"
#include "rend/image.h"
#include "utility/input.h"

enum {
	DRIFTER_STATE_STAND,		// Standing
	DRIFTER_STATE_WALK,			// Walking
	DRIFTER_STATE_STOPWALK,		// Walkting to a stop (no move key held but not standing)
	DRIFTER_STATE_DRIFTSTART,	// First frame of a drift (combo drift included)
	DRIFTER_STATE_DRIFT			// All frames of a drift EXCEPT the first one
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

	unsigned trail_i;         // Index of next trail to update. Usually is > array size, so we divide.
	unsigned trail_cnt;       // 0 when you don't display trails, trail.size() when you do.
	std::vector<Vec2> trail;  // Where each trail should be drawn (body->pos).

	CSprite sprite;

	PhysBody* body;  // Physbody instance I'm attached to.

	CDrifter();
	CDrifter(PhysBody* body);

	void update(const Input* input);         // Basic movement.
	void update_sprite(const Input* input);  // Sprite update. Called by update().
	void draw(const Image* img) const;       // Draw player.

	int state;    // DrifterState enum
	int movedir;  // DrifterDir enum
};