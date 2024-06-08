#include "arbiter.h"
#include "collision.h"
#include "physbody.h"
#include <iostream>

// Apply impulse on a body. Static bodies are ignored.
// @param body The target body.
// @param impulse Vector of impulse.
// @param r Difference vector from body->pos to the impulse point.
static void apply_impulse(PhysBody *body, const Vec2 &impulse, const Vec2 &r) {
    if (body->dynamic_state == PHYSBODY_STATE_STATIC)
        return;

    body->vel += body->m_inv * impulse;
    body->angvel += body->I_inv * cross(r, impulse);
}

Arbiter::Arbiter(PhysBody *A, PhysBody *B) {
    this->A = A;
    this->B = B;

    depth  = 0;
    normal = Vec2(0, 0);
    e      = 0;
    kfric  = 0;
    sfric  = 0;
}

void Arbiter::build() {
    collision(this);
    e     = std::max(A->material.e, B->material.e);
    sfric = std::sqrt(A->material.sfric * B->material.sfric);
    kfric = std::sqrt(A->material.kfric * B->material.kfric);
}

void Arbiter::pre_solve(const Vec2 &grav, double dt) {
    if (A->aggregate_state != PHYSBODY_SOLID || B->aggregate_state != PHYSBODY_SOLID)
        return;

    const Vec2 Apos = A->pos;
    const Vec2 Bpos = B->pos;

    for (unsigned i = 0; i < contact.size(); ++i) {
        const Vec2 r1 = contact[i] - Apos;
        const Vec2 r2 = contact[i] - Bpos;
        const Vec2 dv = (B->vel + cross(B->angvel, r2)) - (A->vel + cross(A->angvel, r1));

        if (dv.len_sqr() < (dt * grav).len_sqr() + EPSILON) {
            e = 0;
            break;
        }
    }
}

void Arbiter::post_solve() {
    // Bias-slop correction mechanism.
    // The parameters slop and bias may need fine-tuning.

    if (A->dynamic_state == PHYSBODY_STATE_STATIC && B->dynamic_state == PHYSBODY_STATE_STATIC)
        return;
    if (A->aggregate_state != PHYSBODY_SOLID || B->aggregate_state != PHYSBODY_SOLID)
        return;

    const double slop       = 0.07f;   // How much clipping is allowed
    const double bias       = 0.6f;    // How much to push the body backwards
    const Vec2   correction = (std::max(depth - slop, 0.0) / (A->m_inv + B->m_inv)) * normal * bias;

    if (A->m_inv != 0)
        A->pos -= correction * A->m_inv;
    if (B->m_inv != 0)
        B->pos += correction * B->m_inv;
}

void Arbiter::solve() {
    if (A->m_inv == 0 && B->m_inv == 0)
        return;
    if (A->dynamic_state == PHYSBODY_STATE_STATIC && B->dynamic_state == PHYSBODY_STATE_STATIC)
        return;
    if (A->aggregate_state != PHYSBODY_SOLID || B->aggregate_state != PHYSBODY_SOLID)
        return;
    if (depth < EPSILON)
        return;

    double Pn_total = 0.0;

    for (unsigned i = 0; i < contact.size(); ++i) {
        /* (1) normal impulse */

        const Vec2   r1  = contact[i] - A->pos;
        const Vec2   r2  = contact[i] - B->pos;
        Vec2         dv  = (B->vel + cross(B->angvel, r2)) - (A->vel + cross(A->angvel, r1));
        const double dvn = dot(dv, normal);

        if (dvn > 0)
            break;

        const Vec2   pa     = A->pos - r1;
        const Vec2   pb     = B->pos - r2;
        const double d      = dot(pb - pa, normal);
        const double d_slop = 0.1;
        const double b      = 0.01 * d / (60);

        const double r1n         = cross(r1, normal);
        const double r2n         = cross(r2, normal);
        const double m_effective = (A->m_inv + r1n * r1n * A->I_inv) + (B->m_inv + r2n * r2n * B->I_inv);

        double Pn = -(1.0 + e + b) * dvn / m_effective / contact.size();

        if (std::abs(Pn) < EPSILON)
            break;

        const double Pn_temp = Pn_total;
        Pn_total             = std::max(Pn_temp + Pn, 0.0);
        Pn                   = Pn_total - Pn_temp;

        const Vec2 PN = normal * Pn;
        apply_impulse(A, -PN, r1);
        apply_impulse(B, PN, r2);

        /* (2) tangent impulse */

        dv                 = (B->vel + cross(B->angvel, r2)) - (A->vel + cross(A->angvel, r1));
        const Vec2 tangent = (dv - (normal * dot(dv, normal))).normalized();

        double Pt = -(1.0) * dot(dv, tangent) / m_effective / contact.size();

        if (std::abs(Pt) < EPSILON)
            break;

        if (std::abs(Pt) > sfric * std::abs(Pn))
            Pt = -kfric * std::abs(Pn);
        else
            Pt = 0;

        const Vec2 PT = tangent * Pt;
        apply_impulse(A, -PT, r1);
        apply_impulse(B, PT, r2);
    }
}