#pragma once
#include "utility/mathutil.h"
#include "physbody.h"
#include "arbiter.h"
#include "quadtree.h"
#include "spring.h"
#include "utility/timing.h"
#include "game/cdrifter.h"
#include "utility/input.h"

struct PhysScene {
	// All PhysBody instances registered in the scene.
	std::vector<PhysBody*> body;
	// All Arbiter instances registered in the scene.
	std::vector<Arbiter> arbiter;
	// All Spring instances registered in the scene.
	std::vector<Spring> spring;
	// Quad tree spatial indexing data structure for this scene.
	QuadNode quadtree;
	// Stores time, frame count etc. Used to maintain a fixed framerate.
	Timing timing;
	// Gravity vector. Fine-tuning may be required because of scaling.
	Vec2 grav;
	// This is just for reference. Changing this won't change the window size.
	Vec2 win_size;
	// Also used for the quad tree.
	Vec2 view_size;
	// Renderer.
	SDL_Renderer* rend;

	~PhysScene();



	bool debug_draw_shapes;
	bool debug_draw_arbiters;
	bool debug_draw_quadtree;
	bool debug_draw_springs;
	bool debug_use_quadtree;


	// Construct a new Scene object.
	PhysScene();
	// Construct a new Scene object
	PhysScene(SDL_Renderer* rend, double dt, Vec2 win_size, Vec2 view_size, Vec2 grav, unsigned quadtree_cap);

	// Update clock.
	// Nothing physics-related happens here.
	void pre_update();
	// Updates the whole scene.
	void update();
	// Renders the whole scene.
	void draw();
	// @brief Add a new PhysBody to the scene.
	// @param b The body to add.
	// @return The pointer to the body.
	PhysBody* add(PhysBody* b);
	// Removes all PhysBodies, Arbiters and Springs from the scene and frees memory.
	void clear();
};
