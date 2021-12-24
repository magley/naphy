#include "scene.h"
#include "rend/rend.h"
#include <set>


struct PhysBodyPair {
	PhysBody* A;
	PhysBody* B;

	bool operator <(const PhysBodyPair& other) const {
		if (A < other.A)
			return true;
		else if (A == other.A)
			return B < other.B;
		else
			return false;
	}
};


Scene::Scene() {
	timing = Timing(1 / 60.0);
	grav = Vec2(0, 400);
	quadtree = QuadNode(Vec2(0, 0), Vec2(0, 0), 0);
}


Scene::Scene(Vec2 grav, double dt, double w, double h, unsigned quadtree_capacity) {
	timing = Timing(dt);
	this->grav = grav;
	this->quadtree = QuadNode(Vec2(0, 0), Vec2(w, h), quadtree_capacity);
}


static void scene_update_collision(Scene* scene);
static void scene_update_force(Scene* scene);
static void scene_update_constraints(Scene* scene);
static void scene_update_velocity(Scene* scene);


void Scene::pre_update() {
	timing.tick();
}


void Scene::update() {
	while (timing.accumulator >= timing.dt) {
		///////////////////////////////////////

		scene_update_collision(this);
		scene_update_force(this);
		scene_update_constraints(this);
		scene_update_velocity(this);
		for (unsigned i = 0; i < arbiter.size(); i++) {
			arbiter[i].post_solve();
		}

		///////////////////////////////////////
	
	
		timing.accumulator -= timing.dt;
		timing.total += timing.dt / timing.scale;
		timing.ticks_phys++;
	}
}


void Scene::render(SDL_Renderer* rend, bool draw_meta) {
	SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
	for (unsigned i = 0; i < body.size(); i++) {
		body[i].draw(rend);
	}

	if (draw_meta) {
		SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
		for (unsigned i = 0; i < arbiter.size(); i++) {
			for (unsigned j = 0; j < arbiter[i].contact.size(); j++) {
				draw_circle_filled(rend, arbiter[i].contact[j].x, arbiter[i].contact[j].y, 5);
			}
		}

		std::vector<QuadNode*> quadtree_nodes = quadtree.get_all_nodes();
		SDL_SetRenderDrawColor(rend, 255, 255, 0, 50);
		for (unsigned i = 0; i < quadtree_nodes.size(); i++) {
			QuadNode* qn = quadtree_nodes[i];
			SDL_FRect r{ (float)qn->pos.x, (float)qn->pos.y, (float)qn->size.x, (float)qn->size.y };
			SDL_RenderDrawRectF(rend, &r);
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
	scene->quadtree.clear();
	scene->quadtree.build(&scene->body);
	const std::vector<QuadNode*> quad_groups = scene->quadtree.get_leaves();
	std::set<PhysBodyPair> checked_pairs;

	for (unsigned k = 0; k < quad_groups.size(); k++) {
		// Broad-phase

		std::vector<PhysBody*>* body_group = &quad_groups[k]->object;

		for (unsigned i = 0; i < body_group->size(); i++) {
			PhysBody* A = (*body_group)[i];
			for (unsigned j = i + 1; j < body_group->size(); j++) {
				PhysBody* B = (*body_group)[j];

				if (A->dynamic_state == PHYSBODY_STATE_SLEEPING && B->dynamic_state == PHYSBODY_STATE_SLEEPING)
				continue;

				// Avoid duplicate checking.
				// It's a waste of CPU, also duplicate arbiters mess up impulses.

				PhysBodyPair p = { A, B };
				if (checked_pairs.find(p) != checked_pairs.end())
					continue;
				checked_pairs.insert(p);

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
	
}
static void scene_update_force(Scene* scene) {
	for (unsigned i = 0; i < scene->body.size(); i++) {
		PhysBody* b = &scene->body[i];

		if (b->m_inv == 0)
			continue;

		const Vec2 dv = (b->force * b->m_inv + scene->grav) * scene->timing.dt;
		double dang = (b->torque * b->I_inv) * scene->timing.dt;

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
			b->pos += b->vel * scene->timing.dt;
			b->ang += b->angvel * scene->timing.dt;
			b->set_angle(b->ang);
		}

		b->force = {0, 0};
		b->torque = 0;
	}
}