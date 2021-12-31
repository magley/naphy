#include "image.h"


Image::Image() {
    rend = NULL;
    img = NULL;
    w = -1;
    h = -1;
}

Image::Image(SDL_Renderer* rend, const char* fname) {
    this->rend = rend;
    this->img = IMG_LoadTexture(rend, fname);
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

void Image::draw(int x, int y, int w, int h, int subimage_x, int subimage_y, int subimage_w, int subimage_h) const {
    const SDL_Rect dest{x, y, w, h};
    const SDL_Rect src{subimage_x, subimage_y, subimage_w, subimage_h};
    SDL_RenderCopy(rend, img, &src, &dest);
}

void Image::draw(int x, int y, double scale, int subimage_x, int subimage_y, int subimage_w, int subimage_h) const {
    const SDL_Rect dest{x, y, (int)(subimage_w * scale), (int)(subimage_h * scale)};
    const SDL_Rect src{subimage_x, subimage_y, subimage_w, subimage_h};
    SDL_RenderCopy(rend, img, &src, &dest);
}