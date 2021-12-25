#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include <string>
#include <iostream>
#include <sstream>

#include "utility/input.h"

#include "rend/rend.h"
#include "rend/image.h"
#include "naphy/scene.h"

#include "gui/gui.h"
#include "gui/gui_checkbox.h"


#define WIN_W 800
#define WIN_H 608
#define WIN_X ((1920 - WIN_W) / 2)
#define WIN_Y ((1080 - WIN_H) / 2)


void add_poly(GUI* gui, Scene* scene, GUIButton* btn) {
	int vertices = 3 + (rand() % 4);
	int size = 20 + (rand() % 30);
	const Shape c = Shape(vertices, size);
	scene->add(PhysBody(Vec2(400, 32), c));
}

void add_circle(GUI* gui, Scene* scene, GUIButton* btn) {
	double radius = 10 + rand() % 20;
	const Shape c = Shape(radius);
	scene->add(PhysBody(Vec2(400, 32), c));
}


int main(int, char**) {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* win = SDL_CreateWindow("naphy", WIN_X, WIN_Y, WIN_W, WIN_H, SDL_WINDOW_OPENGL);
	SDL_Renderer* rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
	Image font(rend, "res/font.png");
	Image gui_atlas(rend, "res/gui.png");
	Input input;
	GUI gui(gui_atlas, font);

	Scene scene = Scene(Vec2(0, 400), 1 / 60.0, WIN_W, WIN_H, 4);
	Shape poly = Shape(std::vector<Vec2> { Vec2(-400, 32), Vec2(-400, -32), Vec2(400, -32), Vec2(400, 32), });

	unsigned b = 0;
	b = scene.add(PhysBody(Vec2(400, 500), poly));
	scene.body[b].calc_mass(0);

	
	GUICheckBox* view_meta_cb = gui.add(new GUICheckBox(Vec2(100, 100), "Display arbiters and quad tree"));
	GUIButton* add_poly_btn = gui.add(new GUIButton(Vec2(100, 140), "Add new polygon"));
				add_poly_btn->reg_click_callback(add_poly, NULL, &scene);
	GUIButton* add_circle_btn = gui.add(new GUIButton(Vec2(100, 164), "Add new circle"));
				add_circle_btn->reg_click_callback(add_circle, NULL, &scene);


	bool running = true;
	while (running) {
		scene.pre_update();

		SDL_Event ev;
		do {
			if (ev.type == SDL_QUIT)
				running = false;
		} while (SDL_PollEvent(&ev));

		input.update();
		gui.update(input);

		//======================================================================
		//Game logic goes here

		scene.body[b].set_angle(10 * DEG2RAD * sin((scene.timing.total + EPSILON) / 2));

		//
		//======================================================================

		scene.update();
		SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
		SDL_RenderClear(rend);
		SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);

		scene.draw(rend, view_meta_cb->checked);
		gui.draw(input);
		draw_text(0, 0, font, "naphy ~ dev.2021.12.25", COL_WHITE, COL_BLUE);

		std::stringstream ss;
		ss << "obj: " << scene.body.size() << " fps:" << (int)(scene.timing.ticks / scene.timing.total);
		draw_text(0, FONT_CH_H, font, ss.str(), COL_WHITE, COL_BLUE);

		SDL_RenderPresent(rend);
	}


	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}