#include "scene.h"
#include "rend/rend.h"
#include <set>




static void scene_update_collision(Scene* scene);
static void scene_update_force(Scene* scene);
static void scene_update_force_constraints(Scene* scene);
static void scene_update_impulse_constraints(Scene* scene);
static void scene_update_velocity(Scene* scene);
static void scene_remove_distant_objects(Scene* scene);

static void collision_quadtree(Scene* scene);
static void collision_naive(Scene* scene);


/**
 * @brief Pairs of physics bodies with a '<' operator.
 * Used in ordered sequences to detect duplicates.
 */
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
	quadtree = QuadNode(Vec2(0, 0), Vec2(320, 240), 0);
	size = Vec2(320, 240);
}


Scene::Scene(Vec2 grav, double dt, double w, double h, unsigned quadtree_cap) {
	timing = Timing(dt);
	this->grav = grav;
	this->quadtree = QuadNode(Vec2(0, 0), Vec2(w, h), quadtree_cap);
	this->size = Vec2(w, h);
}


Scene::~Scene() {
	for (auto o : body) delete o;
	body.clear();
}


void Scene::pre_update() {
	timing.tick();
}


void Scene::update() {
	while (timing.accumulator >= timing.dt) {
		///////////////////////////////////////

		scene_update_collision(this);
		scene_update_force_constraints(this);
		scene_update_force(this);
		scene_update_impulse_constraints(this);
		scene_update_velocity(this);
		for (unsigned i = 0; i < arbiter.size(); i++) {
			arbiter[i].post_solve();
		}

		///////////////////////////////////////
	
	
		timing.accumulator -= timing.dt;
		timing.total += timing.dt / timing.scale;
		timing.ticks_phys++;
	}
	timing.ticks++;

	scene_remove_distant_objects(this);
}


void Scene::draw(SDL_Renderer* rend) {
	if (debug_draw_shapes) 
	{
		SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
		for (unsigned i = 0; i < body.size(); i++) {
			body[i]->draw(rend);
		}
	}

	if (debug_draw_arbiters)
	{
		SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
		for (unsigned i = 0; i < arbiter.size(); i++) {
			for (unsigned j = 0; j < arbiter[i].contact.size(); j++) {
				const Vec2& cp = arbiter[i].contact[j];
				const Vec2& n = arbiter[i].normal;

				draw_circle_filled(rend, cp.x, cp.y, 5);
				draw_arrow(rend, cp.x, cp.y, cp.x + n.x * 30, cp.y + n.y * 30);
			}
		}
	}

	if (debug_draw_quadtree)
	{
		std::vector<QuadNode*> quadtree_nodes = quadtree.get_all_nodes();
		SDL_SetRenderDrawColor(rend, 255, 255, 0, 50);
		for (unsigned i = 0; i < quadtree_nodes.size(); i++) {
			QuadNode* qn = quadtree_nodes[i];
			SDL_FRect r{ 
				(float)qn->pos.x, 
				(float)qn->pos.y, 
				(float)qn->size.x, 
				(float)qn->size.y 
			};
			SDL_RenderDrawRectF(rend, &r);
		}
	}

	if (debug_draw_springs)
	{
		for (unsigned i = 0; i < spring.size(); i++) {
			Spring* s = &spring[i];
			SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
			SDL_RenderDrawLineF(rend, s->A->pos.x, s->A->pos.y, s->B->pos.x, s->B->pos.y);

			const Vec2 dpos = s->A->pos - s->B->pos;
			const Vec2 C = s->A->pos - std::min(dpos.len(), s->rest_length) * (dpos).normalized();
			SDL_SetRenderDrawColor(rend, 0, 255, 0, 255);
			SDL_RenderDrawLineF(rend, s->A->pos.x, s->A->pos.y, C.x, C.y);
		}
	}



	SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
}


PhysBody* Scene::add(PhysBody* b) {
	body.push_back(b);
	return b;
}


//==============================================================================
// Helper functions


