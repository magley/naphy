#include "rend.h"



void draw_circle(SDL_Renderer* rend, double centreX, double centreY, double radius) {
	// It is order of magnitudes faster to draw an n-regular polygon instead of using
	// Bresenham’s algorithm. This is because for polygons we can use SDL_RenderDrawLine
	// which draws the whole line in 1 batch, whereas Bresenham’s algorithm would require
	// using SDL_RenderDrawPoint which is 1 batch per pixel and that's very inefficient.
	// SDL_RenderDrawPoints could potentially be even faster (1 batch for entire shape).

	const unsigned precision = (unsigned)((2 * radius * 3.141));
	std::vector<Vec2> points;
	for (unsigned k = 0; k < precision; k++) {
		const double angle = k * (2.0 * PI) / precision;
		const double c = cos(angle) * radius;
		const double s = sin(angle) * radius;

		points.push_back(Vec2(centreX + c, centreY + s));
	}

	draw_poly(rend, points, Vec2(0, 0), 0);
}

void draw_circle_filled(SDL_Renderer* renderer, double x, double y, double radius) {
	double offsetx, offsety, d;
	offsetx = 0;
	offsety = radius;
	d = radius - 1;

	while (offsety >= offsetx) {
		SDL_RenderDrawLine(renderer, x - offsety, y + offsetx, x + offsety, y + offsetx);
		SDL_RenderDrawLine(renderer, x - offsetx, y + offsety, x + offsetx, y + offsety);
		SDL_RenderDrawLine(renderer, x - offsetx, y - offsety, x + offsetx, y - offsety);
		SDL_RenderDrawLine(renderer, x - offsety, y - offsetx, x + offsety, y - offsetx);

		if (d >= 2 * offsetx) {
			d -= 2 * offsetx + 1;
			offsetx += 1;
		} else if (d < 2 * (radius - offsety)) {
			d += 2 * offsety - 1;
			offsety -= 1;
		} else {
			d += 2 * (offsety - offsetx - 1);
			offsety -= 1;
			offsetx += 1;
		}
	}
}

void draw_poly(SDL_Renderer* renderer, std::vector<Vec2> vert, Vec2 pos, double angle) {
	const Mat2x2 rot = Mat2x2(angle);
	const unsigned points_count = vert.size() + 1; // +1 to make a line loop

	SDL_FPoint* points = (SDL_FPoint*)malloc(sizeof(SDL_FPoint) * points_count);
	for (unsigned i = 0; i < points_count; i++) {
		const Vec2 A = pos + rot * vert[i % vert.size()];
		SDL_FPoint p;
		p.x = A.x;
		p.y = A.y;
		points[i] = p;
	}

	SDL_RenderDrawLinesF(renderer, points, points_count);
	free(points);
}

void draw_arrow(SDL_Renderer* renderer, double x1, double y1, double x2, double y2) {
	// Arrow = line + equilateral triangle.

	const Vec2 A = Vec2(x1, y1);
	const Vec2 B = Vec2(x2, y2);
	const double triangle_size = (B - A).len() / 10;
	const Vec2 C = Vec2(B.x, B.y) - Vec2(0, triangle_size / 2); // Where the triangle base starts

	std::vector<Vec2> triangle;
	for (unsigned i = 0; i < 3; i++) {
		double theta = i * (2.0 * PI) / 3;
		const double c = cos(theta) * triangle_size;
		const double s = sin(theta) * triangle_size;
		triangle.push_back(Vec2(c, s));
	}

	SDL_RenderDrawLineF(renderer, A.x, A.y, B.x, B.y);
	draw_poly(renderer, triangle, C, std::atan2(B.y - A.y, B.x - A.x));
}

void draw_text(int x, int y, Image& font_sheet, std::string text) {
	draw_text(x, y, 1.0, font_sheet, text, {255, 255, 255, 255}, {255, 255, 255, 255});
}

void draw_text(int x, int y, Image& font_sheet, std::string text, SDL_Color col, SDL_Color bg) {
	draw_text(x, y, 1.0, font_sheet, text, col, bg);
}

void draw_text(int x, int y, double scale, const Image& font_sheet, std::string text, SDL_Color col, SDL_Color bg) {
	const unsigned chars_per_line = font_sheet.w / FONT_CH_W;

	uint8_t r, g, b, a; // All external rendering data should stay the same after this function.

	// Background

	const int p = 0; // padding
	SDL_Rect rect = {
		x - p, 
		y - p, 
		(int)(FONT_CH_W * scale * text.size()) + 2 * p, 
		(int)(FONT_CH_H * scale + 2 * p)
	};

	SDL_GetRenderDrawColor(font_sheet.rend, &r, &g, &b, &a);
	SDL_SetRenderDrawColor(font_sheet.rend, bg.r, bg.g, bg.b, bg.a);
	SDL_RenderFillRect(font_sheet.rend, &rect);
	SDL_SetRenderDrawColor(font_sheet.rend, r, g, b, a);

	// Text

	SDL_GetTextureColorMod(font_sheet.tex, &col.r, &col.g, &col.b);
	SDL_GetTextureAlphaMod(font_sheet.tex, &col.a);

	SDL_SetTextureColorMod(font_sheet.tex, col.r, col.g, col.b);
	SDL_SetTextureAlphaMod(font_sheet.tex, col.a);

	for (unsigned i = 0; i < text.size(); i++) {

		const char c = text[i];

		const int char_x = (c % chars_per_line) * FONT_CH_W;
		const int char_y = (c / chars_per_line) * FONT_CH_H;

		font_sheet.draw(
			x + i * FONT_CH_W * scale, 
			y, 
			(int)round(FONT_CH_W * scale), 
			(int)round(FONT_CH_H * scale), 
			char_x, 
			char_y, 
			FONT_CH_W, 
			FONT_CH_H
		);
	}

	SDL_SetTextureColorMod(font_sheet.tex, r, g, b);
	SDL_SetTextureAlphaMod(font_sheet.tex, a);
}