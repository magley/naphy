#include "test.h"

#define internal static

internal unsigned scene_index;

internal void reset_scene(GUIButton* btn) {
	GUI* gui = btn->gui;
	start_scene(gui->scene);
}

internal void set_scale(PhysScene* scene, unsigned scale) {
	VIEW_W = scene->win_size.x / scale;
	VIEW_H = scene->win_size.y / scale;
	SDL_RenderSetLogicalSize(scene->rend, VIEW_W, VIEW_H);
	scene->quadtree.clear();
	scene->quadtree = QuadNode({0, 0}, Vec2(VIEW_W, VIEW_H), scene->quadtree.capacity);
}

internal void scene_clear_ext(GameScene* scene, Vec2 grav, unsigned scale) {
	scene->entity.clear();

    scene->physscene->grav = grav;
	scene->physscene->body.clear();
	scene->physscene->arbiter.clear();
	scene->physscene->spring.clear();
	set_scale(scene->physscene, scale);
}

internal void add_poly(GUIButton* btn) {
	PhysScene* scene = btn->gui->scene->physscene;

	int vertices = 3 + (rand() % 4);
	int size = 20 + (rand() % 30);
	const Shape c = Shape(vertices, size);
	scene->add(new PhysBody(Vec2(400, 32), c));
}

internal void add_circle(GUIButton* btn) {
	PhysScene* scene = btn->gui->scene->physscene;

	double radius = 10 + rand() % 20;
	const Shape c = Shape(radius);
	scene->add(new PhysBody(Vec2(400, 32), c));
}

internal void next_scene(GUIButton* btn) {
	PhysScene* scene = btn->gui->scene->physscene;

	scene_index++; // range check for scene_index goes in start_scene().
	reset_scene(btn);
}

internal void add_box(GUIButton* btn) {
	PhysScene* scene = btn->gui->scene->physscene;

	static int i = 0;

	int vertices = 4;
	int size = 50;
	const Shape c = Shape(vertices, size);
	scene->add(new PhysBody(Vec2(VIEW_W * 0.8 + ((i++ + 5) % 3) * 0, 32), c))->set_angle(45 * DEG2RAD);
}


//-------------------------------------------------------------------------------------------------


internal void init_test_basic(GameScene* scn) {
	PhysScene* scene = scn->physscene;
	GUI* gui = scn->gui;

    scene_clear_ext(scn, {0, 981}, 1);
    PhysBody* b;

    const Shape rect = Shape(Vec2(scene->win_size.x, 64));

	// Floor

	b = scene->add(new PhysBody(Vec2(VIEW_W / 2, VIEW_H - 48), rect));
	b->calc_mass(0);
	b->material.set_ice();

	// Three shapes, one dynamic, one static, one half-static, connected with springs

	PhysBody *b0, *b1, *b2;
	b0 = scene->add(new PhysBody(Vec2(400, 360), Shape(10, 40)));
	b0->m_inv = 0;
	b0->m = 0;
	b0->material.set_jelly();
	b1 = scene->add(new PhysBody(Vec2(100, 300), Shape(30)));
	b1->calc_mass(0);
	b1->material.set_jelly();
	b2 = scene->add(new PhysBody(Vec2(200, 500), Shape(6, 20)));
	b2->material.set_jelly();

	scene->spring.push_back(Spring(b0, b2, 50, 2, 4));
	scene->spring.push_back(Spring(b1, b2, 40, 4, 8));

	// Curtain: grid of small bodies connected with springs

	const unsigned bw = 15;
	const unsigned bh = 10;
	std::vector<std::vector<PhysBody*>> curtain(bh, std::vector<PhysBody*>(bw, NULL));

	for (unsigned y = 0; y < bh; y++) {
		for (unsigned x = 0; x < bw; x++) {
			curtain[y][x] = new PhysBody(Vec2(600 + (8 + x) * x, 160 + 16 * y - x * 2), Shape(4));
			scene->add(curtain[y][x]);

			if (y == 0) {
				curtain[y][x]->calc_mass(0);
			}
		}
	}
	for (unsigned y = 0; y < bh; y++) {
		for (unsigned x = 0; x < bw; x++) {
			if (y + 1 < bh)
				scene->spring.push_back(Spring(curtain[y][x], curtain[y + 1][x], 0, 6, 4));
			if (x + 1 < bw)
				scene->spring.push_back(Spring(curtain[y][x], curtain[y][x + 1], 0, 6, 4));
		}
	}

	// GUI - checkbox

	GUICheckBox* cb;

	cb = gui->add(new GUICheckBox(gui, {0, 24}, "Draw PhysBody"));
		cb->checked = true;
		cb->reg_toggle_target(&scene->debug_draw_shapes, true);
	cb = gui->add(new GUICheckBox(gui, {24, 24}, "Draw Arbiter"));
		cb->reg_toggle_target(&scene->debug_draw_arbiters, true);
	cb = gui->add(new GUICheckBox(gui, {48, 24}, "Draw QuadTree"));
		cb->reg_toggle_target(&scene->debug_draw_quadtree, true);
	cb = gui->add(new GUICheckBox(gui, {72, 24}, "Draw Spring"));
		cb->checked = true;
		cb->reg_toggle_target(&scene->debug_draw_springs, true);
	cb = gui->add(new GUICheckBox(gui, {96, 24}, "Use QuadTree"));
		cb->checked = true;
		cb->reg_toggle_target(&scene->debug_use_quadtree, true);

	// GUI - button

	GUIButton* btn;

	btn = gui->add(new GUIButton(gui, {0, 48}, "Add new polygon"));
		btn->reg_click_callback(add_poly);
	btn = gui->add(new GUIButton(gui, {144, 48}, "Reset scene"));
		btn->reg_click_callback(reset_scene);
	btn = gui->add(new GUIButton(gui, {48, 48}, "Add new circle"));
		btn->reg_click_callback(add_circle);
	btn = gui->add(new GUIButton(gui, {96, 48}, "Add box"));
		btn->reg_click_callback(add_box);
	btn = gui->add(new GUIButton(gui, {144, 48}, "Reset scene"));
		btn->reg_click_callback(reset_scene);
	btn = gui->add(new GUIButton(gui, {192, 48}, "Next test"));
		btn->reg_click_callback(next_scene);

	// GUI - label

	GUILabel* lbl;
	lbl = gui->add(new GUILabel({0, 0}, "naphy test :: shapes", COL_WHITE, COL_BLUE));
}

