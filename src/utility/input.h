#pragma once
#include <stdint.h>

struct SDL_Window;

// SDL input wrapper. Supports keyboard and mouse functionality.
struct Input {
	// Mouse button state in current frame.
	uint32_t mouse_curr;
	// Mouse button state in previous frame.
	uint32_t mouse_prev;
	// Mouse position in the current frame.
	int mouse_x, mouse_y;
	// Mouse position in the previous frame.
	int mouse_x_prev, mouse_y_prev;
	// Keyboard state in previous frame.
	uint8_t kb_prev[256];
	// Keyboard state in current frame.
	uint8_t kb_curr[256];
	// Pointer to an SDL_Window. Scaling the viewport with SDL_RenderSetLogicalSize affects some SDL
	// events through an event filter. Mouse state is *not* affected, which means we have to adjust
	// mouse cooridnates manually, by dividing with the view scale. An SDL_Renderer is not enough.
	// This handle will not be modified by Input. 
	SDL_Window* win;


	// Construct a new Input object "attached" to the provided SDL_Window.
	Input(SDL_Window* win);


	// Update the input. This should be called once per frame, AFTER polling SDL events.
	void update();
	// Check if a key is held down.
	// @param scancode SDL_SCANCODE_*
	bool key_down(unsigned scancode) const;
	// Check if a key was pressed.
	// @param scancode SDL_SCANCODE_*
	bool key_press(unsigned scancode) const;
	// Check if a key was released.
	// @param scancode SDL_SCANCODE_*
	bool key_up(unsigned scancode) const;
	// Check if a mouse button is held down.
	// @param scancode SDL_BUTTON_*MASK
	bool mouse_down(unsigned button_mask) const;
	// Check if a mouse button was pressed.
	// @param scancode SDL_BUTTON_*MASK
	bool mouse_press(unsigned button_mask) const;
	// Check if a mouse button was released.
	// @param scancode SDL_BUTTON_*MASK
	bool mouse_up(unsigned button_mask) const;
	// @brief Check if the mouse cursor is within rectangle bounds (all inclusive).
	// @param x1 Left
	// @param x2 Right
	// @param y1 Top
	// @param y2 Bottom
	bool mouse_in_bounds(int x1, int x2, int y1, int y2) const;
};