#pragma once

#include "image.h"

// Sprite = Image with more context.
// A Sprite is a portion of an Image with support for animation.
// Animation is supported using fixed-size offsets.
// The i-th frame is at pixel offset: pos + (size.x * i, 0).
// Because of this, all frames must be placed horizontaly in the image and they have the same size.
struct Sprite {
	Image* img;
	unsigned w, h; // Size of the sprite (in pixels).
	unsigned x, y; // Position of the sprite in the Image (from top-left corner, in pixels).
	unsigned frames;

	Sprite();
	Sprite(Image* image, unsigned x_offset, unsigned y_offset, unsigned width, unsigned height, unsigned anim_frames);
};

// A SpriteContext structure carries all info 
struct SpriteContext {
	unsigned sprite_index;   // Identifier of a Sprite object.
	unsigned image_index;    // Which frame of the Sprite object to draw.
	int flip;                // Whether to flip on either axis. We're using SDL, so it's SDL_FLIP_*.
	int depth;               // Depth. Smaller values are behind greater values.
	double x, y;             // Where to draw the sprite on the screen.
	unsigned char r, g, b, a;// Color and alpha.
};

inline bool operator < (const SpriteContext& a, const SpriteContext& b) {
	return a.depth < b.depth;
}
