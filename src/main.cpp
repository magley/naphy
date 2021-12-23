#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include <string>
#include <iostream>

#include "rend/image.h"
#include "naphy/scene.h"


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



double time_total = 0.0;									// Total time, in seconds.
double time_scale = 1;										// Scale, because it's too slow by default.
double time_curr = SDL_GetTicks() / 1000.0 * time_scale;	// Current time, in ms.
double time_accumulator = 0.0;								// Accumulator for fixed time steps.
unsigned ticks = 0;											// Number of game loop (not physics loop) frames.
unsigned ticks_phys = 0;									// Number of physics loop frames.



int main(int, char**) {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* win = SDL_CreateWindow("", 100, 100, 800, 608, SDL_WINDOW_OPENGL);
	SDL_Renderer* rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	Image font(rend, "res/font.png");

	Scene scene = Scene();
	PhysBody* b;


	Shape poly = Shape(std::vector<Vec2> { Vec2(-400, 32), Vec2(-400, -32), Vec2(400, -32), Vec2(400, 32), });
	Shape poly2 = Shape(std::vector<Vec2> { Vec2(-32, 32), Vec2(-32, -32), Vec2(32, -32), Vec2(32, 32)  });
	Shape poly3 = Shape(3, 55);
	Shape poly4 = Shape(std::vector<Vec2>{ Vec2(320, 0), Vec2(-320, 0)});
	Shape poly5 = Shape(5, 64);
	Shape c(25);
	

	b = scene.add(PhysBody(Vec2(300, 400), poly2));
	b = scene.add(PhysBody(Vec2(300, 360), poly2));
	b = scene.add(PhysBody(Vec2(500, 64), poly2));
	b = scene.add(PhysBody(Vec2(490, 0), poly5));
	b = scene.add(PhysBody(Vec2(400, 32), c));
	b = scene.add(PhysBody(Vec2(450, 32), c));
	b = scene.add(PhysBody(Vec2(500, 32), c));
	b = scene.add(PhysBody(Vec2(550, 32), c));
	b = scene.add(PhysBody(Vec2(400, 580), poly));
	b->calc_mass(0);

	printf("%d\n", b->dynamic_state);



	while (1) {
		const double time_new = SDL_GetTicks() / 1000.0 * time_scale;
		const double time_diff = time_new - time_curr;
		time_curr = time_new;
		time_accumulator += time_diff;



		SDL_Event ev;
		SDL_PollEvent(&ev);

		if (ev.type == SDL_QUIT)
			break;


		b->set_angle(7 * DEG2RAD * cos((time_total + EPSILON) / PI * 5));

		time_accumulator = clamp(0.0, 0.1, time_accumulator);
		while (time_accumulator >= scene.dt) {
			scene.update();
			time_accumulator -= scene.dt;
			time_total += scene.dt / time_scale;
			ticks_phys++;
		}


		SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
		SDL_RenderClear(rend);
		SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);


		draw_text(0, 0, font, "naphy ~ development version 2021.12.23");

		scene.render(rend);

		SDL_RenderPresent(rend);
	}


	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}