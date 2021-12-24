#pragma once
#include <stdint.h>


/**
 * @brief SDL Input wrapper. Supports keyboard and mouse.
 */
struct Input {
	// Mouse state in current frame.
	uint32_t mouse_curr;
	// Mouse state in previous frame.
	uint32_t mouse_prev;
	// Keyboard state in previous frame.
	uint8_t kb_prev[256];
	// Keyboard state in current frame.
	uint8_t kb_curr[256];
	// Mouse position in the current frame.
	int mouse_x, mouse_y;
	// Mouse position in the previous frame.
	int mouse_x_prev, mouse_y_prev;


	/**
	 * @brief Construct a new Input object.
	 */
	Input();


	/**
	 * @brief Update the input.
	 * This should be called once per frame, after polling SDL events.
	 */
	void update();
	/**
	 * @brief Check if a key is held down.
	 * @param scancode SDL_SCANCODE_*
	 */
	bool key_down(unsigned scancode) const;
	/**
	 * @brief Check if a key was pressed.
	 * @param scancode SDL_SCANCODE_*
	 */
	bool key_press(unsigned scancode) const;
	/**
	 * @brief Check if a key was released.
	 * @param scancode SDL_SCANCODE_*
	 */
	bool key_up(unsigned scancode) const;
	/**
	 * @brief Check if a mouse button is held down.
	 * @param scancode SDL_BUTTON_*MASK
	 */
	bool mouse_down(unsigned button_mask) const;
	/**
	 * @brief Check if a mouse button was pressed.
	 * @param scancode SDL_BUTTON_*MASK
	 */
	bool mouse_press(unsigned button_mask) const;
	/**
	 * @brief Check if a mouse button was released.
	 * @param scancode SDL_BUTTON_*MASK
	 */
	bool mouse_up(unsigned button_mask) const;
	/**
	 * @brief Check if the mouse cursor is within rectangle bounds.
	 * 
	 * @param x1 Left
	 * @param x2 Right
	 * @param y1 Top
	 * @param y2 Bottom
	 */
	bool mouse_in_bounds(int x1, int x2, int y1, int y2) const;
};