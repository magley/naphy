#include "cdrifter.h"
#include "naphy/collision.h"
#include "naphy/scene.h"


#include <stdio.h>

CDrifter::CDrifter() {
}

CDrifter::CDrifter(PhysBody* body) {
	this->body = body;
	this->drift_time = 0;
	this->drift_combo = 0;
	this->sprite = CSprite(SPR_DRIFTER_DOWN_STAND, 0.315);
	this->state = DRIFTER_STATE_STAND;
	this->movedir = DRIFTER_DOWN;
}

void CDrifter::update(const Input* input, Scene* scene) {
	if (body == NULL)
		return;

	//---------------------------------------------------------------------------------------------
	//
	double vel_walk = 100;
	double vel_drift = 500;
	double acc = 50;
	double deacc = 15;
	int drift_time_start = 28;     // How much time does a drift take
	int drift_time_halt = 20;      // After this the drifting stops (you slow down to a stop)
	int drift_time_combo = 15;     // After this you can combo
	int drift_time_combo_end = 4;  // After this you can't combo
	int drift_punished = 0;
	//
	//---------------------------------------------------------------------------------------------

	const int inputx = input->key_down(SDL_SCANCODE_D) - input->key_down(SDL_SCANCODE_A);
	const int inputy = input->key_down(SDL_SCANCODE_S) - input->key_down(SDL_SCANCODE_W);
	const int inputdrift = input->key_press(SDL_SCANCODE_SPACE);

	if (drift_time == 0) {
		// Walking

		if (inputy < 0) {
			body->vel.y -= acc;
			state = DRIFTER_STATE_WALK;
			movedir = DRIFTER_UP;
		} else if (inputy > 0) {
			body->vel.y += acc;
			state = DRIFTER_STATE_WALK;
			movedir = DRIFTER_DOWN;
		} else {
			if (body->vel.y > 0) body->vel.y -= deacc;
			if (body->vel.y < 0) body->vel.y += deacc;

			if (inputy == 0)
				state = DRIFTER_STATE_STOPWALK;

			if (std::abs(body->vel.y) <= deacc) {
				body->vel.y = 0;

				if (std::abs(body->vel.x) <= deacc)
					state = DRIFTER_STATE_WALK;
			}
		}
		if (inputx < 0) {
			body->vel.x -= acc;
			state = DRIFTER_STATE_WALK;
			movedir = DRIFTER_LEFT;
		} else if (inputx > 0) {
			body->vel.x += acc;
			state = DRIFTER_STATE_WALK;
			movedir = DRIFTER_RIGHT;
		} else {
			if (body->vel.x > 0) body->vel.x -= deacc;
			if (body->vel.x < 0) body->vel.x += deacc;

			if (inputy == 0)
				state = DRIFTER_STATE_STOPWALK;

			if (std::abs(body->vel.x) <= deacc) {
				body->vel.x = 0;

				if (std::abs(body->vel.y) <= deacc)
					state = DRIFTER_STATE_STAND;
			}
		}

		// Limit walk speed

		if (body->vel.y > vel_walk) body->vel.y = vel_walk;
		if (body->vel.y < -vel_walk) body->vel.y = -vel_walk;
		if (body->vel.x > vel_walk) body->vel.x = vel_walk;
		if (body->vel.x < -vel_walk) body->vel.x = -vel_walk;
	}

	// Drift start

	if (inputdrift) {
		int can_drift = drift_time == 0;
		int should_punish = drift_time > drift_time_combo;
		int can_combo = drift_time <= drift_time_combo && drift_time >= drift_time_combo_end;

		if (should_punish) {
			drift_punished = 1;
		}

		if (can_drift || (can_combo && !drift_punished)) {
			drift_time = drift_time_start;
			state = DRIFTER_STATE_DRIFTSTART;

			const Vec2 mousepos = Vec2(input->mouse_x, input->mouse_y);
			const Vec2 dir = (mousepos - body->pos).normalized();
			Vec2 driftvec = vel_drift * dir;

			// Prevent clipping by raycasting
			for (const PhysBody* b : scene->body) {
				if (b->dynamic_state == PHYSBODY_STATE_STATIC) {
					const Vec2 ray_A = body->pos;
					const Vec2 ray_B = ray_A + driftvec * scene->timing.dt; // TODO: Ugly dt
					const Arbiter R = raycast(ray_A, ray_B, b);

					if (R.depth > 0) {
						// Move in the axis which dir and R.normal are less parallel in.

						Vec2 delta, vec_dir;
						delta = dir - R.normal;
						delta.x = abs(delta.x);
						delta.y = abs(delta.y);

						if (delta.x > delta.y) vec_dir = {dir.x * 0.95, dir.y * 0.05};
						else vec_dir = {dir.x * 0.05, dir.y * 0.95};
						vec_dir.normalize();

						driftvec = (R.depth + 1.0) * vec_dir / scene->timing.dt; // TODO: Ugly dt
						break;
					}
				}
			}

			body->vel = driftvec;

			if (std::abs(body->vel.y) > std::abs(body->vel.x)) {
				if (body->vel.y > 0)
					movedir = DRIFTER_DOWN;
				else
					movedir = DRIFTER_UP;
			} else {
				if (body->vel.x > 0)
					movedir = DRIFTER_RIGHT;
				else
					movedir = DRIFTER_LEFT;
			}

			if (can_combo || drift_combo == 0)
				drift_combo++;
		}
	}

	// While drifting

	if (drift_time > 0) {
		if (drift_time < drift_time_start && state == DRIFTER_STATE_DRIFTSTART) {
			state = DRIFTER_STATE_DRIFT;
		}

		if (drift_time <= drift_time_halt) {
			body->vel *= 0.8;

			int combo_timeout = drift_time_start - drift_time > drift_time_combo;

			if (combo_timeout)
				drift_combo = 0;
		}

		drift_time--;

		// Finish drifting 

		if (drift_time == 0) {
			state = DRIFTER_STATE_STAND;
			drift_punished = 0;
		}
	}

	// Sprite

	update_sprite(input);
}