internal void init_stacking_scene(GameScene* scn) {
	PhysScene* scene = scn->physscene;
	GUI* gui = scn->gui;

	scene_clear_ext(scn, {0, 981}, 1);

	Shape rect = Shape(Vec2{1600, 64});
	Shape rect2 = Shape(Vec2{120, 12});

	// Floor

	PhysBody* b;
	b = scene->add(new PhysBody(Vec2(VIEW_W / 2, VIEW_H - 48), rect));
	b->calc_mass(0);

	// Tower thingy

	b = scene->add(new PhysBody(Vec2(400, VIEW_H - 48 - 32 - 60), rect2));
	b->set_angle(90 * DEG2RAD);
	b = scene->add(new PhysBody(Vec2(520, VIEW_H - 48 - 32 - 60), rect2));
	b->set_angle(90 * DEG2RAD);
	b = scene->add(new PhysBody(Vec2(460, VIEW_H - 48 - 32 - 120 - 8), rect2));

	b = scene->add(new PhysBody(Vec2(540, VIEW_H - 48 - 32 - 60), rect2));
	b->set_angle(90 * DEG2RAD);
	b = scene->add(new PhysBody(Vec2(660, VIEW_H - 48 - 32 - 60), rect2));
	b->set_angle(90 * DEG2RAD);
	b = scene->add(new PhysBody(Vec2(600, VIEW_H - 48 - 32 - 120 - 8), rect2));

	b = scene->add(new PhysBody(Vec2(468, VIEW_H - 48 - 32 - 120 - 8 - 60), rect2));
	b->set_angle(90 * DEG2RAD);
	b = scene->add(new PhysBody(Vec2(592, VIEW_H - 48 - 32 - 120 - 8 - 60), rect2));
	b->set_angle(90 * DEG2RAD);
	b = scene->add(new PhysBody(Vec2(530, VIEW_H - 48 - 32 - 120 - 8 - 120 - 8), rect2));

	// GUI - checkbox

	GUICheckBox* cb;

	cb = gui->add(new GUICheckBox(gui, {0, 24}, "Draw PhysBody"));
		cb->checked = true;
		cb->reg_toggle_target(&scene->debug_draw_shapes, true);
	cb = gui->add(new GUICheckBox(gui, {24, 24}, "Draw Arbiter"));
		cb->reg_toggle_target(&scene->debug_draw_arbiters, true);
	cb = gui->add(new GUICheckBox(gui, {48, 24}, "Draw QuadTree"));
		cb->reg_toggle_target(&scene->debug_draw_quadtree, true);
	cb = gui->add(new GUICheckBox(gui, {72, 24}, "Draw Spring"));
		cb->checked = true;
		cb->reg_toggle_target(&scene->debug_draw_springs, true);
	cb = gui->add(new GUICheckBox(gui, {96, 24}, "Use QuadTree"));
		cb->checked = true;
		cb->reg_toggle_target(&scene->debug_use_quadtree, true);

	// GUI - button

	GUIButton* btn;

	btn = gui->add(new GUIButton(gui, {0, 48}, "Add new polygon"));
		btn->reg_click_callback(add_poly);
	btn = gui->add(new GUIButton(gui, {144, 48}, "Reset scene"));
		btn->reg_click_callback(reset_scene);
	btn = gui->add(new GUIButton(gui, {48, 48}, "Add new circle"));
		btn->reg_click_callback(add_circle);
	btn = gui->add(new GUIButton(gui, {96, 48}, "Add box"));
		btn->reg_click_callback(add_box);
	btn = gui->add(new GUIButton(gui, {144, 48}, "Reset scene"));
		btn->reg_click_callback(reset_scene);
	btn = gui->add(new GUIButton(gui, {192, 48}, "Next test"));
		btn->reg_click_callback(next_scene);

	// GUI - label

	GUILabel* lbl;
	lbl = gui->add(new GUILabel({0, 0}, "naphy test :: stacking", COL_WHITE, COL_BLUE));
}

