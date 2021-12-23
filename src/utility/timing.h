#pragma once


/**
 * @brief Timing object, capturing total time, total frames (both physics frames and regular frames) and accumulator to maintain constant framerate.
 * Note that this structure doesn't actually do anything to keep a constant framerate, it just gathers data.
 * See Scene::update().
 */
struct Timing {
	/**
	 * @brief Delta time, equal to the inverse framerate.
	 */
	double dt;
	/**
	 * @brief Elapsed time (in seconds)
	 */
	double total;
	/**
	 * @brief Scale (used to speed up or slow down everything).
	 */
	double scale;
	/**
	 * @brief Current time (in ms).
	 */
	double curr;
	/**
	 * @brief Accumulator, used to maintain a fixed timestep.
	 */
	double accumulator;
	/**
	 * @brief Number of game loop frames.
	 */
	unsigned ticks;
	/**
	 * @brief Number of physics loop frames.
	 */
	unsigned ticks_phys;

	/**
	 * @brief Construct a new Timing object.
	 * 
	 * @param dt Delta time, equal to the inverse framerate.
	 * If unsure, use 1 / 60.0.
	 */
	Timing(double dt);
	Timing();

	/**
	 * @brief Update the timing object.
	 * 
	 */
	void tick();
};