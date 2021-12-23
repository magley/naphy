#include "rend.h"



void draw_circle(SDL_Renderer* rend, double centreX, double centreY, double radius) {
	// It is order of magnitudes faster to draw an n-regular polygon instead of using
	// Bresenham’s algorithm. This is because for polygons we can use SDL_RenderDrawLine
	// which draws the whole line in 1 batch, whereas Bresenham’s algorithm would require
	// using SDL_RenderDrawPoint which is 1 batch per pixel and that's very inefficient.
	// SDL_RenderDrawPoints could poentitally be even faster (1 batch for entire shape).


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


void draw_circle_filled(SDL_Renderer* renderer, double centreX, double centreY, double radius) {
	double offsetx, offsety, d;
	offsetx = 0;
	offsety = radius;
	d = radius - 1;

	while (offsety >= offsetx) {
		SDL_RenderDrawLine(renderer, centreX - offsety, centreY + offsetx, centreX + offsety, centreY + offsetx);
		SDL_RenderDrawLine(renderer, centreX - offsetx, centreY + offsety, centreX + offsetx, centreY + offsety);
		SDL_RenderDrawLine(renderer, centreX - offsetx, centreY - offsety, centreX + offsetx, centreY - offsety);
		SDL_RenderDrawLine(renderer, centreX - offsety, centreY - offsetx, centreX + offsety, centreY - offsetx);

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
	const unsigned points_count = vert.size() + 1; // To draw a proper line loop, the first vertex must be added twice so it's +1.

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
	// Arrow = line & isosceles triangle.
	// Tip of the triangle has to be at (x2, y2),
	// so we have shorten the line at the point B
	// The triangle's origin is the base altitude. 

	const double tri_base = 12;
	const double tri_height = 10;

	Vec2 A = Vec2(x1, x1);
	Vec2 B = Vec2(x2, y2);
	const Vec2 ABn = (B - A).normalized();
	B -= ABn * (tri_height * 0.866025); // 0.866025 ~ sqrt(3) / 2
	std::vector<Vec2> triangle = { Vec2(tri_height, 0), Vec2(0, -tri_base * 0.5), Vec2(0, tri_base * 0.5) };
	
	SDL_RenderDrawLineF(renderer, A.x, A.y, B.x, B.y);
	draw_poly(renderer, triangle, Vec2(B.x, B.y), std::atan2(B.y - A.y, B.x - A.x));
}


void draw_text(int x, int y, Image& font_sheet, std::string text) {
	const unsigned char_w = 12;
	const unsigned char_h = 20;
	const unsigned chars_per_line = font_sheet.w / char_w;
	for (unsigned i = 0; i < text.size(); i++) {

		const char c = text[i];

		const int char_x = (c % chars_per_line) * char_w;
		const int char_y = (c / chars_per_line) * char_h;
		font_sheet.draw(x + i * char_w, y, char_x, char_y, char_w, char_h);
	}
}