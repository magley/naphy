#pragma once

struct PhysBody;
struct Arbiter;

// Computes data for the arbiter based on contacts between the two bodies.
// THE BODIES ARE NOT MODIFIED.
// @param R The arbiter.
// @returns 1 if a collision happened, 0 otherwise.
int collision(Arbiter* const R);