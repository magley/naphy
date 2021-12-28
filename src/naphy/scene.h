#pragma once
#include "utility/mathutil.h"
#include "physbody.h"
#include "arbiter.h"
#include "quadtree.h"
#include "spring.h"
#include "utility/timing.h"


struct Scene {
	// All PhysBody instances registered in the scene.
	std::vector<PhysBody*> body;
	// All Arbiter instances registered in the scene.
	std::vector<Arbiter> arbiter;
	// All Spring instances registered in the scene.
	std::vector<Spring> spring;
	// Quad tree spatial indexing data structure for this scene.
	QuadNode quadtree;
	// Size of the scene. Everything outside the bounds + padding is safe to remove.
	Vec2 size;
	// Stores time, frame count etc. Used to maintain a fixed framerate.
	Timing timing;
	// Gravity vector. Fine-tuning may be required because of scaling.
	Vec2 grav;

	~Scene();


	bool debug_draw_shapes;
	bool debug_draw_arbiters;
	bool debug_draw_quadtree;
	bool debug_draw_springs;
	bool debug_use_quadtree;


	// Construct a new Scene object.
	Scene();
	// Construct a new Scene object
	// @param grav Gravity.
	// @param dt Change in time.
	// @param w Maximum width of the scene. Used for the quad tree.
	// @param h Maximum height of the scene. Used for the quad tree.
	// @param quadtree_capacity Capacity of a quadtree node.
	Scene(Vec2 grav, double dt, double w, double h, unsigned quadtree_capacity);

	// Nothing physics-related happens here.
	void pre_update();
	// Updates the whole scene.
	void update();
	// Renders the whole scene.
	// @param rend Pointer to the SDL_Renderer where everything will be drawn.
	void draw(SDL_Renderer* rend);
	// @brief Add a new PhysBody to the scene.
	// @param b The body to add.
	// @return The pointer to the body.
	PhysBody* add(PhysBody* b);
	// Removes all PhysBodies, Arbiters and Springs from the scene and frees memory.
	void clear();
};
