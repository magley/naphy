#include "spring.h"
#include "physbody.h"


Spring::Spring(PhysBody* A, PhysBody* B, double d, double k, double c) {
	this->A = A;
	this->B = B;
	this->rest_length = d;
	this->k = k;
	this->c = c;
}


void Spring::solve() {
	if (A->dynamic_state == PHYSBODY_STATE_STATIC && B->dynamic_state == PHYSBODY_STATE_STATIC)
		return;

	const Vec2 dp = B->pos - A->pos;
	const double dist = dp.len();
	const Vec2 dp_norm = dp.normalized();
	const Vec2 p = dp_norm * dist;
	const double x = dist - rest_length;
	const Vec2 px = (p - x * dp);
	const Vec2 dv_lin = B->vel - A->vel;
	const Vec2 dv_ang = cross(B->angvel - A->angvel, dp_norm);
	const Vec2 dv = dv_lin + dv_ang;
	
	const Vec2 F_lin = (-k * x) * dp - (c * dv);
	const double F_ang = cross(F_lin, dp_norm);

	A->force -= F_lin;
	B->force += F_lin;

	A->torque -= F_ang;
	B->torque += F_ang;
}