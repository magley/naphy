#pragma once
#include "SDL2/SDL_pixels.h"

struct PhysMaterial {
	// Static friction keeps an inert body at rest.
	double sfric;
	// Kinetic friction puts a moving body to rest.
	double kfric;
	// Coefficient of restitution is the bounce factor.
	double e;
	// Color for drawing. Used to visually identify different materials.
	SDL_Color col;

	PhysMaterial();
	PhysMaterial(double static_fric, double kinetic_friction, double resitution);

	// Presets

	void set_default();
	void set_ice();
	void set_metal();
	void set_jelly();
};