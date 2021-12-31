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

#define VIEW_W 960
#define VIEW_H 540

#define VIEW_SCALE ((double)((WIN_W / VIEW_W)))


PhysBody* init_test_scene(Scene* scene) {
	scene->body.clear();
	scene->arbiter.clear();
	scene->spring.clear();

	Shape rect = Shape({{-500, 32}, {-500, -32}, {500, -32}, {500, 32}});

	// Floor

	PhysBody* b;
	b = scene->add(new PhysBody({VIEW_W / 2, VIEW_H - 48}, rect));
	b->calc_mass(0);
	b->material.set_ice();

	// Springy thingy

	PhysBody *b0, *b1, *b2;
	b0 = scene->add(new PhysBody(Vec2(400, 260), Shape(10, 60)));
		b0->m_inv = 0;
		b0->m = 0;
		b0->material.set_jelly();
	b1 = scene->add(new PhysBody(Vec2(100, 200), Shape(40)));
		b1->calc_mass(0);
		b1->material.set_jelly();
	b2 = scene->add(new PhysBody(Vec2(200, 300), Shape(6, 30)));
		b2->material.set_jelly();

	scene->spring.push_back(Spring(b0, b2, 50, 2, 4));
	scene->spring.push_back(Spring(b1, b2, 40, 4, 8));

	// Curtain

	const int bw = 15;
	const int bh = 10;

	std::vector<std::vector<PhysBody*>> curtain(bh, std::vector<PhysBody*>(bw, NULL));

	for (int y = 0; y < bh; y++) {
		for (int x = 0; x < bw; x++) {
			curtain[y][x] = new PhysBody(Vec2(600 + 16 * x, 100 + 16 * y), Shape(4));
			scene->add(curtain[y][x]);

			if (y == 0) {
				curtain[y][x]->calc_mass(0);
			}
		}
	}

	for (int y = 0; y < bh; y++) {
		for (int x = 0; x < bw; x++) {
			if (y < bh-1) 
				scene->spring.push_back(Spring(curtain[y][x], curtain[y + 1][x], 0, 4, 1));
			if (x < bw-1) 
				scene->spring.push_back(Spring(curtain[y][x], curtain[y][x + 1], 0, 4, 1));
		}
	}

	b = scene->add(new PhysBody({16, 0}, Shape(10)));
	b->calc_mass(1);
	return b;
}

void add_poly(Scene* scene, GUIButton* btn) {
	int vertices = 3 + (rand() % 4);
	int size = 20 + (rand() % 30);
	const Shape c = Shape(vertices, size);
	scene->add(new PhysBody(Vec2(400, 32), c));
}

void add_circle(Scene* scene, GUIButton* btn) {
	double radius = 10 + rand() % 20;
	const Shape c = Shape(radius);
	scene->add(new PhysBody(Vec2(400, 32), c));
}

void reset_scene(Scene* scene, GUIButton* btn) {
	scene->clear();
	init_test_scene(scene);
}

int main(int, char**) {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* win = SDL_CreateWindow("naphy", WIN_X, WIN_Y, WIN_W, WIN_H, SDL_WINDOW_OPENGL);
	SDL_Renderer* rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
	SDL_RenderSetLogicalSize(rend, VIEW_W, VIEW_H);
	SDL_Rect viewport = {0, 0, VIEW_W, VIEW_H};
	SDL_RenderSetViewport(rend, &viewport);
	Image font(rend, "res/font.png");
	Image gui_atlas(rend, "res/gui.png");
	Input input(win);
	GUI gui(win, gui_atlas, font);


	Scene scene = Scene(Vec2(0, 981), 1 / 60.0, VIEW_W, VIEW_H, 16);
	PhysBody* player = init_test_scene(&scene);


	GUICheckBox* draw_physbody = gui.add(new GUICheckBox(&gui, Vec2(100, 100), "Draw PhysBody"));
				 draw_physbody->checked = true;
	GUICheckBox* draw_arbiter = gui.add(new GUICheckBox(&gui, Vec2(124, 100), "Draw Arbiter"));
	GUICheckBox* draw_quadtree = gui.add(new GUICheckBox(&gui, Vec2(148, 100), "Draw QuadTree"));
	GUICheckBox* draw_springs = gui.add(new GUICheckBox(&gui, Vec2(172, 100), "Draw Spring"));
				 draw_springs->checked = true;
	GUICheckBox* use_quadtree = gui.add(new GUICheckBox(&gui, Vec2(196, 100), "Use QuadTree"));
				 use_quadtree->checked = true;

	GUIButton*	add_poly_btn = gui.add(new GUIButton(&gui, Vec2(100, 140), "Add new polygon"));
				add_poly_btn->reg_click_callback(add_poly, &scene);
	GUIButton* 	add_circle_btn = gui.add(new GUIButton(&gui, Vec2(100, 164), "Add new circle"));
			   	add_circle_btn->reg_click_callback(add_circle, &scene);
	GUIButton* 	reset_scene_btn = gui.add(new GUIButton(&gui, Vec2(100, 188), "Reset scene"));
			   	reset_scene_btn->reg_click_callback(reset_scene, &scene);


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

		//-----------------------------------------------------------------------------------------
		//
		//Game logic goes here

		player->vel.x += (input.key_down(SDL_SCANCODE_D) - input.key_down(SDL_SCANCODE_A)) * 8;
		player->vel.y += (input.key_down(SDL_SCANCODE_S) - input.key_down(SDL_SCANCODE_W)) * 15; 

		if (player->vel.y < -400) player->vel.y = -400;
		if (player->vel.y > +400) player->vel.y = +400;
		if (player->vel.x < -400) player->vel.x = -400;
		if (player->vel.x > +400) player->vel.x = +400;

		//
		//-----------------------------------------------------------------------------------------

		scene.update();

		SDL_SetRenderDrawColor(rend, 29, 18, 37, 255);
		SDL_RenderClear(rend);
		SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);

		scene.draw(rend);
		gui.draw(input);

		draw_text(0 * gui.scale, 0 * gui.scale, gui.scale, font, "naphy ~ dev.2021.12.31", COL_WHITE, COL_BLUE);
		std::stringstream ss;
		ss << "obj: " << scene.body.size();
		draw_text(0 * gui.scale, FONT_CH_H * gui.scale, gui.scale, font, ss.str(), COL_WHITE, COL_BLUE);

		SDL_RenderPresent(rend);
	}


	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}