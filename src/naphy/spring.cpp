#include "spring.h"
#include "physbody.h"

Spring::Spring(PhysBody* A, PhysBody* B, double d, double k, double c) {
	this->A = A;
	this->B = B;
	this->rest_length = d;
	this->k = k;
	this->c = c;
}

#include <stdio.h>

void Spring::solve() {
	if (A->dynamic_state == PHYSBODY_STATE_STATIC && B->dynamic_state == PHYSBODY_STATE_STATIC)
		return;


	const Vec2 dp = B->pos - A->pos;
	const Vec2 dv = B->vel - A->vel;
	const Vec2 v = dv.len() * dp.normalized();
	const double x = dp.len() - rest_length;
	const Vec2 F = (-k * x) * dp.normalized() - (c * v);

	A->force -= F;
	B->force += F;
}