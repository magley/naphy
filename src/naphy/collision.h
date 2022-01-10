#pragma once
#include "utility/mathutil.h"

struct PhysBody;
struct Arbiter;

// Computes data for the arbiter based on contacts between the two bodies.
// THE BODIES ARE NOT MODIFIED.
// @param R The arbiter.
// @returns 1 if a collision happened, 0 otherwise.
int collision(Arbiter* const R);

// Raycast against a POLYGON body.
// Returns an arbiter detaling raycast results (depth=0 -> no collision).
Arbiter raycast(Vec2 raystart, Vec2 rayend, const PhysBody* B);

// Sweep-based continuous collsion.
int sweep(Arbiter* R, double dt, int iterations, Vec2 A_vel_priv, Vec2 B_vel_priv);