#pragma once

struct PhysBody;
struct Arbiter;

/**
 * @brief Computes data for the arbiter based on contacts between the two bodies.
 * @param R The arbiter.
 * @returns int 1 if a collision happened, 0 otherwise.
 */
int collision(Arbiter* const R);