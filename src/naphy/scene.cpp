#include "scene.h"
#include "rend/rend.h"


Scene::Scene() {
	grav = Vec2(0, 400);
	dt = 1 / 60.0;
}


Scene::Scene(Vec2 grav, double dt) {
	this->grav = grav;
	this->dt = dt;
}


static void scene_update_collision(Scene* scene);
static void scene_update_force(Scene* scene);
static void scene_update_constraints(Scene* scene);
static void scene_update_velocity(Scene* scene);


void Scene::update() {
	scene_update_collision(this);
	scene_update_force(this);
	scene_update_constraints(this);
	scene_update_velocity(this);

	for (unsigned i = 0; i < arbiter.size(); i++) {
		arbiter[i].post_solve();
	}
}


void Scene::render(SDL_Renderer* rend) {
	SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
	for (unsigned i = 0; i < body.size(); i++) {
		body[i].draw(rend);
	}

	SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
	for (unsigned i = 0; i < arbiter.size(); i++) {
		for (unsigned j = 0; j < arbiter[i].contact.size(); j++) {
			draw_circle_filled(rend, arbiter[i].contact[j].x, arbiter[i].contact[j].y, 5);
		}
	}

	SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
}


PhysBody* Scene::add(PhysBody b) {
	body.push_back(b);
	return &body[body.size() - 1];
}


static void scene_update_collision(Scene* scene) {
	scene->arbiter.clear();

	for (unsigned i = 0; i < scene->body.size(); i++) {
		PhysBody* A = &(scene->body[i]);
		for (unsigned j = i + 1; j < scene->body.size(); j++) {
			PhysBody* B = &(scene->body[j]);

			if (A->dynamic_state == PHYSBODY_STATE_SLEEPING && B->dynamic_state == PHYSBODY_STATE_SLEEPING)
				continue;

			// Middle-phase

			if (A->bbox_collision(B) == 0)
				continue;

			// Narrow-phase

			Arbiter a(A, B);
			a.build();
			if (a.contact.size() == 0)
				continue;
			scene->arbiter.push_back(a);
		}
	}
}
static void scene_update_force(Scene* scene) {
	for (unsigned i = 0; i < scene->body.size(); i++) {
		PhysBody* b = &scene->body[i];

		if (b->m_inv == 0)
			continue;

		const Vec2 dv = (b->force * b->m_inv + scene->grav) * scene->dt;
		double dang = (b->torque * b->I_inv) * scene->dt;

		b->vel += dv;
		b->angvel += dang;

		if (std::abs(dang) < 0.05)
			dang = 0;

		if (b->dynamic_state == PHYSBODY_STATE_SLEEPING && dv.len_sqr() > 0.1)
			b->dynamic_state = PHYSBODY_STATE_AWAKE;		

		if (b->dynamic_state == PHYSBODY_STATE_AWAKE) {
			b->vel += dv;
			b->angvel += dang;
		}
	}
}
static void scene_update_constraints(Scene* scene) {
	for (unsigned i = 0; i < scene->arbiter.size(); i++)
		scene->arbiter[i].pre_solve();

	for (unsigned j = 0; j < 10; j++) {
		for (unsigned i = 0; i < scene->arbiter.size(); i++)
			scene->arbiter[i].solve();
	}

	// post_solve() goes after everything else
}
static void scene_update_velocity(Scene* scene) {
	for (unsigned i = 0; i < scene->body.size(); i++) {
		PhysBody* b = &scene->body[i];

		if (b->m_inv == 0)
			continue;
		if (b->dynamic_state != PHYSBODY_STATE_AWAKE)
			continue;

		if (b->vel.len_sqr() <= 0.01) {
			b->dynamic_state = PHYSBODY_STATE_SLEEPING;
			b->vel = Vec2(0, 0);
			b->angvel = 0;
		} else {
			b->pos += b->vel * scene->dt;
			b->ang += b->angvel * scene->dt;
			b->set_angle(b->ang);
		}

		b->force = {0, 0};
		b->torque = 0;
	}
}