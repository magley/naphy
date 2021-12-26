#pragma once

#include "utility/mathutil.h"
struct PhysBody;

struct Spring {
	// Connected body.
	PhysBody *A, *B;
	// Equilibrium distance.
	double rest_length;
	// Spring constant.
	double k;
	// Damping factor.
	double c;

	Spring(PhysBody* A, PhysBody* B, double rest_length, double k, double c);

	void solve();
};
