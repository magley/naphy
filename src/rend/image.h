#pragma once

#include "SDL2/SDL_image.h"

// Wrapper structure for SDL_Texture.
struct Image {	
	// Pointer to the rendering target. Do not modify manually.
	SDL_Renderer* rend;
	// Pointer to the texture data. Do not modify manually.
	SDL_Texture* img;
	// Width of the image. Do not modify manually.	
	int w;
	// Height of the image. Do not modify manually.
	int h;
	
	//Construct an empty Image object, for unitialized arrays. Don't use this explicitly.	
	Image();
	// Construct a new Image object.
	// @param rend SDL Renderer reference, on which the image will be drawn.
	// @param fname Filename of the image.
	Image(SDL_Renderer* rend, const char* fname);	
	// Draws the entire image at specified coordinates.
	// @param x X position.
	// @param y Y position.	
	inline void draw(int x, int y) const {
		draw(x, y, w, h, 0, 0, w, h, SDL_FLIP_NONE);
	}	
	// Draws a slice of the image at specified coordinates.
	// @param x X position.
	// @param y Y position.
	// @param subx X position inside the image where the slice begins.
	// @param suby Y position inside the image where the slice begins.
	// @param subw Width of the slice.
	// @param subh Height of the slice.
	// @param sdl_flip Which axis to flip on, valid value is one of SDL_FLIP* 
	inline void draw(int x, int y, int subx, int suby, int subw, int subh, int flip = 0) const {
		draw(x, y, subw, subh, subx, suby, subw, subh, flip);
	}
	// Draws a scaled slice of the image at specified coordinates.
	// @param x X position.
	// @param y Y position.
	// @param w Width of the image.
	// @param h Height of the image.
	// @param subx X position inside the image where the slice begins.
	// @param suby Y position inside the image where the slice begins.
	// @param subw Width of the slice.
	// @param subh Height of the slice.
	// @param sdl_flip Which axis to flip on, valid value is one of SDL_FLIP* 
	void draw(int x, int y, int w, int h, int subx, int suby, int subw, int subh, int flip = 0) const;
};