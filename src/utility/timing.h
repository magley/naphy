#pragma once


// Timing object, capturing total time, total frames (both physics frames and regular frames) and an
// accumulator to maintain constant framerate. Note that this structure isn't in charge of keeping a 
// constant framerate, it's merely an abstraction to a bunch of time-related variables.
// Uses SDL to capture ticks.
// See Scene::update().
struct Timing {
	// Delta time, equal to the inverse framerate.
	double dt;
	// Elapsed time (in seconds)
	double total;
	// Scale (1 = normal speed, 2 = double speed, etc.).
	double scale;
	// Current time (in ms).
	double curr;
	// Accumulator, used to maintain a fixed timestep.
	double accumulator;
	// Number of game loop frames. For physics frames, see ticks_phys.
	unsigned ticks;
	// Number of physics loop frames. For game frames, see ticks.
	unsigned ticks_phys;

	// Construct a new Timing object./
	// @param dt Delta time, equal to the inverse framerate. If unsure, use 1 / 60.0.
	Timing(double dt);
	Timing();

	// Update the timing object (partially). Other members get updated elsewhere (Scene::update()).
	void tick();
};