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


	/**
	 * @brief Gravity vector. Fine-tuning may be required because of scaling.
	 */
	Vec2 grav;
	/**
	 * @brief Delta time, should be equal to the inverse framerate.
	 */
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
	 * @return Pointer to the PhysBody instance inside 'body'.
	 * Note that a copy of 'b' is inserted into the array, not
	 * the actual instance of 'b', therefore this function does
	 * NOT return a pointer to 'b'. 
	 */
	PhysBody* add(PhysBody b);
};
