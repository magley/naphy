#pragma once
#include "utility/mathutil.h"
#include <vector>


struct PhysBody;


// An Arbiter stores collision information between two PhysBody instances.
struct Arbiter {
    // Pointer to the colliding body.
	PhysBody *A, *B;
    // Penetration depth.
	double depth;
    // Normal of collision.
    // Normal impulses are perpendicular to this vector, while tangential impulses are normal to it.
	Vec2 normal;
    // Vector of all contact points.
	std::vector<Vec2> contact;
    // Relative values of physical properties of the two bodies, gathered from their PhysMaterials.
	double e, kfric, sfric;


    // Initialize the arbiter. To check for collision and generate data, call build().
    // @param A First body.
    // @param B Second body.
	Arbiter(PhysBody* A, PhysBody* B);
    // Performs a narrow-phase collision between the two bodies and computes the parameters required
    // for a collision response.
	void build();
    // Some edge cases may alter the arbiter parameters for faster convergence.
	void pre_solve();
    // Resolve collision using sequential impulses.
	void solve();
    // Position correction. This should be called after all the bodies' velocities are integrated.
	void post_solve();
};
