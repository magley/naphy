#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include <string>
#include <iostream>

#include "rend/rend.h"
#include "rend/image.h"
#include "naphy/scene.h"


#define WIN_W 800
#define WIN_H 608
#define WIN_X ((1920 - WIN_W) / 2)
#define WIN_Y ((1080 - WIN_H) / 2)


int main(int, char**) {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* win = SDL_CreateWindow("naphy", WIN_X, WIN_Y, WIN_W, WIN_H, SDL_WINDOW_OPENGL);
	SDL_Renderer* rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
	Image font(rend, "res/font.png");

	Scene scene = Scene(Vec2(0, 400), 1 / 60.0, WIN_W, WIN_H, 4);
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


	while (1) {
		scene.pre_update();

		SDL_Event ev;
		SDL_PollEvent(&ev);

		if (ev.type == SDL_QUIT)
			break;

		b->set_angle(7 * DEG2RAD * cos((scene.timing.total + EPSILON) / PI * 5));
		scene.update();


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