#pragma once

#include "cphys.h"
#include "csprite.h"
#include "rend/image.h"
#include "utility/input.h"
#include <list>

struct GameScene;

enum DrifterLayersPhysBody{ // Use for PhysBody::layer.
	LAYER_DRIFTER,
	LAYER_SOLID,
	LAYER_PIT,
};

enum DrifterState{
	DRIFTER_STATE_STAND,		// Standing
	DRIFTER_STATE_WALK,			// Walking
	DRIFTER_STATE_STOPWALK,		// Walkting to a stop (no move key held but not standing)
	DRIFTER_STATE_DRIFT,		// All frames of a drift
	DRIFTER_STATE_FALLDIE		// When you fall in a bottomless pit
};

enum DrifterDir{
	DRIFTER_DOWN,
	DRIFTER_RIGHT,
	DRIFTER_UP,
	DRIFTER_LEFT,
};

struct CDrifter {
	unsigned state;   		// DrifterState
	unsigned movedir; 		// DrifterDir
	unsigned drift_time;
	std::list<Vec2> trails;
	CDrifter();
	
	void move_and_drift(CPhys* phys, const Input* input);
	void fall_in_pits(CPhys* phys);
	void update_sprite(const CPhys* phys, CSprite* spr, const Input* input);
	void draw(const CPhys* phys, const CSprite* spr, const Image* image, GameScene* scene) const;
};