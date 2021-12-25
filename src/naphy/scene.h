#pragma once
#include "utility/mathutil.h"
#include "physbody.h"
#include "arbiter.h"
#include "quadtree.h"
#include "utility/timing.h"


struct Scene {
	/**
	 * @brief All PhysBody instances registered in the scene.
	 */
	std::vector<PhysBody> body;
	/**
	 * @brief All Arbiter instances registered in the scene.
	 */
	std::vector<Arbiter> arbiter;
	/**
	 * @brief Quad tree spatial indexing data structure for this scene.
	 */
	QuadNode quadtree;
	/**
	 * @brief Size of the scene. Everything outside the bounds + padding
	 * is safe to remove.
	 */
	Vec2 size;

	/**
	 * @brief Handles current program time state for constant framerate.
	 */
	Timing timing;


	/**
	 * @brief Gravity vector. Fine-tuning may be required because of scaling.
	 */
	Vec2 grav;


	/**
	 * @brief Construct a new Scene object.
	 */
	Scene();
	/**
	 * @brief Construct a new Scene object
	 * 
	 * @param grav Gravity.
	 * @param dt Change in time.
	 * @param w Maximum width of the scene. Used for the quad tree.
	 * @param h Maximum height of the scene. Used for the quad tree.
	 * @param quadtree_capacity Capacity of a quadtree node.
	 */
	Scene(Vec2 grav, double dt, double w, double h, unsigned quadtree_capacity);

	/**
	 * @brief Nothing physics-related happens here.
	 */
	void pre_update();
	/**
	 * @brief Updates the whole scene.
	 */
	void update();
	/**
	 * @brief Renders the whole scene.
	 * 
	 * @param rend Pointer to the SDL_Renderer where everything will be drawn.
	 * @param draw_meta [Debug] draw arbiters and quad tree.
	 */
	void draw(SDL_Renderer* rend, bool draw_meta);
	/**
	 * @brief Add a new PhysBody to the scene.
	 * 
	 * @param b The body to add.
	 * @return Index of the object in the array.
	 */
	unsigned add(PhysBody b);
};
