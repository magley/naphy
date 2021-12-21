#pragma once
#include "utility/mathutil.h"
#include "physbody.h"
#include "arbiter.h"


struct Scene {
	/**
	 * @brief All PhysBody instances registered in the scene.
	 */
	std::vector<PhysBody> body;
	/**
	 * @brief All Arbiter instances registered in the scene.
	 */
	std::vector<Arbiter> arbiter;


	Vec2 grav;
	double dt;


	/**
	 * @brief Construct a new Scene object.
	 */
	Scene();
	/**
	 * @brief Construct a new Scene object
	 * 
	 * @param grav Gravity.
	 * @param dt Change in time.
	 */
	Scene(Vec2 grav, double dt);
	/**
	 * @brief Updates the whole scene.
	 */
	void update();
	/**
	 * @brief Renders the whole scene.
	 * 
	 * @param rend Pointer to the SDL_Renderer on which everything will be rendered.
	 */
	void render(SDL_Renderer* rend);
	/**
	 * @brief Add a new PhysBody to the scene.
	 * 
	 * @param b The body to add.
	 * @return PhysBody* Pointer to the body inside the vector.
	 * Note that the return value is not equal to &b.
	 */
	PhysBody* add(PhysBody b);
};
