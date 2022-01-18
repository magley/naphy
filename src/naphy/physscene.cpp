#include "physscene.h"
#include "rend/rend.h"
#include <set>

static void scene_update_collision(PhysScene* scene);
static void scene_update_force(PhysScene* scene);
static void scene_update_constraints(PhysScene* scene);
static void scene_update_velocity(PhysScene* scene);
static void scene_remove_distant_objects(PhysScene* scene);

static void collision_quadtree(PhysScene* scene);
static void collision_naive(PhysScene* scene);

// Pairs of physics bodies with a '<' operator.
// This is used to avoid duplicates when generating Arbiters during broad-phase collision detection.
// Order matters, so it's up to you to ensure that A < B (when creating Arbiters and PhysBodyPairs).
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

PhysScene::PhysScene() {
	timing = Timing(1 / 60.0);
	rend = NULL;
	grav = Vec2(0, 400);
	quadtree = QuadNode(Vec2(0, 0), Vec2(320, 240), 0);
	win_size = Vec2(320, 240);
	view_size = Vec2(320, 240);
}

PhysScene::PhysScene(SDL_Renderer* rend, double dt, Vec2 win_size, Vec2 view_size, Vec2 grav, unsigned quadtree_cap) {
	timing = Timing(dt);
	this->grav = grav;
	this->rend = rend;
	this->quadtree = QuadNode(Vec2(0, 0), view_size, quadtree_cap);
	this->win_size = win_size;
	this->view_size = view_size;
}

PhysScene::~PhysScene() {
	clear();
}

void PhysScene::pre_update() {
	timing.tick();
}

void PhysScene::update() {
	scene_update_collision(this);
	scene_update_force(this);
	scene_update_constraints(this);
	scene_update_velocity(this);

	for (unsigned i = 0; i < arbiter.size(); i++) {
		arbiter[i].post_solve();
	}
}

void PhysScene::draw() {
	if (debug_draw_shapes) 
	{
		for (unsigned i = 0; i < body.size(); i++) {
			SDL_SetRenderDrawColor(rend, 
				body[i]->material.col.r,
				body[i]->material.col.g,
				body[i]->material.col.b,
				body[i]->material.col.a
			);
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

				draw_circle(rend, cp.x, cp.y, 4);
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

PhysBody* PhysScene::add(PhysBody* b) {
	body.push_back(b);
	return b;
}

void PhysScene::clear() {
	for (auto o : body) { 
		delete o; 
		o = NULL; 
	}
	body.clear();
	arbiter.clear();
	spring.clear();
}


//=================================================================================================
// Helper functions


static void collision_quadtree(PhysScene* scene) {
	scene->quadtree.build(&scene->body);
	const std::vector<QuadNode*> quad_groups = scene->quadtree.get_leaves();
	std::set<PhysBodyPair> checked_pairs;

	for (unsigned k = 0; k < quad_groups.size(); k++) {
		// Broad-phase - check for collision only between bodies in the same quadtree group.

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
				// It's a waste of CPU and duplicate arbiters mess up constraint solvers (why?).
				// Don't swap A and B if A > B, it doesn't work (why?).

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

static void collision_naive(PhysScene* scene) {
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

static void scene_update_collision(PhysScene* scene) {
	scene->arbiter.clear();
	scene->quadtree.clear();

	for (unsigned i = 0; i < scene->body.size(); i++) {
		PhysBody* b = scene->body[i];
		b->cld.clear();
	}

	if (scene->debug_use_quadtree) {
		collision_quadtree(scene);
	} else {
		collision_naive(scene);
	}

	for (unsigned i = 0; i < scene->arbiter.size(); i++) {
		Arbiter* R = &scene->arbiter[i];
		R->A->cld.push_back(R->B);
		R->B->cld.push_back(R->A);
	}
}

static void scene_update_constraints(PhysScene* scene) {
	for (unsigned i = 0; i < scene->arbiter.size(); i++)
		scene->arbiter[i].pre_solve(scene->grav, scene->timing.dt);

	const int iterations = 10;
	for (unsigned j = 0; j < iterations; j++) {
		for (unsigned i = 0; i < scene->arbiter.size(); i++)
			scene->arbiter[i].solve();
	}
}

static void scene_update_force(PhysScene* scene) {
	// Accumulate external forces.
	// For now, we only have springs. Gravity is done below this, when calculating dv.

	for (unsigned j = 0; j < 5; j++) {
		for (unsigned i = 0; i < scene->spring.size(); i++) {
			scene->spring[i].solve();
		}
	}

	// Apply force

	for (unsigned i = 0; i < scene->body.size(); i++) {
		PhysBody* b = scene->body[i];

		if (b->dynamic_state == PHYSBODY_STATE_STATIC)
			continue;

		const Vec2 dv = (b->force * b->m_inv + scene->grav) * scene->timing.dt;
		const double dang = (b->torque * b->I_inv) * scene->timing.dt;

		// Wake up sleeping bodes that moved significantly.

		if (b->dynamic_state == PHYSBODY_STATE_SLEEPING) {
			if (b->m_inv != 0 && dv.len_sqr() + b->vel.len_sqr() > EPSILON)
				b->dynamic_state = PHYSBODY_STATE_AWAKE;
			if (b->I_inv != 0 && std::abs(dang) + std::abs(b->angvel) > EPSILON)
				b->dynamic_state = PHYSBODY_STATE_AWAKE;
		}

		// Move awoken bodies.

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

static void scene_update_velocity(PhysScene* scene) {
	for (unsigned i = 0; i < scene->body.size(); i++) {
		PhysBody* b = scene->body[i];

		if (b->dynamic_state == PHYSBODY_STATE_AWAKE) {
			// If the body didn't move much, put it to sleep.

			const bool vel_is_0 = b->vel.len() <= EPSILON;
			const bool angvel_is_0 = std::abs(b->angvel) <= EPSILON;

			if (vel_is_0) b->vel = {0, 0};
			if (angvel_is_0) b->angvel = 0;

			if (vel_is_0 && angvel_is_0) {
				b->dynamic_state = PHYSBODY_STATE_SLEEPING;
			} else {
				if (b->m_inv != 0) {
					// Euler-Cromer (symplectic Euler)
					//b->pos = b->pos + b->vel * scene->timing.dt;

					// Adams-Bashforth (2-step)
					b->pos = b->pos + (3 * b->vel - b->vel_prev) / 2.0 * scene->timing.dt;
					b->vel_prev = b->vel;
				}
				if (b->I_inv != 0) {
					b->ang += b->angvel * scene->timing.dt;
					b->set_angle(b->ang);
				}
			}
		}

		b->force = {0, 0};
		b->torque = 0;
	}
}

static void scene_remove_distant_objects(PhysScene* scene) {
	const double padding = 32;
	const Vec2 my_ul = Vec2(0, 0) - Vec2(padding, padding);
	const Vec2 my_dr = my_ul + scene->view_size + 2 * Vec2(padding, padding);

	for (int i = scene->body.size() - 1; i >= 0; i--) {
		PhysBody* b = scene->body[i];
		if (!b->bbox_query(my_ul, my_dr)) {
			// TODO: finish this and remove all arbiters and springs attached to this body, too
		}
	}
}