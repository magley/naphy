#pragma once

#include "SDL2/SDL_image.h"


/**
 * @brief Wrapper structure for SDL_Texture.
 */
struct Image {
	/**
	 * @brief Pointer to the rendering target. Do not modify manually.
	 */
	SDL_Renderer* rend;
	/**
	 * @brief Pointer to the texture data. Do not modify manually.
	 */
	SDL_Texture* img;
	/**
	 * @brief Width of the image. Do not modify manully.
	 */
	int w;
	/**
	 * @brief Height of the image. Do not modify manully.
	 */
	int h;
	/**
	 * @brief Construct an empty Image object. Don't use this manually.
	 */
	Image();
	/**
	 * @brief Construct a new Image object.
	 * 
	 * @param rend SDL Renderer reference, on which the image will be drawn.
	 * @param fname Filename of the image.
	 */
	Image(SDL_Renderer* rend, const char* fname);
	/**
	 * @brief Draws the entire image at specified coordinates.
	 * 
	 * @param x X position.
	 * @param y Y position.
	 */
	void draw(int x, int y) const;
	/**
	 * @brief Draws a slice of the image at specified coordinates.
	 * 
	 * @param x X position.
	 * @param y Y position.
	 * @param subimage_x X position inside the image where the slice begins.
	 * @param subimage_y Y position inside the image where the slice begins.
	 * @param subimage_w Width of the slice.
	 * @param subimage_h Height of the slice.
	 */
	void draw(int x, int y, int subimage_x, int subimage_y, int subimage_w, int subimage_h) const;
};