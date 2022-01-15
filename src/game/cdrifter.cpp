#include "cdrifter.h"
#include "naphy/collision.h"
#include "gamescene.h"
#include <stdio.h>

CDrifter::CDrifter() {
	state = DRIFTER_STATE_STAND;
	movedir = DRIFTER_DOWN;
	drift_time = 0;
	trails.clear();
}

static const double vel_walk = 100;
static const double vel_drift = 500;
static const double acc = 20;
static const double deacc = 10;
static const int drift_time_start = 28;     // How much time does a drift take
static const int drift_time_halt = 20;      // After this the drifting stops (you slow down to a stop)
static const int drift_time_combo = 15;     // After this you can combo
static const int drift_time_combo_end = 4;  // After this you can't combo

void CDrifter::move_and_drift(CPhys* phys, const Input* input) {
	PhysBody* const body = phys->body;

	//---------------------------------------------------------------------------------------------

	const int input_x = input->key_down(SDL_SCANCODE_D) - input->key_down(SDL_SCANCODE_A);
	const int input_y = input->key_down(SDL_SCANCODE_S) - input->key_down(SDL_SCANCODE_W);
	const int input_drift = input->key_press(SDL_SCANCODE_SPACE);

	//---------------------------------------------------------------------------------------------

	const int can_walk = (drift_time == 0) && (state != DRIFTER_STATE_FALLDIE);

	if (can_walk) 
	{
		if (input_y != 0) {
			int dir_index = (input_y < 0 ? 0 : 1);
			int move_dir[] = {DRIFTER_UP, DRIFTER_DOWN};
			int move_dir_raw[] = {-1, 1};
			
			body->vel.y += acc * move_dir_raw[dir_index];
			movedir = move_dir[dir_index];
			state = DRIFTER_STATE_WALK;
		} else {
			if (body->vel.y > 0) body->vel.y -= deacc;
			if (body->vel.y < 0) body->vel.y += deacc;

			if (input_y == 0) 
				state = DRIFTER_STATE_STOPWALK;

			if (std::abs(body->vel.y) <= deacc) 
				body->vel.y = 0;
		}

		if (input_x != 0) {
			int dir_index = (input_x < 0 ? 0 : 1);
			int move_dir[] = {DRIFTER_LEFT, DRIFTER_RIGHT};
			int move_dir_raw[] = {-1, 1};
			
			body->vel.x += acc * move_dir_raw[dir_index];
			movedir = move_dir[dir_index];
			state = DRIFTER_STATE_WALK;
		} else {
			if (body->vel.x > 0) body->vel.x -= deacc;
			if (body->vel.x < 0) body->vel.x += deacc;

			if (input_x == 0) 
				state = DRIFTER_STATE_STOPWALK;

			if (std::abs(body->vel.x) <= deacc) 
				body->vel.x = 0;
		}

		if (body->vel.y > vel_walk) body->vel.y = vel_walk;
		if (body->vel.y < -vel_walk) body->vel.y = -vel_walk;
		if (body->vel.x > vel_walk) body->vel.x = vel_walk;
		if (body->vel.x < -vel_walk) body->vel.x = -vel_walk;

		if (std::abs(body->vel.y) <= deacc && std::abs(body->vel.x) <= deacc) {
			state = DRIFTER_STATE_STAND;
		}
	}

	//---------------------------------------------------------------------------------------------

	const int is_drifting = (drift_time > 0);

	if (is_drifting)
	{
		if (drift_time <= drift_time_halt) {
			body->vel *= 0.8;
		}

		if (drift_time == 0) {
			state = DRIFTER_STATE_STAND;
		}

		drift_time--;
	}
	
	//---------------------------------------------------------------------------------------------

	const int can_combo = drift_time <= drift_time_combo && drift_time >= drift_time_combo_end; 
	const int can_drift = (drift_time == 0 || can_combo) && (state != DRIFTER_STATE_FALLDIE);

	if (can_drift)
	{
		if (input_drift) {
			drift_time = drift_time_start;
			state = DRIFTER_STATE_DRIFT;

			const Vec2 mousepos = Vec2(input->mouse_x, input->mouse_y);
			const Vec2 dir = (mousepos - body->pos).normalized();
			const Vec2 driftvec = vel_drift * dir;

			body->vel = driftvec;

			unsigned new_dir[2][2] = {
				{ DRIFTER_UP, DRIFTER_DOWN},
				{ DRIFTER_LEFT, DRIFTER_RIGHT}
			};

			unsigned new_dir_xy = std::abs(body->vel.y) > std::abs(body->vel.x) ? 0 : 1;
			unsigned new_dir_posneg;
			if (new_dir_xy == 0) new_dir_posneg = body->vel.y < 0 ? 0 : 1;
			else new_dir_posneg = body->vel.x < 0 ? 0 : 1;

			movedir = new_dir[new_dir_xy][new_dir_posneg];
		}
	}

	//---------------------------------------------------------------------------------------------
}