void CDrifter::update_sprite(const Input* input) {
	const int inputx = input->key_down(SDL_SCANCODE_D) - input->key_down(SDL_SCANCODE_A);
	const int inputy = input->key_down(SDL_SCANCODE_S) - input->key_down(SDL_SCANCODE_W);
	const int inputdrift = input->key_press(SDL_SCANCODE_SPACE);

	const unsigned spr_mat[4][3] = {
		{SPR_DRIFTER_DOWN_STAND, 	SPR_DRIFTER_DOWN_WALK, 	SPR_DRIFTER_DOWN_DRIFT},
		{SPR_DRIFTER_RIGHT_STAND, 	SPR_DRIFTER_RIGHT_WALK, SPR_DRIFTER_RIGHT_DRIFT},
		{SPR_DRIFTER_UP_STAND, 		SPR_DRIFTER_UP_WALK, 	SPR_DRIFTER_UP_DRIFT},
		{SPR_DRIFTER_RIGHT_STAND, 	SPR_DRIFTER_RIGHT_WALK, SPR_DRIFTER_RIGHT_DRIFT},
	};

	// Determine which sprite to display

	unsigned spr_mat_spr;
	unsigned spr_mat_dir;
	unsigned reset_if_same = 0;

	if (state == DRIFTER_STATE_STAND) {
		spr_mat_spr = 0;
		sprite.repeat = 1;
	} else if (state == DRIFTER_STATE_WALK || state == DRIFTER_STATE_STOPWALK) {
		spr_mat_spr = 1;
		sprite.repeat = 1;
	} else if (state == DRIFTER_STATE_DRIFTSTART || state == DRIFTER_STATE_DRIFT) {
		spr_mat_spr = 2;
		if (state == DRIFTER_STATE_DRIFTSTART)
			reset_if_same = 1;
		sprite.repeat = 0;
	}

	spr_mat_dir = movedir;

	if (movedir == DRIFTER_LEFT)
		sprite.sdl_flip = SDL_FLIP_HORIZONTAL;
	else 
		sprite.sdl_flip = SDL_FLIP_NONE;

	sprite.set(spr_mat[spr_mat_dir][spr_mat_spr], 1, reset_if_same);
	sprite.update();

	// Trail

	if (state == DRIFTER_STATE_DRIFTSTART || state == DRIFTER_STATE_DRIFT) {
		trails.push_back(body->pos);

		if (trails.size() > 8)
			trails.pop_front();
	} else {
		trails.clear();
	}
}

void CDrifter::draw(const Image* img) const {
	const Vec2 spr_pos_offset = Vec2(
		spr[sprite.sprite_index].size.x / 2,
		spr[sprite.sprite_index].size.y - 2);

	// Draw trail

	uint8_t r_, g_, b_, a_;
	SDL_GetTextureColorMod(img->img, &r_, &g_, &b_);
	SDL_GetTextureAlphaMod(img->img, &a_);
	{
		auto it = trails.begin();
		for (unsigned i = 0; it != trails.end(); ++it, ++i) {
			SDL_SetTextureColorMod(img->img,
								(55 + (50 - 5 * i * i) % 200) % 255,
								(200 - 5 * i * i) % 255,
								(2 * i * i + 8 * i + 160) % 255);

			Vec2 p = *it;
			((CSprite)sprite).draw(p - spr_pos_offset);
		}
	}
	SDL_SetTextureAlphaMod(img->img, a_);
	SDL_SetTextureColorMod(img->img, r_, g_, b_);

	// Draw main sprite

	((CSprite)sprite).draw(body->pos - spr_pos_offset);
}