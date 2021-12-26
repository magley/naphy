#pragma once
#include "utility/mathutil.h"
#include <vector>


struct PhysBody;


/**
 * @brief Structure that stores information about collision between two PhysBody instances.
 */
struct Arbiter {
    // Colliding body
	PhysBody *A, *B;
    /**
     * @brief Intersection depth.
     */
	double depth;
    /**
     * @brief Collision normal.
     */
	Vec2 normal;
    /**
     * @brief Array of contact points.
     */
	std::vector<Vec2> contact;
    /**
     * @brief Relative physical value of the corresponding PhysBody property.
     */
	double e, kfric, sfric;


    /**
     * @brief Initialize arbiter fields.
     * 
     * @param A First body.
     * @param B Second body.
     */
	Arbiter(PhysBody* A, PhysBody* B);
    /**
     * @brief An arbiter is built by performing a narrow-phase collision between the two bodies
     * and computing the parameters required to initiate collision resolution.
     */
	void build();
    /**
     * @brief Some edge cases may alter the arbiter parameters for faster convergence.
     */
	void pre_solve();
    /**
     * @brief Resolve collision using the sequential impulses method.
     */
	void solve();
    /**
     * @brief Position correction. This should be called after all the velocities are integrated.
     */
	void post_solve();
};
