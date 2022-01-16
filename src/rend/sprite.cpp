#include "sprite.h"

Sprite::Sprite() { ; }

Sprite::Sprite(Image* image, unsigned x_offset, unsigned y_offset, unsigned width, unsigned height, unsigned anim_frames)  {
    img = image;
    x = x_offset;
    y = y_offset;
    w = width;
    h = height;
    frames = anim_frames;
}