#pragma once

struct PhysBody;
struct Arbiter;

/**
 * @brief Computes data for the arbiter based on contacts between the two bodies.
 * 
 * 
 * There's a reason we specify not only the arbiter, but also the two bodies.
 * Normally, we could just pass the arbiter parameter and fetch body pointer
 * from its fields. This function checks the types of the bodies' shapes, and
 * delegates the work to a helper function based on those types. For circle-poly
 * collision, we have a dedicated algorithm, but for poly-circle, all that's
 * needed is to swap the parameters and call the circle-poly (as well flip the
 * collision normal). We *could* just swap the pointers in the arbiter, but that
 * would mess up later collision checking
 * 
 * TODO: PROOF
 * 
 * 
 * 
 * 
 * @param A The first body.
 * @param B The second body.
 * @param R The arbiter.
 * 
 * @returns int 1 if a collision happened, 0 otherwise.
 */
int collision(const PhysBody* const A, const PhysBody* const B, Arbiter* const R);