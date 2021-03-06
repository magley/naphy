#include "image.h"

Image::Image() {
	rend = NULL;
	tex = NULL;
	w = -1;
	h = -1;
}

Image::Image(SDL_Renderer* rend, const char* fname) {
	this->rend = rend;
	this->tex = IMG_LoadTexture(rend, fname);
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
}

void Image::draw(int x, int y, int w, int h, int subx, int suby, int subw, int subh, int flip) const {
	const SDL_Rect dest{x, y, w, h};
	const SDL_Rect src{subx, suby, subw, subh};
	SDL_RenderCopyEx(rend, tex, &src, &dest, 0, NULL, (SDL_RendererFlip)flip);
}