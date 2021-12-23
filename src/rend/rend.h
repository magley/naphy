#pragma once
#include "SDL2/SDL_render.h"
#include "utility/mathutil.h"
#include <vector>


void draw_circle(SDL_Renderer* renderer, double centreX, double centreY, double radius);
void draw_circle_filled(SDL_Renderer* renderer, double centreX, double centreY, double radius);
void draw_poly(SDL_Renderer* renderer, std::vector<Vec2> verts, Vec2 pos, double angle);
void draw_arrow(SDL_Renderer* renderer, double x1, double y1, double x2, double y2);