void CDrifter::fall_in_pits(CPhys* phys) {
	PhysBody* const body = phys->body;

	if (state == DRIFTER_STATE_FALLDIE) {
		body->vel.x *= 0.9;
		body->vel.y += 12;
	}

	for (unsigned i = 0; i < body->cld.size(); i++) {
		PhysBody* other = body->cld[i];

		if (other->layer == LAYER_PIT && drift_time == 0) {
			if (state != DRIFTER_STATE_FALLDIE) {
				body->vel.y = 0;
				state = DRIFTER_STATE_FALLDIE;
			}
		}
	}
}

void CDrifter::update_sprite(const CPhys* phys, CSprite* spr, const Input* input) {
	const PhysBody* const body = phys->body;

	const int inputx = input->key_down(SDL_SCANCODE_D) - input->key_down(SDL_SCANCODE_A);
	const int inputy = input->key_down(SDL_SCANCODE_S) - input->key_down(SDL_SCANCODE_W);
	const int inputdrift = input->key_press(SDL_SCANCODE_SPACE);
	const unsigned spr_mat[4][3] = {
		{SPR_DRIFTER_DOWN_STAND, 	SPR_DRIFTER_DOWN_WALK, 	SPR_DRIFTER_DOWN_DRIFT},
		{SPR_DRIFTER_RIGHT_STAND, 	SPR_DRIFTER_RIGHT_WALK, SPR_DRIFTER_RIGHT_DRIFT},
		{SPR_DRIFTER_UP_STAND, 		SPR_DRIFTER_UP_WALK, 	SPR_DRIFTER_UP_DRIFT},
		{SPR_DRIFTER_RIGHT_STAND, 	SPR_DRIFTER_RIGHT_WALK, SPR_DRIFTER_RIGHT_DRIFT},
	};

	unsigned spr_mat_dir = 0; 		// row in 'spr_mat'
	unsigned spr_mat_spr = 0;		// column in 'spr_mat'
	unsigned reset_if_same = 0;
	int img_index = -1;				// -1 if not fixed

	if (state == DRIFTER_STATE_STAND) {
		spr_mat_spr = 0;
		spr->repeat = 1;
		spr->image_speed = 0.315;
	} else if (state == DRIFTER_STATE_WALK || state == DRIFTER_STATE_STOPWALK) {
		spr_mat_spr = 1;
		spr->repeat = 1;
		spr->image_speed = 0.315;
	} else if (state == DRIFTER_STATE_DRIFT) {
		spr_mat_spr = 2;
		spr->repeat = 0;
		spr->image_speed = 0.315;
		if (drift_time == drift_time_start)
			reset_if_same = 1;
	} else if (state == DRIFTER_STATE_FALLDIE) {
		spr_mat_spr = 2;
		img_index = 6;
		spr->image_speed = 0;
	}

	spr_mat_dir = movedir;

	if (movedir == DRIFTER_LEFT) spr->sdl_flip = SDL_FLIP_HORIZONTAL;
	else spr->sdl_flip = SDL_FLIP_NONE;

	spr->set(spr_mat[spr_mat_dir][spr_mat_spr], 1, reset_if_same);
	spr->update();

	if (img_index != -1)
		spr->image_index = img_index;

	// Trail

	if (state == DRIFTER_STATE_DRIFT) {
		trails.push_back(body->pos);
	
		if (trails.size() > 8)
			trails.pop_front();
	} else {
		trails.clear();
	}
}

void CDrifter::draw(const CPhys* phys, const CSprite* sprite, const Image* image, GameScene* scene) const {
	PhysBody* const body = phys->body;

	const Vec2 spr_pos_offset = Vec2(
		spr[sprite->sprite_index].size.x / 2,
		spr[sprite->sprite_index].size.y - 2);

	// Draw trail

	{
		auto it = trails.begin();
		for (unsigned i = 0; it != trails.end(); ++it, ++i) {
			const uint8_t r = (55 + (50 - 5 * i * i) % 200) % 255;
			const uint8_t g = (200 - 5 * i * i) % 255;
			const uint8_t b = (2 * i * i + 8 * i + 160) % 255;
			const Vec2 p = *it;
			scene->draw_sprite(sprite->make_ctx(p - spr_pos_offset, r, g, b));
		}
	}

	// Draw main sprite

	scene->draw_sprite(sprite->make_ctx(body->pos - spr_pos_offset));
}