#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <string>
#include <sstream>
#include <iostream>
#include "utility/input.h"
#include "rend/rend.h"
#include "rend/image.h"
#include "naphy/scene.h"
#include "gui/gui.h"
#include "gui/gui_checkbox.h"
#include "gui/gui_label.h"

#include "game/cdrifter.h"


#define WIN_W 960
#define WIN_H 540
#define WIN_X ((1920 - WIN_W) / 2)
#define WIN_Y ((1080 - WIN_H) / 2)
#define VIEW_W 480
#define VIEW_H 270


PhysBody* player;
CDrifter drifter;

void init_drifter_scene(Scene* scene) {
	scene->body.clear();
	scene->arbiter.clear();
	scene->spring.clear();
	scene->grav = {0, 0};

	PhysBody* b;

	b = scene->add(new PhysBody({300, 150}, Shape(70)));
	b->calc_mass(0);
	b->material.e = 0;

	player = scene->add(new PhysBody({32, 132}, Shape(2)));
	player->material.e = 0;
	drifter = CDrifter(player);
}


void init_test_scene(Scene* scene) {
	scene->body.clear();
	scene->arbiter.clear();
	scene->spring.clear();

	Shape rect = Shape({{-500, 32}, {-500, -32}, {500, -32}, {500, 32}});
	Shape rect2 = Shape({{-60, 8}, {-60, -8}, {60, -8}, {60, 8}});

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
	player = b;
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
	init_drifter_scene(scene);
}

int main(int, char**) {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* win = SDL_CreateWindow("naphy", WIN_X, WIN_Y, WIN_W, WIN_H, SDL_WINDOW_OPENGL);
	SDL_Renderer* rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
	SDL_RenderSetLogicalSize(rend, VIEW_W, VIEW_H);


	Image font(rend, "res/font.png");
	Image gui_atlas(rend, "res/gui.png");
	Image img_drifter(rend, "res/drifter.png");
	Image img_floor(rend, "res/img_floor.png");
	sprites_init(&img_drifter);
	Input input(win);
	GUI gui(win, gui_atlas, font);


	Scene scene = Scene({0, 0 * 981}, 1 / 60.0, VIEW_W, VIEW_H, 16);
	init_drifter_scene(&scene);


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

	GUILabel*	lbl_naphy = gui.add(new GUILabel(&gui, {0, 0}, "naphy ~ dev.2022.01.01", COL_WHITE, COL_BLUE));
	GUILabel*	lbl_obj = gui.add(new GUILabel(&gui, {0, FONT_CH_H * gui.scale}, "obj:", COL_WHITE, COL_BLUE));


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
		//Game logic goes here
		//

		drifter.update(&input);

		//
		//-----------------------------------------------------------------------------------------

		scene.update();
		lbl_obj->text = "obj:" + std::to_string(scene.body.size());


		SDL_SetRenderDrawColor(rend, 29, 18, 37, 255);
		SDL_RenderClear(rend);
		SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);

		img_floor.draw(0, 0);
		scene.draw(rend);
		drifter.draw(&img_drifter);

		gui.draw(input);
		SDL_RenderPresent(rend);
	}


	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}