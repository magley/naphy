#include "cdrifter.h"
#include <stdio.h>


CDrifter::CDrifter() {
	this->body = NULL;
	this->drift_time = 0;
	this->drift_combo = 0;
	this->sprite = CSprite(SPR_DRIFTER_DOWN_STAND, 1 / 60.0);
	this->trail_i = 0;
	this->trail_cnt = 0;
	this->trail = {};
	this->state = DRIFTER_STATE_STAND;
	this->movedir = DRIFTER_DOWN;
}

CDrifter::CDrifter(PhysBody* body) {
	this->body = body;
	this->drift_time = 0;
	this->drift_combo = 0;
	this->sprite = CSprite(SPR_DRIFTER_DOWN_STAND, 1.75 / 60.0);
	this->trail_i = 0;
	this->trail_cnt = 0;
	this->trail = std::vector<Vec2>(10);
	this->state = DRIFTER_STATE_STAND;
	this->movedir = DRIFTER_DOWN;
}

void CDrifter::update(const Input* input) {
	if (body == NULL)
		return;

	//---------------------------------------------------------------------------------------------
	//
	double vel_walk = 100;
	double vel_drift = 500;
	double acc = 5;
	double deacc = 1.5;
	int drift_time_start = 280; 		// How much time does a drift take
	int drift_time_halt = 200; 			// After this the drifting stops (you slow down to a stop)
	int drift_time_combo = 150;			// After this you can combo
	int drift_time_combo_end = 40;		// After this you can't combo
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
		if (body->vel.y <-vel_walk) body->vel.y =-vel_walk;
		if (body->vel.x > vel_walk) body->vel.x = vel_walk;
		if (body->vel.x <-vel_walk) body->vel.x =-vel_walk;
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
			state = DRIFTER_STATE_DRIFT;

			const Vec2 mousepos = Vec2(input->mouse_x, input->mouse_y);
			const Vec2 dir = (mousepos - body->pos).normalized();
			body->vel = vel_drift * dir;

			if (std::abs(body->vel.y) > std::abs(body->vel.x)) {
				if (body->vel.y > 0) movedir = DRIFTER_DOWN;
				else movedir = DRIFTER_UP;
			} else {
				if (body->vel.x > 0) movedir = DRIFTER_RIGHT;
				else movedir = DRIFTER_LEFT;				
			}

			if (can_combo || drift_combo == 0)
				drift_combo++;
		}
	}

	// While drifting

	if (drift_time > 0) {
		if (drift_time <= drift_time_halt) {
			body->vel *= 0.95;

			int combo_timeout = drift_time_start - drift_time > drift_time_combo;

			if (combo_timeout)
				drift_combo = 0;
		}

		drift_time--;

		// End of drifting

		if (drift_time == 0) {
			state = DRIFTER_STATE_STAND;
			drift_punished = 0;
		}

		// Trail

		if (trail_cnt == 0) {
			// When we start drifting, move all the trail sprites to the current position.
			// This will make sure that none of the trail sprites are rogue.
			for (unsigned i = 0; i < trail.size(); i++)
				trail[i] = body->pos;
			trail_i = 0;
		}

		trail_cnt = trail.size();
	} else {
		// Not drifting.
		trail_cnt = 0;
	}

	// The trail is always updated

	if (trail_i % 5 == 0)
		trail[trail_i / 5] = body->pos;
	if (trail_i++ > trail.size() * 5)
		trail_i = 0;

	// Sprite

	update_sprite(input);
}

void CDrifter::update_sprite(const Input* input) {
	const int inputx = input->key_down(SDL_SCANCODE_D) - input->key_down(SDL_SCANCODE_A);
	const int inputy = input->key_down(SDL_SCANCODE_S) - input->key_down(SDL_SCANCODE_W);
	const int inputdrift = input->key_press(SDL_SCANCODE_SPACE);

	unsigned sprite_matrix[4][3] = {
		{SPR_DRIFTER_DOWN_STAND, SPR_DRIFTER_DOWN_WALK, SPR_DRIFTER_DOWN_DRIFT},
		{SPR_DRIFTER_RIGHT_STAND, SPR_DRIFTER_RIGHT_WALK, SPR_DRIFTER_RIGHT_DRIFT},
		{SPR_DRIFTER_UP_STAND, SPR_DRIFTER_UP_WALK, SPR_DRIFTER_UP_DRIFT},
		{SPR_DRIFTER_RIGHT_STAND, SPR_DRIFTER_RIGHT_WALK, SPR_DRIFTER_RIGHT_DRIFT},
	};

	unsigned spr_mat_spr;
	unsigned spr_mat_dir;
	unsigned reset_if_same = 0;

	if (state == DRIFTER_STATE_STAND) {
		spr_mat_spr = 0;
		sprite.repeat = 1;
	} else if (state == DRIFTER_STATE_WALK || state == DRIFTER_STATE_STOPWALK) {
		spr_mat_spr = 1;
		sprite.repeat = 1;
	} else if (state == DRIFTER_STATE_DRIFT) {
		spr_mat_spr = 2;
		if (inputdrift)
			reset_if_same = 1;
		sprite.repeat = 0;
	}
	spr_mat_dir = movedir;

	if (movedir == DRIFTER_LEFT) sprite.sdl_flip = SDL_FLIP_HORIZONTAL;
	if (movedir == DRIFTER_RIGHT) sprite.sdl_flip = SDL_FLIP_NONE;

	sprite.set(sprite_matrix[spr_mat_dir][spr_mat_spr], 1, reset_if_same);

	sprite.update();
}


void CDrifter::draw(const Image* img) const {
	const Vec2 spr_pos_offset = Vec2(
		spr[sprite.sprite_index].size.x / 2,
		spr[sprite.sprite_index].size.y - 2
	);

	// Draw trail

	uint8_t _r, _g, _b, _a;;
	SDL_GetTextureColorMod(img->img, &_r, &_g, &_b);
	SDL_GetTextureAlphaMod(img->img, &_a);

	for (unsigned i = 0; i < trail_cnt; i++) {
		SDL_SetTextureColorMod(img->img, 
			(50 + i * 70) % 255, 
			(230 - i * 49) % 255, 
			(180 + i * 10) % 255
		);
		((CSprite)sprite).draw(trail[i] - spr_pos_offset);
	}

	SDL_SetTextureColorMod(img->img, _r, _g, _b);
	SDL_SetTextureAlphaMod(img->img, _a);

	// Draw main sprite

	((CSprite)sprite).draw(body->pos - spr_pos_offset);
}