internal void init_bridge_test(GameScene* scn) {
	PhysScene* scene = scn->physscene;
	GUI* gui = scn->gui;

    scene_clear_ext(scn, {0, 981}, 1);
    PhysBody* b;

    const Shape rect = Shape(Vec2(100, 500));

	b = scene->add(new PhysBody(Vec2(100 / 2, VIEW_H - 500 / 2), rect));
	b->calc_mass(0);

	b = scene->add(new PhysBody(Vec2(VIEW_W - 100 / 2, VIEW_H - 500 / 2), rect));
	b->calc_mass(0);

	// Bridge

	std::vector<PhysBody*> bridge;

	unsigned bridge_cnt = 14 + 2;
	for (unsigned x = 0; x < bridge_cnt; x++) {
		double gap = (VIEW_W - 200) / bridge_cnt;
		double w = gap * 0.9;
		bridge.push_back(new PhysBody(Vec2(80 + w + gap * x, 324), Shape(Vec2(w, 16))));
		scene->add(bridge[x]);

		
		if (x == 0 || x == bridge_cnt - 1) {
			bridge[x]->calc_mass(0);
		} else {
			bridge[x]->calc_mass(10);
		}
		bridge[x]->I_inv = 0;
		bridge[x]->I = 0;
	}
	for (unsigned x = 0; x < bridge_cnt - 1; x++) {
		scene->spring.push_back(Spring(bridge[x], bridge[x + 1], 10, 30000, 20000));
	}

	// Player

	b = scene->add(new PhysBody({32, 0}, Shape(Vec2(32, 120))));
	b->calc_mass(1);
	b->material.e = 0;
	b->I_inv = 0;
	b->I = 0;
	Entity player{C_JUMPNRUN | C_PHYS};
	player.cjumpnrun = CJumpNRun();
	player.cphys = CPhys(b);
	scn->entity.push_back(player); // pass by copy

	// GUI - checkbox

	GUICheckBox* cb;

	cb = gui->add(new GUICheckBox(gui, {0, 24}, "Draw PhysBody"));
		cb->checked = true;
		cb->reg_toggle_target(&scene->debug_draw_shapes, true);
	cb = gui->add(new GUICheckBox(gui, {24, 24}, "Draw Arbiter"));
		cb->reg_toggle_target(&scene->debug_draw_arbiters, true);
	cb = gui->add(new GUICheckBox(gui, {48, 24}, "Draw QuadTree"));
		cb->reg_toggle_target(&scene->debug_draw_quadtree, true);
	cb = gui->add(new GUICheckBox(gui, {72, 24}, "Draw Spring"));
		cb->checked = true;
		cb->reg_toggle_target(&scene->debug_draw_springs, true);
	cb = gui->add(new GUICheckBox(gui, {96, 24}, "Use QuadTree"));
		cb->checked = true;
		cb->reg_toggle_target(&scene->debug_use_quadtree, true);

	// GUI - button

	GUIButton* btn;

	btn = gui->add(new GUIButton(gui, {0, 48}, "Add new polygon"));
		btn->reg_click_callback(add_poly);
	btn = gui->add(new GUIButton(gui, {144, 48}, "Reset scene"));
		btn->reg_click_callback(reset_scene);
	btn = gui->add(new GUIButton(gui, {48, 48}, "Add new circle"));
		btn->reg_click_callback(add_circle);
	btn = gui->add(new GUIButton(gui, {96, 48}, "Add box"));
		btn->reg_click_callback(add_box);
	btn = gui->add(new GUIButton(gui, {144, 48}, "Reset scene"));
		btn->reg_click_callback(reset_scene);
	btn = gui->add(new GUIButton(gui, {192, 48}, "Next test"));
		btn->reg_click_callback(next_scene);

	// GUI - label

	GUILabel* lbl;
	lbl = gui->add(new GUILabel({0, 0}, "naphy test :: bridge (arrow keys, space)", COL_WHITE, COL_BLUE));
}

