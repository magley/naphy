#include "cjumpnrun.h"

static const double vel_walk = 400;
static const double acc = 30;
static const double deacc = 15;
static const double jumpheight = -500;

void CJumpNRun::jump_n_run(CPhys* phys, const Input* input) {
    PhysBody* const body = phys->body;

    const int input_x = input->key_down(SDL_SCANCODE_RIGHT) - input->key_down(SDL_SCANCODE_LEFT);
	const int input_jump = input->key_press(SDL_SCANCODE_SPACE);

    if (input_x != 0) {
        int dir_index = (input_x < 0 ? 0 : 1);
        int move_dir_raw[] = {-1, 1};
        
        body->vel.x += acc * move_dir_raw[dir_index];
    } else {
        if (body->vel.x > 0) body->vel.x -= deacc;
        if (body->vel.x < 0) body->vel.x += deacc;

        if (std::abs(body->vel.x) <= deacc) 
            body->vel.x = 0;
    }

    if (body->vel.x > vel_walk) body->vel.x = vel_walk;
    if (body->vel.x < -vel_walk) body->vel.x = -vel_walk;

    if (input_jump) {
        body->vel.y = jumpheight;
    }
}