#pragma once

#include "utility/mathutil.h"
struct PhysBody;

// A Spring binds two bodies using Hooke's law.
// TODO The parameters k and c should be normalized to [0, 1].
struct Spring {
	// Connected body.
	PhysBody *A, *B;
	// Equilibrium distance. If the bodies are not on this distance, force is applied.
	double rest_length;
	// Spring constant. Determines how strong the force between the adjacent bodies is.
	double k;
	// Damping factor. Determines how quickly the spring force weakens.
	double c;

	// Construct a new spring between the two bodies. Be careful of duplicate springs.
	// @param A First body.
	// @param B Second body.
	// @param rest_length Distance at which the bodes are in equilibrium.
	// @param k Strength of the spring. If unsure, try 0.5.
	// @param c Damping factor. If unsure, try 0.5.
	Spring(PhysBody* A, PhysBody* B, double rest_length, double k, double c);

	// Apply force to the two bound bodies.
	void solve();
};
