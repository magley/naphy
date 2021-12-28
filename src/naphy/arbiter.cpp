#include "arbiter.h"
#include "physbody.h"
#include "collision.h"
#include <iostream>




// Apply impulse on a body. Static bodies are ignored.
// @param body The target body.
// @param impulse Vector of impulse.
// @param r Difference vector from body->pos to the impulse point. 
static void apply_impulse(PhysBody* body, const Vec2& impulse, const Vec2& r) {
	if (body->dynamic_state == PHYSBODY_STATE_STATIC)
		return;

	body->vel	 += body->m_inv * impulse;
	body->angvel += body->I_inv * cross(r, impulse);
}


Arbiter::Arbiter(PhysBody* A, PhysBody* B) {
	this->A = A;
	this->B = B;

	depth = 0;
	normal = Vec2(0, 0);
	e = 0;
	kfric = 0;
	sfric = 0;
}


void Arbiter::build() {
	collision(this);
	e = std::max(A->material.e, B->material.e);
	sfric = std::sqrt(A->material.sfric * B->material.sfric);
	kfric = std::sqrt(A->material.kfric * B->material.kfric);
}


void Arbiter::pre_solve() {
	const Vec2 Apos = A->pos;
	const Vec2 Bpos = B->pos;

	for (unsigned i = 0; i < contact.size(); ++i) {
		const Vec2 r1 = contact[i] - Apos;
		const Vec2 r2 = contact[i] - Bpos;
		const Vec2 dv = (B->vel + cross(B->angvel, r2)) - (A->vel + cross(A->angvel, r1));
	}
}


void Arbiter::post_solve() {
	// Bias-slop correction mechanism.
	// The idea is that sequential impulses may overshoot/undershoot, resulting in bodies clipping.
	// We can push the body in the opposite direction along the normal by an amount proportional to 
	// the two bodies' intersection depth . The parameters slop and bias may need fine-tuning.

	if (A->dynamic_state == PHYSBODY_STATE_STATIC && B->dynamic_state == PHYSBODY_STATE_STATIC)
		return;

	const double slop = 0.01f;	// How much clipping is allowed
	const double bias = 0.6f;	// How much to push the body backwards
	const Vec2 correction = (std::max(depth - slop, 0.0) / (A->m_inv + B->m_inv)) * normal * bias;
	A->pos -= correction * A->m_inv;
	B->pos += correction * B->m_inv;
}



void Arbiter::solve() {
	if (A->dynamic_state == PHYSBODY_STATE_STATIC && B->dynamic_state == PHYSBODY_STATE_STATIC)
		return;
	if (depth < EPSILON)
		return;

	/*
		Accumulated normal impuse.
		The idea is that instead of clamping the normal impulse at each iteration, we
		predict the new total impulse, clamp *that*, and then refresh the iteration
		impulse by subtracting old total impulse and new total impulse. That way, if
		an iteration impulse is negative-large, it won't get clamped to 0, but to the
		negative accumulated impulse.

		Let n be the magnitude of accumulated impulse
		Let q be the magnitude of iteration impulse
		Let {x} denote max(x, 0)

		When does  q != {n + q} - n  ?
		When {n + q} != n + q
		By definition:
			{n + q} = n + q	,	n + q >= 0
						0	,	n + q < 0

		Since we do  n := {n + q}  on each iteration, we know that n >= 0.
		Therefore:  n + q < 0  when  -q > n  i.e. when  q < -n
		-n is negative, so we will clamp the impulse only if iteration impulse 
		is smaller than -n, instead of when iteration impulse is smaller than 0.

		The logic behind this is that we "ignore" only impulses that would not just 
		undo all previous iterations, but also move away from the convergence point.
	*/
	double Pn_total = 0.0;


	for (unsigned i = 0; i < contact.size(); ++i) {
		/* (1) normal impulse */

		const Vec2 r1 = contact[i] - A->pos;
		const Vec2 r2 = contact[i] - B->pos;
		Vec2 dv = (B->vel + cross(B->angvel, r2)) - (A->vel + cross(A->angvel, r1));
		const double dvn = dot(dv, normal);

		if (dvn > 0)
			break;

		const Vec2 pa = A->pos - r1;
		const Vec2 pb = B->pos - r2;
		const double d = dot(pb - pa, normal);
		const double b = -1.0 / (60.0);

		const double r1n = cross(r1, normal);
		const double r2n = cross(r2, normal);
		const double m_effective = (A->m_inv + r1n * r1n * A->I_inv) + (B->m_inv + r2n * r2n * B->I_inv);
		double Pn = -(1.0 + e + b) * dvn / m_effective / contact.size();

		if (std::abs(Pn) < EPSILON)
			break;

		const double Pn_temp = Pn_total;
		Pn_total = std::max(Pn_temp + Pn, 0.0);
		Pn = Pn_total - Pn_temp;

		const Vec2 PN = normal * Pn;
		apply_impulse(A, -PN, r1);
		apply_impulse(B,  PN, r2);

		/* (2) tangent impulse */

		dv = (B->vel + cross(B->angvel, r2)) - (A->vel + cross(A->angvel, r1));
		const Vec2 tangent = (dv - (normal * dot(dv, normal))).normalized();

		double Pt = -(1.0) * dot(dv, tangent) / m_effective / contact.size();
		if (std::abs(Pt) < EPSILON)
			break;

		/* 
			https://www.epj-conferences.org/articles/epjconf/pdf/2021/03/epjconf_pg2021_14005.pdf
			http://www.cs.uu.nl/docs/vakken/mgp/2014-2015/lecture%201-2%20Essential%20Physics.pdf

			See the section on Friction.

			Static friction: object will not move unless tangential gravity is stronger.
			Tangential gravity is just the normal impulse (gravity + normal force).
			In other words: magnitude of friction must be greater than the static friction treshold 
			in order for the object to move normally.

			We know from Coulomb's law that:
				|friction| <= u |normalforce|

			if |friction| > static_friction * |normal_force|:
				Object is moving normally
				Apply kinetic friction
			else:
				Object did not overcome static friction
				Tangential impulse should be 0.

			Kinetic friction: only present when the object is moving.
			Kinetic friction is the one that slows the object down.

			Therefore we can shorten the normal vector and flip its direction (because normal force 
			is positive but kinetic friction damps velocity, and the tangent always forms a positive 
			dot product with velocity:
				friction = -tangent * kinetic_friction * |normal_force|
		*/

		if (std::abs(Pt) >= sfric * std::abs(Pn)) {
			Pt = -kfric * std::abs(Pn);
		} else {
			Pt = 0;
		}

		const Vec2 PT = tangent * Pt;
		apply_impulse(A, -PT, r1);
		apply_impulse(B,  PT, r2);
	}
}