static void collision_quadtree(Scene* scene) {
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

				// If neither body is awake, don't do collision.

				if (A->dynamic_state != PHYSBODY_STATE_AWAKE 
				&& B->dynamic_state != PHYSBODY_STATE_AWAKE)
					continue;

				// Avoid duplicate checking.
				// It's a waste of CPU and duplicate arbiters mess up impulses.

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

static void collision_naive(Scene* scene) {
	for (unsigned i = 0; i < scene->body.size(); i++) {
		PhysBody* A = (scene->body[i]);
		for (unsigned j = i + 1; j < scene->body.size(); j++) {
			PhysBody* B = (scene->body[j]);

			// If neither body is awake, don't do collision.

			if (A->dynamic_state != PHYSBODY_STATE_AWAKE 
			&& B->dynamic_state != PHYSBODY_STATE_AWAKE)
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

static void scene_update_collision(Scene* scene) {
	scene->arbiter.clear();
	scene->quadtree.clear();

	if (scene->debug_use_quadtree){
		collision_quadtree(scene);
	} else {
		collision_naive(scene);
	}
}

static void scene_update_force(Scene* scene) {
	for (unsigned i = 0; i < scene->body.size(); i++) {
		PhysBody* b = scene->body[i];

		if (b->dynamic_state == PHYSBODY_STATE_STATIC)
			continue;
		
		Vec2 dv = (b->force * b->m_inv + scene->grav) * scene->timing.dt;
		double dang = (b->torque * b->I_inv) * scene->timing.dt;

		if (b->dynamic_state == PHYSBODY_STATE_SLEEPING) {
			if (b->m_inv != 0 && dv.len_sqr() + b->vel.len_sqr() > 0.01)
				b->dynamic_state = PHYSBODY_STATE_AWAKE;
			if (b->I_inv != 0 && std::abs(dang) + std::abs(b->angvel) > 0.01)
				b->dynamic_state = PHYSBODY_STATE_AWAKE;
		}	

		if (b->dynamic_state == PHYSBODY_STATE_AWAKE) {
			if (b->m_inv != 0) {
				b->vel += dv;
			}

			if (b->I_inv != 0) {
				b->angvel += dang;
			}
		}
	}
}


static void scene_update_force_constraints(Scene* scene) {
	for (unsigned i = 0; i < scene->spring.size(); i++) {
		scene->spring[i].solve();
	}
}

static void scene_update_impulse_constraints(Scene* scene) {
	for (unsigned i = 0; i < scene->arbiter.size(); i++)
		scene->arbiter[i].pre_solve();

	for (unsigned j = 0; j < 10; j++) {
		for (unsigned i = 0; i < scene->arbiter.size(); i++)
			scene->arbiter[i].solve();
	}
}

static void scene_update_velocity(Scene* scene) {
	for (unsigned i = 0; i < scene->body.size(); i++) {
		PhysBody* b = scene->body[i];

		if (b->dynamic_state != PHYSBODY_STATE_AWAKE)
			continue;

		if (b->vel.len_sqr() <= 0.01 && std::abs(b->angvel) <= 0.01) {
			b->dynamic_state = PHYSBODY_STATE_SLEEPING;
			b->vel = Vec2(0, 0);
			b->angvel = 0;
		} else {
			if (b->m_inv != 0) {
				b->pos += b->vel * scene->timing.dt;
			}

			if (b->I_inv != 0) {
				b->ang += b->angvel * scene->timing.dt;
				b->set_angle(b->ang);
			}
		}

		b->force = {0, 0};
		b->torque = 0;
	}
}

static void scene_remove_distant_objects(Scene* scene) {
	const double padding = 32;
	const Vec2 my_ul = Vec2(0, 0) - Vec2(padding, padding);
	const Vec2 my_dr = my_ul + scene->size + 2 * Vec2(padding, padding);

	for (int i = scene->body.size() - 1; i >= 0; i--) {
		PhysBody* b = scene->body[i];
		if (!b->bbox_query(my_ul, my_dr)) {
			// TODO : finish this
			// TODO : remove all arbiters and springs that are involved with this body
		}
	}
}
