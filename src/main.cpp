#include "test/test.h"

#define globalvar static

globalvar SDL_Window* win;
globalvar SDL_Renderer* rend;
globalvar SDL_Event ev;
globalvar unsigned running = 1;
const globalvar unsigned WIN_W = 1440;
const globalvar unsigned WIN_H = 810;
const globalvar unsigned WIN_X = ((1920 - WIN_W) / 2);
const globalvar unsigned WIN_Y = ((1080 - WIN_H) / 2);


#include <set>


struct SpriteWithDepth {
	CSprite spr;
	int depth;
};

bool operator <(const SpriteWithDepth& a, const SpriteWithDepth& b) {
	return a.depth < b.depth;
}

int main(int, char**) {
	SDL_Init(SDL_INIT_VIDEO);
	win = SDL_CreateWindow("naphy dev.2022.01.11", WIN_X, WIN_Y, WIN_W, WIN_H, SDL_WINDOW_OPENGL);
	rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
	SDL_RenderSetLogicalSize(rend, VIEW_W, VIEW_H);

	Image font(rend, "res/font.png");
	Image gui_atlas(rend, "res/gui.png");
	Image img_drifter(rend, "res/drifter.png");
	Image img_floor(rend, "res/img_floor.png");

	Input input(win);

	GameScene scene = GameScene(
		new PhysScene(rend, 1.0/60.0, Vec2(WIN_W,WIN_H), Vec2(WIN_W,WIN_H), Vec2(0,981), 16),
		new GUI(win, gui_atlas, font)
	);

	start_scene(&scene);
	sprites_init(&img_drifter);


	//std::multiset<SpriteWithDepth> q;
	//q.insert({CSprite(SPR_DRIFTER_DOWN_WALK, 2), 2});
	//q.insert({CSprite(SPR_DRIFTER_DOWN_DRIFT, 2), 15});

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
			physscene->update();
			physscene->timing.accumulator -= physscene->timing.dt;
			physscene->timing.total += physscene->timing.dt / physscene->timing.scale;
			physscene->timing.ticks_phys++;
		}
		physscene->timing.ticks++;

		// Draw

		SDL_SetRenderDrawColor(rend, 29, 18, 37, 255);
		SDL_RenderClear(rend);
		SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);

		physscene->draw();

		//int i = 0;
		//for (auto it = q.begin(); it != q.end(); it++) {
		//	it->spr.draw(Vec2(8 * i++, 128));
		//}

		gui->draw(input);
		SDL_RenderPresent(rend);
	}

	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}














