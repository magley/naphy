#pragma once
#include "SDL2/SDL_render.h"
#include <string>
#include <vector>
#include "utility/mathutil.h"
#include "image.h"


#define FONT_CH_W 18
#define FONT_CH_H 24

#define COL_BLACK {0,0,0,0}
#define COL_WHITE {255,255,255,255}
#define COL_BLUE {0,0,255,255}


void draw_circle(SDL_Renderer* renderer, double centreX, double centreY, double radius);
void draw_circle_filled(SDL_Renderer* renderer, double centreX, double centreY, double radius);
void draw_poly(SDL_Renderer* renderer, std::vector<Vec2> verts, Vec2 pos, double angle);
void draw_arrow(SDL_Renderer* renderer, double x1, double y1, double x2, double y2);
void draw_text(int x, int y, Image& font_sheet, std::string text);
void draw_text(int x, int y, Image& font_sheet, std::string text, SDL_Color col, SDL_Color bg);
void draw_text(int x, int y, double scale, const Image& font_sheet, std::string text, SDL_Color col, SDL_Color bg);