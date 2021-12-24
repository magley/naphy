#include "input.h"
#include "SDL2/SDL_keyboard.h"
#include "SDL2/SDL_mouse.h"
#include "SDL2/SDL_events.h"


Input::Input() {
	for (unsigned i = 0; i < 256; i++) {
		kb_prev[i] = 0;
		kb_curr[i] = 0;
	}

	mouse_curr = 0;
	mouse_prev = 0;
	mouse_x = 0;
	mouse_y = 0;
	mouse_x_prev = 0;
	mouse_y_prev = 0;
}


void Input::update() {
	const Uint8* state = SDL_GetKeyboardState(NULL);
	for (unsigned i = 0; i < 256; i++) {
		kb_prev[i] = kb_curr[i];
		kb_curr[i] = state[i];
	}

	mouse_prev = mouse_curr;
	mouse_x_prev = mouse_x;
	mouse_y_prev = mouse_y;
	mouse_curr = SDL_GetMouseState(&mouse_x, &mouse_y);
}


bool Input::key_down(unsigned scancode) const {
	return kb_curr[scancode] != 0;
}


bool Input::key_press(unsigned scancode) const {
	return kb_curr[scancode] != 0 && kb_prev[scancode] == 0;
}


bool Input::key_up(unsigned scancode) const {
	return kb_curr[scancode] == 0 && kb_prev[scancode] != 0;
}


bool Input::mouse_down(unsigned button_mask) const {
	return ((mouse_curr & button_mask) != 0);
}


bool Input::mouse_press(unsigned button_mask) const {
	return ((mouse_curr & button_mask) != 0)
		&& ((mouse_prev & button_mask) == 0);
}


bool Input::mouse_up(unsigned button_mask) const {
	return ((mouse_curr & button_mask) == 0
		&& ((mouse_prev & button_mask) != 0));
}


bool Input::mouse_in_bounds(int x1, int x2, int y1, int y2) const {
	return x1 <= mouse_x && mouse_x <= x2 && y1 <= mouse_y && mouse_y <= y2;
}