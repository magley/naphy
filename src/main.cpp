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


#define WIN_W 1280
#define WIN_H 720
#define WIN_X ((1920 - WIN_W) / 2)
#define WIN_Y ((1080 - WIN_H) / 2)



void init_test_scene(Scene* scene) {
	scene->body.clear();
	scene->arbiter.clear();
	scene->spring.clear();

	Shape poly = Shape({Vec2(-WIN_W/2,32), Vec2(-WIN_W/2,-32), Vec2(WIN_W/2,-32), Vec2(WIN_W/2,32)});
	Shape poly2 = Shape({ Vec2(-4, 350), Vec2(-4, -350), Vec2(4, -350), Vec2(4, 350) });

	PhysBody* b;
	b = scene->add(new PhysBody(Vec2(16, WIN_H/2), poly2));			
	b->calc_mass(0);
	b = scene->add(new PhysBody(Vec2(WIN_W - 16, WIN_H/2), poly2));	
	b->calc_mass(0);
	b = scene->add(new PhysBody(Vec2(WIN_W/2, WIN_H - 100), poly));	
	b->calc_mass(0);
}

void add_poly(GUI* gui, Scene* scene, GUIButton* btn) {
	int vertices = 3 + (rand() % 4);
	int size = 20 + (rand() % 30);
	const Shape c = Shape(vertices, size);
	scene->add(new PhysBody(Vec2(400, 32), c));
}

void add_circle(GUI* gui, Scene* scene, GUIButton* btn) {
	double radius = 10 + rand() % 20;
	const Shape c = Shape(radius);
	scene->add(new PhysBody(Vec2(400, 32), c));
}

void reset_scene(GUI* gui, Scene* scene, GUIButton* btn) {
	scene->body.clear();
	scene->arbiter.clear();
	init_test_scene(scene);
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


	Scene scene = Scene(Vec2(0, 981), 1 / 60.0, WIN_W, WIN_H, 16);
	init_test_scene(&scene);


	PhysBody *b1, *b2;
	b1 = scene.add(new PhysBody(Vec2(100, 460), Shape(40)));
	b2 = scene.add(new PhysBody(Vec2(800, 300), Shape(30)));
	b1->calc_mass(0);
	scene.spring.push_back(Spring(b1, b2, 200, 200, 150));


	GUICheckBox* draw_physbody = gui.add(new GUICheckBox(Vec2(100, 100), "Draw PhysBody"));
				 draw_physbody->checked = true;
	GUICheckBox* draw_arbiter = gui.add(new GUICheckBox(Vec2(124, 100), "Draw Arbiter"));
	GUICheckBox* draw_quadtree = gui.add(new GUICheckBox(Vec2(148, 100), "Draw QuadTree"));
	GUICheckBox* draw_springs = gui.add(new GUICheckBox(Vec2(170, 100), "Draw Spring"));
				 draw_springs->checked = true;
	GUICheckBox* use_quadtree = gui.add(new GUICheckBox(Vec2(192, 100), "Use QuadTree"));
				 use_quadtree->checked = true;

	GUIButton*	add_poly_btn = gui.add(new GUIButton(Vec2(100, 140), "Add new polygon"));
				add_poly_btn->reg_click_callback(add_poly, NULL, &scene);
	GUIButton* 	add_circle_btn = gui.add(new GUIButton(Vec2(100, 164), "Add new circle"));
			   	add_circle_btn->reg_click_callback(add_circle, NULL, &scene);
	GUIButton* 	reset_scene_btn = gui.add(new GUIButton(Vec2(100, 188), "Reset scene"));
			   	reset_scene_btn->reg_click_callback(reset_scene, NULL, &scene);


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

		scene.debug_draw_shapes = draw_physbody->checked;
		scene.debug_draw_arbiters = draw_arbiter->checked;
		scene.debug_draw_quadtree = draw_quadtree->checked;
		scene.debug_draw_springs = draw_springs->checked;
		scene.debug_use_quadtree = use_quadtree->checked;

		//======================================================================
		//Game logic goes here

		if (input.key_down(SDL_SCANCODE_UP))
			b2->vel.y -= 10;
		if (input.key_down(SDL_SCANCODE_RIGHT))
			b2->vel.x += 10;
		if (input.key_down(SDL_SCANCODE_LEFT))
			b2->vel.x -= 10;

		//
		//======================================================================

		scene.update();
		SDL_SetRenderDrawColor(rend, 29, 18, 37, 255);
		SDL_RenderClear(rend);
		SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);

		scene.draw(rend);
		gui.draw(input);
		draw_text(0, 0, font, "naphy ~ dev.2021.12.26", COL_WHITE, COL_BLUE);

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