internal void init_drifter_scene(GameScene* scn) {
	PhysScene* scene = scn->physscene;
	GUI* gui = scn->gui;

	scene_clear_ext(scn, {0, 0}, 3);

	PhysBody* b;

	b = scene->add(new PhysBody({10, 136}, Shape(Vec2{20, 37})));
	b->calc_mass(0);
	b->layer = LAYER_SOLID;
	b->material.e = 0;

	b = scene->add(new PhysBody({113, 136}, Shape(Vec2{105, 35})));
	b->calc_mass(0);
	b->layer = LAYER_SOLID;
	b->material.e = 0;

	b = scene->add(new PhysBody({183, 153}, Shape({{36, -8}, {26, 46}, {-28, 0}, {-18, -34}})));
	b->calc_mass(0);
	b->layer = LAYER_SOLID;
	b->material.e = 0;

	b = scene->add(new PhysBody({259, 170}, Shape(Vec2{101, 56})));
	b->calc_mass(0);
	b->layer = LAYER_SOLID;
	b->material.e = 0;

	b = scene->add(new PhysBody({323, 150}, Shape(Vec2{34, 42})));
	b->calc_mass(0);
	b->layer = LAYER_SOLID;
	b->material.e = 0;

	Shape drifter_shape = Shape({{-3.5, -2}, {3.5, -2}, {3.5, 2}, {-3.5, 2}});
	b = scene->add(new PhysBody({32, 112}, drifter_shape));
	b->material.e = 0;
	b->I_inv = 0;
	b->I = 0;
	b->layer = LAYER_DRIFTER;

	Entity player{C_DRIFTER | C_SPRITE | C_PHYS};
	player.cdrifter = CDrifter();
	player.cphys = CPhys(b);
	player.csprite = CSprite(SPR_DRIFTER_DOWN_STAND, 0.5);
	player.csprite.depth = 0;
	scn->entity.push_back(player); // pass by copy

	b = scene->add(new PhysBody({275, 235}, Shape(Vec2{29, 70})));
	b->calc_mass(0);
	b->aggregate_state = PHYSBODY_PASSIVE;
	b->layer = LAYER_PIT;
	b->material.e = 0;


	Entity background{C_SPRITE};
	background.csprite = CSprite(SPR_BG_FLOOR, 0);
	background.csprite.depth = -100;
	scn->entity.push_back(background); // pass by copy


	// GUI

	GUICheckBox* cb;

	cb = gui->add(new GUICheckBox(gui, {0, 24}, "Draw PhysBody"));
		cb->reg_toggle_target(&scene->debug_draw_shapes, true);

	cb = gui->add(new GUICheckBox(gui, {24, 24}, "Draw Arbiter"));
		cb->reg_toggle_target(&scene->debug_draw_arbiters, true);

	GUIButton* btn;

	btn = gui->add(new GUIButton(gui, {48, 48}, "Reset scene"));
		btn->reg_click_callback(reset_scene);
	btn = gui->add(new GUIButton(gui, {96, 48}, "Next test"));
		btn->reg_click_callback(next_scene);

	GUILabel* lbl;

	lbl = gui->add(new GUILabel({0, 0}, "naphy test :: drifter (WASD, mouse, space)", COL_WHITE, COL_BLUE));
}


//-------------------------------------------------------------------------------------------------


void start_scene(GameScene* gamescene) {
	gamescene->physscene->clear();
	gamescene->gui->clear();

	const std::vector<void (*)(GameScene*)> init_func_arr = {
		init_test_basic,
		init_stacking_scene,
		init_bridge_test,
		init_drifter_scene
	};

	if (scene_index >= init_func_arr.size())
		scene_index = 0;
	init_func_arr[scene_index](gamescene);
}