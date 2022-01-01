#include "cdrifter.h"





CDrifter::CDrifter() {
	this->body = NULL;
	this->drift_time = 0;
	this->drift_combo = 0;
	this->sprite = CSprite(SPR_DRIFTER_DOWN_STAND, 0.75 / 60.0);
}

CDrifter::CDrifter(PhysBody* body) {
	this->body = body;
	this->drift_time = 0;
	this->drift_combo = 0;
	this->sprite = CSprite(SPR_DRIFTER_DOWN_STAND, 0.75 / 60.0);
}

void CDrifter::update(const Input* input) {
	if (body == NULL)
		return;

	//---------------------------------------------------------------------------------------------
	//
	double vel_walk = 150;
	double vel_drift = 500;
	double vel_ski = 300;
	double acc = 5;
	double deacc = 1;
	int drift_time_start = 400; 		// How much time does a drift take
	int drift_time_halt = 250; 			// After this the drifting stops (you slow down to a stop)
	int drift_time_combo = 180;			// After this you can combo
	int drift_time_combo_end = 50;		// After this you can't combo
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
			sprite.sprite_index = SPR_DRIFTER_UP_WALK;
		} else if (inputy > 0) {
			body->vel.y += acc;
			sprite.sprite_index = SPR_DRIFTER_DOWN_WALK;
		} else {
			if (body->vel.y > 0) body->vel.y -= deacc;
			if (body->vel.y < 0) body->vel.y += deacc;
			if (std::abs(body->vel.y) <= deacc) {

				if (body->vel.x == 0) {
					if (body->vel.y < 0) sprite.sprite_index = SPR_DRIFTER_UP_STAND;
					else if (body->vel.y > 0) sprite.sprite_index = SPR_DRIFTER_DOWN_STAND;
				}
				
				body->vel.y = 0;
			}
		}
		if (inputx < 0) {
			body->vel.x -= acc;
			sprite.sprite_index = SPR_DRIFTER_LEFT_WALK;
		} else if (inputx > 0) {
			body->vel.x += acc;
			sprite.sprite_index = SPR_DRIFTER_RIGHT_WALK;
		} else {
			if (body->vel.x > 0) body->vel.x -= deacc;
			if (body->vel.x < 0) body->vel.x += deacc;
			if (std::abs(body->vel.x) <= deacc) {

				if (body->vel.y == 0) {
					if (body->vel.x < 0) sprite.sprite_index = SPR_DRIFTER_LEFT_STAND;
					else if (body->vel.x > 0) sprite.sprite_index = SPR_DRIFTER_RIGHT_STAND;
				}

				body->vel.x = 0;
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

			const Vec2 mousepos = Vec2(input->mouse_x, input->mouse_y);
			const Vec2 dir = (mousepos - body->pos).normalized();
			body->vel = vel_drift * dir;

			if (can_combo || drift_combo == 0)
				drift_combo++;
		}
	}

	// While drifting

	if (drift_time > 0) {
		if (drift_time <= drift_time_halt) {
			body->vel *= 0.975;

			int combo_timeout = drift_time_start - drift_time > drift_time_combo;

			if (combo_timeout)
				drift_combo = 0;
		}

		drift_time--;

		// End of drifting

		if (drift_time == 0) {
			drift_punished = 0;
		}
	}	


	// Sprite

	sprite.update();
}


void CDrifter::draw(const Image* img) const {
	Vec2 spr_pos = Vec2(
		body->pos.x - spr[sprite.sprite_index].size.x / 2,
		body->pos.y - spr[sprite.sprite_index].size.y
	);

	((CSprite)sprite).draw(spr_pos);
}