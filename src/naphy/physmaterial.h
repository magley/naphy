#pragma once
#include "SDL2/SDL_pixels.h"


// Compound structure for various physical properties.
struct PhysMaterial {
	// Static friction keeps an inert body at rest.
	double sfric;
	// Kinetic friction puts a moving body to rest.
	double kfric;
	// Coefficient of restitution is the bounce factor.
	double e;
	// Color for drawing. Used to visually identify different materials for debugging.
	SDL_Color col;

	PhysMaterial();
	PhysMaterial(double static_fric, double kinetic_friction, double resitution);

	// Preset function. Call one of these to change the values to its preset.

	void set_default();
	void set_ice();
	void set_metal();
	void set_jelly();
};