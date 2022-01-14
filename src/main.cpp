#include "test/test.h"

#define globalvar static


const globalvar unsigned WIN_W = 1440;
const globalvar unsigned WIN_H = 810;
const globalvar unsigned WIN_X = ((1920 - WIN_W) / 2);
const globalvar unsigned WIN_Y = ((1080 - WIN_H) / 2);

#include <set>

int main(int, char**) {
	SDL_Window* win;
	SDL_Renderer* rend;
	SDL_Event ev;
	unsigned running = 1;

	SDL_Init(SDL_INIT_VIDEO);
	win = SDL_CreateWindow("naphy dev.2022.01.14", WIN_X, WIN_Y, WIN_W, WIN_H, SDL_WINDOW_OPENGL);
	rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
	SDL_RenderSetLogicalSize(rend, VIEW_W, VIEW_H);

	Image font(rend, "res/font.png");
	Image gui_atlas(rend, "res/gui.png");
	Image img_drifter(rend, "res/drifter.png");
	Image img_floor(rend, "res/img_floor.png");
	sprites_init(&img_drifter);

	Input input(win);

	GameScene scene = GameScene(
		new PhysScene(rend, 1.0 / 60.0, Vec2(WIN_W, WIN_H), Vec2(WIN_W, WIN_H), Vec2(0, 981), 16),
		new GUI(win, gui_atlas, font)
	);
	start_scene(&scene);
	scene.background = &img_floor;

	while (running) {
		PhysScene* const physscene = scene.physscene;
		GUI* const gui  = scene.gui;

		// Pre-update

		physscene->pre_update();

		do {
			if (ev.type == SDL_QUIT)
				running = false;
		} while (SDL_PollEvent(&ev));

		input.update();
		gui->update(input);

		// Update

		while (physscene->timing.accumulator >= physscene->timing.dt) {
			for (Entity& e : scene.entity) {
				e.update(&input);
			}

			physscene->update();
			physscene->timing.accumulator -= physscene->timing.dt;
			physscene->timing.total += physscene->timing.dt / physscene->timing.scale;
			physscene->timing.ticks_phys++;
		}
		physscene->timing.ticks++;

		// Draw

		SDL_SetRenderDrawColor(rend, 19, 18, 37, 255);
		SDL_RenderClear(rend);
		SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);

		if (scene.background != NULL) {
			scene.background->draw(0, 0);
		}

		for (Entity& e : scene.entity) {
			e.draw(&scene, &img_drifter);
		}
			
		physscene->draw();

		gui->draw(input);
		SDL_RenderPresent(rend);
	}

	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}