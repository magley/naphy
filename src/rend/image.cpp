#include "image.h"


Image::Image(SDL_Renderer* rend, const char* fname) {
    this->rend = rend;
    this->img = IMG_LoadTexture(rend, "res/font.png");
    SDL_QueryTexture(img, NULL, NULL, &w, &h);
}


void Image::draw(int x, int y) const {
    const SDL_Rect dest{x, y, w, h};
    SDL_RenderCopy(rend, img, NULL, &dest);
}


void Image::draw(int x, int y, int subimage_x, int subimage_y, int subimage_w, int subimage_h) const {
    const SDL_Rect dest{x, y, subimage_w, subimage_h};
    const SDL_Rect src{subimage_x, subimage_y, subimage_w, subimage_h};
    SDL_RenderCopy(rend, img, &src, &dest);
}