#include "timing.h"
#include "SDL2/SDL_timer.h"
#include "utility/mathutil.h"

Timing::Timing() {
	dt = 1 / 60.0;
	total = 0.0;
	scale = 1.0;
	curr = 0.0;
	accumulator = 0.0;
	ticks = 0;
	ticks_phys = 0;
}

Timing::Timing(double dt) {
	this->dt = dt;
	total = 0.0;
	scale = 1.0;
	curr = 0.0;
	accumulator = 0.0;
	ticks = 0;
	ticks_phys = 0;
}

void Timing::tick() {
	const double time_new = SDL_GetTicks() / 1000.0 * scale;
	const double time_diff = time_new - curr;
	curr = time_new;
	accumulator += time_diff;
	accumulator = clamp(0.0, 0.1, accumulator);
}