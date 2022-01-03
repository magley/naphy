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
	
	//Construct an empty Image object. Don't use this manually.	
	Image();
	// Construct a new Image object.
	// @param rend SDL Renderer reference, on which the image will be drawn.
	// @param Filename of the image.
	Image(SDL_Renderer* rend, const char* fname);	
	// Draws the entire image at specified coordinates.
	// @param x X position.
	// @param y Y position.	
	void draw(int x, int y) const;	
	// Draws a slice of the image at specified coordinates.
	// @param x X position.
	// @param y Y position.
	// @param subimage_x X position inside the image where the slice begins.
	// @param subimage_y Y position inside the image where the slice begins.
	// @param subimage_w Width of the slice.
	// @param subimage_h Height of the slice.
	void draw(int x, int y, int subimage_x, int subimage_y, int subimage_w, int subimage_h) const;
	// Draws a scaled slice of the image at specified coordinates.
	// @param x X position.
	// @param y Y position.
	// @param w Width of the image.
	// @param h Height of the image.
	// @param subimage_x X position inside the image where the slice begins.
	// @param subimage_y Y position inside the image where the slice begins.
	// @param subimage_w Width of the slice.
	// @param subimage_h Height of the slice.
	void draw(int x, int y, int w, int h, int subimage_x, int subimage_y, int subimage_w, int subimage_h) const;
	// Draws a scaled slice of the image at specified coordinates.
	// @param x X position.
	// @param y Y position.
	// @param scale Scale.
	// @param subimage_x X position inside the image where the slice begins.
	// @param subimage_y Y position inside the image where the slice begins.
	// @param subimage_w Width of the slice.
	// @param subimage_h Height of the slice.
	void draw(int x, int y, double scale, int subimage_x, int subimage_y, int subimage_w, int subimage_h) const;


	void draw_ex(int x, int y, int subx, int suby, int subw, int subh, int sdl_flip) const;
};