/*
// Resize view and fix scene quadtree to match the new size.
// scale is how many times the view is smaller than the screen.
static void set_scale(Scene* scene, unsigned scale) {
	VIEW_W = WIN_W / scale;
	VIEW_H = WIN_H / scale;
	SDL_RenderSetLogicalSize(rend, VIEW_W, VIEW_H);
	scene->quadtree.clear();
	scene->quadtree = QuadNode({0, 0}, {VIEW_W, VIEW_H}, scene->quadtree.capacity);
}

static PhysBody* player;
static CDrifter drifter;
static int running = 1; // When 0, the game loop ends.
static int resetme = 0; // Set to 1 to signal that the scene should be reset. Check is done manually.

enum {TEST_INDEX_SHAPES, TEST_INDEX_STACKING, TEST_INDEX_DRIFTER, TEST_INDEX_size};
static unsigned test_index = TEST_INDEX_SHAPES;
static void init_test_scene(Scene* scene, GUI* gui);
static void init_stacking_scene(Scene* scene, GUI* gui);
static void init_drifter_scene(Scene* scene, GUI* gui);

static void (*test_init_func[TEST_INDEX_size])(Scene* scene, GUI* gui) = {
	init_test_scene,
	init_stacking_scene,
	init_drifter_scene,
};

static void add_poly(Scene* scene, GUIButton* btn) {
	int vertices = 3 + (rand() % 4);
	int size = 20 + (rand() % 30);
	const Shape c = Shape(vertices, size);
	scene->add(new PhysBody(Vec2(400, 32), c));
}

static void add_circle(Scene* scene, GUIButton* btn) {
	double radius = 10 + rand() % 20;
	const Shape c = Shape(radius);
	scene->add(new PhysBody(Vec2(400, 32), c));
}

static void reset_scene(Scene* scene, GUIButton* btn) {
	GUI* gui = btn->gui;

	scene->clear();
	gui->clear();
	test_init_func[test_index](scene, gui);

	resetme = 1;
}

static void next_scene(Scene* scene, GUIButton* btn) {
	test_index++;
	if (test_index == TEST_INDEX_size)
		test_index = 0;
	reset_scene(scene, btn);
}

static void add_box(Scene* scene, GUIButton* btn) {
	static int i = 0;

	int vertices = 4;
	int size = 50;
	const Shape c = Shape(vertices, size);
	scene->add(new PhysBody(Vec2(VIEW_W * 0.8 + ((i++ + 5) % 3) * 0, 32), c))->set_angle(45 * DEG2RAD);
}

static void init_drifter_scene(Scene* scene, GUI* gui) {
	scene->grav = {0, 0};
	scene->body.clear();
	scene->arbiter.clear();
	scene->spring.clear();
	scene->grav = {0, 0};
	set_scale(scene, 3);

	PhysBody* b;

	b = scene->add(new PhysBody({31, 137}, Shape(Vec2{62, 38})));
	b->calc_mass(0);
	b->layer = LAYER_SOLID;
	b->material.e = 0;

	b = scene->add(new PhysBody({162, 137}, Shape(Vec2{110, 38})));
	b->calc_mass(0);
	b->layer = LAYER_SOLID;
	b->material.e = 0;

	b = scene->add(new PhysBody({300, 175}, Shape({{63, -28}, {63, 24}, {-44, 24}, {-100, -28}})));
	b->calc_mass(0);
	b->layer = LAYER_SOLID;
	b->material.e = 0;

	b = scene->add(new PhysBody({420, 151}, Shape(Vec2{120, 44})));
	b->calc_mass(0);
	b->layer = LAYER_SOLID;
	b->material.e = 0;

	b = scene->add(new PhysBody({230, 137.75}, Shape({{-14, -20}, {32, 12}, {-14, 12}})));
	b->calc_mass(0);
	b->layer = LAYER_SOLID;
	b->material.e = 0;

	Shape drifter_shape = Shape({{-3.5, -2}, {3.5, -2}, {3.5, 2}, {-3.5, 2}});
	player = scene->add(new PhysBody({32, 112}, drifter_shape));
	player->material.e = 0;
	player->I_inv = 0;
	player->I = 0;
	player->layer = LAYER_DRIFTER;
	drifter = CDrifter(player);

	b = scene->add(new PhysBody({328, 235}, Shape(Vec2{29, 70})));
	b->calc_mass(0);
	b->aggregate_state = PHYSBODY_PASSIVE;
	b->layer = LAYER_PIT;
	b->material.e = 0;


	// GUI

	GUICheckBox* draw_physbody = gui->add(new GUICheckBox(gui, {0, 24}, "Draw PhysBody"));
	draw_physbody->checked = true;
	draw_physbody->reg_toggle_target(&scene->debug_draw_shapes, true);

	GUICheckBox* draw_arbiter = gui->add(new GUICheckBox(gui, {24, 24}, "Draw Arbiter"));
	draw_arbiter->reg_toggle_target(&scene->debug_draw_arbiters, true);

	GUIButton* add_circle_btn = gui->add(new GUIButton(gui, {0, 48}, "Add new circle"));
	add_circle_btn->reg_click_callback(add_circle, scene);

	GUIButton* reset_scene_btn = gui->add(new GUIButton(gui, {48, 48}, "Reset scene"));
	reset_scene_btn->reg_click_callback(reset_scene, scene);
	GUIButton* next_scene_btn = gui->add(new GUIButton(gui, {96, 48}, "Next test"));
	next_scene_btn->reg_click_callback(next_scene, scene);

	GUILabel* lbl_naphy = gui->add(new GUILabel(gui, {0, 0}, "naphy test :: drifter (WASD, mouse, space)", COL_WHITE, COL_BLUE));
}

static void init_test_scene(Scene* scene, GUI* gui) {
	scene->grav = {0, 981};
	scene->body.clear();
	scene->arbiter.clear();
	scene->spring.clear();
	set_scale(scene, 1);

	Shape rect = Shape({{-800, 32}, {-800, -32}, {800, -32}, {800, 32}});
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
			curtain[y][x] = new PhysBody(Vec2(600 + 16 * x, 200 + 16 * y), Shape(4));
			scene->add(curtain[y][x]);

			if (y == 0) {
				curtain[y][x]->calc_mass(0);
			}
		}
	}

	for (int y = 0; y < bh; y++) {
		for (int x = 0; x < bw; x++) {
			if (y < bh - 1)
				scene->spring.push_back(Spring(curtain[y][x], curtain[y + 1][x], 0, 6, 3));
			if (x < bw - 1)
				scene->spring.push_back(Spring(curtain[y][x], curtain[y][x + 1], 0, 6, 3));
		}
	}

	b = scene->add(new PhysBody({106, 0}, Shape(30)));
	b->calc_mass(1);
	player = b;

	// GUI

	GUICheckBox* draw_physbody = gui->add(new GUICheckBox(gui, {0, 24}, "Draw PhysBody"));
	draw_physbody->checked = true;
	draw_physbody->reg_toggle_target(&scene->debug_draw_shapes, true);
	GUICheckBox* draw_arbiter = gui->add(new GUICheckBox(gui, {24, 24}, "Draw Arbiter"));
	draw_arbiter->reg_toggle_target(&scene->debug_draw_arbiters, true);
	GUICheckBox* draw_quadtree = gui->add(new GUICheckBox(gui, {48, 24}, "Draw QuadTree"));
	draw_quadtree->reg_toggle_target(&scene->debug_draw_quadtree, true);
	GUICheckBox* draw_springs = gui->add(new GUICheckBox(gui, {72, 24}, "Draw Spring"));
	draw_springs->checked = true;
	draw_springs->reg_toggle_target(&scene->debug_draw_springs, true);
	GUICheckBox* use_quadtree = gui->add(new GUICheckBox(gui, {96, 24}, "Use QuadTree"));
	use_quadtree->checked = true;
	use_quadtree->reg_toggle_target(&scene->debug_use_quadtree, true);

	GUIButton* add_poly_btn = gui->add(new GUIButton(gui, {0, 48}, "Add new polygon"));
	add_poly_btn->reg_click_callback(add_poly, scene);
	GUIButton* add_circle_btn = gui->add(new GUIButton(gui, {48, 48}, "Add new circle"));
	add_circle_btn->reg_click_callback(add_circle, scene);
	GUIButton* add_box_btn = gui->add(new GUIButton(gui, {96, 48}, "Add box"));
	add_box_btn->reg_click_callback(add_box, scene);
	GUIButton* reset_scene_btn = gui->add(new GUIButton(gui, {144, 48}, "Reset scene"));
	reset_scene_btn->reg_click_callback(reset_scene, scene);
	GUIButton* next_scene_btn = gui->add(new GUIButton(gui, {192, 48}, "Next test"));
	next_scene_btn->reg_click_callback(next_scene, scene);

	GUILabel* lbl_naphy = gui->add(new GUILabel(gui, {0, 0}, "naphy test :: shapes", COL_WHITE, COL_BLUE));
}

static void init_stacking_scene(Scene* scene, GUI* gui) {
	scene->grav = {0, 981};
	scene->body.clear();
	scene->arbiter.clear();
	scene->spring.clear();
	set_scale(scene, 1);

	Shape rect = Shape(Vec2{1600, 64});
	Shape rect2 = Shape(Vec2{120, 12});

	// Floor

	PhysBody* b;
	b = scene->add(new PhysBody({VIEW_W / 2, VIEW_H - 48}, rect));
	b->calc_mass(0);

	// Tower thingy

	b = scene->add(new PhysBody({400, VIEW_H - 48 - 32 - 60}, rect2));
	b->set_angle(90 * DEG2RAD);
	b = scene->add(new PhysBody({520, VIEW_H - 48 - 32 - 60}, rect2));
	b->set_angle(90 * DEG2RAD);
	b = scene->add(new PhysBody({460, VIEW_H - 48 - 32 - 120 - 8}, rect2));

	b = scene->add(new PhysBody({540, VIEW_H - 48 - 32 - 60}, rect2));
	b->set_angle(90 * DEG2RAD);
	b = scene->add(new PhysBody({660, VIEW_H - 48 - 32 - 60}, rect2));
	b->set_angle(90 * DEG2RAD);
	b = scene->add(new PhysBody({600, VIEW_H - 48 - 32 - 120 - 8}, rect2));

	b = scene->add(new PhysBody({468, VIEW_H - 48 - 32 - 120 - 8 - 60}, rect2));
	b->set_angle(90 * DEG2RAD);
	b = scene->add(new PhysBody({592, VIEW_H - 48 - 32 - 120 - 8 - 60}, rect2));
	b->set_angle(90 * DEG2RAD);
	b = scene->add(new PhysBody({530, VIEW_H - 48 - 32 - 120 - 8 - 120 - 8}, rect2));


	// GUI

	GUICheckBox* draw_physbody = gui->add(new GUICheckBox(gui, {0, 24}, "Draw PhysBody"));
	draw_physbody->checked = true;
	draw_physbody->reg_toggle_target(&scene->debug_draw_shapes, true);
	GUICheckBox* draw_arbiter = gui->add(new GUICheckBox(gui, {24, 24}, "Draw Arbiter"));
	draw_arbiter->checked = true;
	draw_arbiter->reg_toggle_target(&scene->debug_draw_arbiters, true);
	GUICheckBox* draw_quadtree = gui->add(new GUICheckBox(gui, {48, 24}, "Draw QuadTree"));
	draw_quadtree->reg_toggle_target(&scene->debug_draw_quadtree, true);
	GUICheckBox* draw_springs = gui->add(new GUICheckBox(gui, {72, 24}, "Draw Spring"));
	draw_springs->checked = true;
	draw_springs->reg_toggle_target(&scene->debug_draw_springs, true);
	GUICheckBox* use_quadtree = gui->add(new GUICheckBox(gui, {96, 24}, "Use QuadTree"));
	use_quadtree->checked = true;
	use_quadtree->reg_toggle_target(&scene->debug_use_quadtree, true);

	GUIButton* reset_scene_btn = gui->add(new GUIButton(gui, {120, 24}, "Reset scene"));
	reset_scene_btn->reg_click_callback(reset_scene, scene);
	GUIButton* next_scene_btn = gui->add(new GUIButton(gui, {168, 24}, "Next test"));
	next_scene_btn->reg_click_callback(next_scene, scene);

	GUILabel* lbl_naphy = gui->add(new GUILabel(gui, {0, 0}, "naphy test :: stacking", COL_WHITE, COL_BLUE));
}

int main(int, char**) {
	SDL_Init(SDL_INIT_VIDEO);
	win = SDL_CreateWindow("naphy ~ dev.2022.01.10", WIN_X, WIN_Y, WIN_W, WIN_H, SDL_WINDOW_OPENGL);
	rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
	SDL_RenderSetLogicalSize(rend, VIEW_W, VIEW_H);

	Image font(rend, "res/font.png");
	Image gui_atlas(rend, "res/gui->png");
	Image img_drifter(rend, "res/drifter.png");
	Image img_floor(rend, "res/img_floor.png");
	sprites_init(&img_drifter);
	Input input(win);
	GUI gui(win, gui_atlas, font);

	Scene scene = Scene({0, 0}, 1 / 60.0, VIEW_W, VIEW_H, 16);
	test_init_func[test_index](&scene, &gui);

	while (running) {
		resetme = 0;
		scene.pre_update();

		SDL_Event ev;
		do {
			if (ev.type == SDL_QUIT)
				running = false;
		} while (SDL_PollEvent(&ev));

		input.update();
		gui->update(input);

		if (resetme)
			continue;

		//-----------------------------------------------------------------------------------------
		//// Update

		while (scene.timing.accumulator >= scene.timing.dt) {
			if (test_index == TEST_INDEX_DRIFTER) drifter.update(&input, &scene);

			scene.update();

			scene.timing.accumulator -= scene.timing.dt;
			scene.timing.total += scene.timing.dt / scene.timing.scale;
			scene.timing.ticks_phys++;
		}
		scene.timing.ticks++;

		//
		//-----------------------------------------------------------------------------------------

		if (resetme)
			continue;

		//-----------------------------------------------------------------------------------------
		//// Draw

		SDL_SetRenderDrawColor(rend, 29, 18, 37, 255);
		SDL_RenderClear(rend);
		SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);

		if (test_index == TEST_INDEX_DRIFTER) img_floor.draw(0, 0);
		scene.draw(rend);
		if (test_index == TEST_INDEX_DRIFTER) drifter.draw(&img_drifter);

		gui->draw(input);
		SDL_RenderPresent(rend);

		//
		//-----------------------------------------------------------------------------------------
	}

	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}
*/