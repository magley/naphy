#include "test/test2.h"
#include "game/gamescene.h"
#include "naphy/physbody.h"
#include <sstream>
#include <string>

static int scene_index = 0;

void test_common(GameScene *scn) {
}

void reset_scene(GUIButton *btn) {
    GUI *gui = btn->gui;
    test2(gui->scene);
}

void next_scene(GUIButton *btn) {
    PhysScene *scene = btn->gui->scene->physscene;

    scene_index++;   // range check for scene_index goes in start_scene().
    reset_scene(btn);
}

void gui_common(GameScene *scn) {
    PhysScene *scene = scn->physscene;
    GUI       *gui   = scn->gui;

    // Checkbox

    GUICheckBox *cb;

    cb          = gui->add(new GUICheckBox(gui, {0, 24}, "Draw PhysBody"));
    cb->checked = true;
    cb->reg_toggle_target(&scene->debug_draw_shapes, true);

    cb          = gui->add(new GUICheckBox(gui, {24, 24}, "Draw Arbiter"));
    cb->checked = false;
    cb->reg_toggle_target(&scene->debug_draw_arbiters, true);

    cb          = gui->add(new GUICheckBox(gui, {48, 24}, "Draw QuadTree"));
    cb->checked = false;
    cb->reg_toggle_target(&scene->debug_draw_quadtree, true);

    cb          = gui->add(new GUICheckBox(gui, {72, 24}, "Draw Spring"));
    cb->checked = true;
    cb->reg_toggle_target(&scene->debug_draw_springs, true);

    cb          = gui->add(new GUICheckBox(gui, {96, 24}, "Use QuadTree"));
    cb->checked = true;
    cb->reg_toggle_target(&scene->debug_use_quadtree, true);

    // Label

    GUILabel *lbl;
    lbl = gui->add(new GUILabel({0, 0}, "stacking", COL_WHITE, COL_BLUE));

    // Buttons

    GUIButton *btn;
    btn = gui->add(new GUIButton(gui, {144, 48}, "Reset scene"));
    btn->reg_click_callback(reset_scene);
    btn = gui->add(new GUIButton(gui, {48, 48}, "Next test"));
    btn->reg_click_callback(next_scene);
}

void scene_clear_ext(GameScene *scene, Vec2 grav, unsigned scale) {
    scene->entity.clear();

    scene->physscene->grav = grav;
    scene->physscene->body.clear();
    scene->physscene->arbiter.clear();
    scene->physscene->spring.clear();

    VIEW_W2 = scene->physscene->win_size.x / scale;
    VIEW_H2 = scene->physscene->win_size.y / scale;
    SDL_RenderSetLogicalSize(scene->physscene->rend, VIEW_W2, VIEW_H2);
    scene->physscene->quadtree.clear();
    scene->physscene->quadtree = QuadNode({0, 0}, Vec2(VIEW_W2, VIEW_H2), scene->physscene->quadtree.capacity);
}

void test_boxes_stacking(GameScene *scn) {
    PhysScene *scene = scn->physscene;
    GUI       *gui   = scn->gui;

    scene_clear_ext(scn, {0, 981}, 1);
    PhysBody *b = nullptr;

    // Floor

    b = scene->add(new PhysBody(Vec2(VIEW_W2 / 2.0, VIEW_H2 - 48), Shape(Vec2(1200, 64))));
    b->calc_mass(0);

    // Boxes

    for (int i = 0; i < 3; i++) {
        float box_size = 96;
        float x        = VIEW_W2 / 2.0;
        float y        = (VIEW_H2 - 48) - ((box_size + 24) * (i + 1));

        b = scene->add(new PhysBody(Vec2(x, y), Shape(Vec2(box_size, box_size))));
    }
}

void test_mass_proportions(GameScene *scn) {
    PhysScene *scene = scn->physscene;
    GUI       *gui   = scn->gui;

    scene_clear_ext(scn, {0, 981}, 1);
    PhysBody *b = nullptr;

    // Floor

    b = scene->add(new PhysBody(Vec2(VIEW_W2 / 2.0, VIEW_H2 - 48), Shape(Vec2(1200, 64))));
    b->calc_mass(0);

    // Boxes

    std::vector<float> masses = {0.1, 10.0, 1.0, 100000.0, 10.0, 100000000000.0};

    for (int k = 0; k < masses.size() / 2; k++) {
        int   boxsize1 = 48;
        int   boxsize2 = 192;
        int   i        = (k * 2);
        int   j        = (k * 2) + 1;
        float x        = (VIEW_W2 / 4.0) * (k + 1);
        float y1       = (VIEW_H2 - 48) - ((boxsize1 + 24) * 1);
        float y2       = (VIEW_H2 - 48) - ((boxsize2 + 24) * 2);

        PhysBody *b1 = new PhysBody(Vec2(x, y1), Shape(Vec2(boxsize1, boxsize1)));
        b1->calc_mass(masses[i]);

        PhysBody *b2 = new PhysBody(Vec2(x, y2), Shape(Vec2(boxsize2, boxsize2)));
        b1->calc_mass(masses[i]);

        scene->add(b1);
        scene->add(b2);
    }
}

void test_rolling(GameScene *scn) {
    PhysScene *scene = scn->physscene;
    GUI       *gui   = scn->gui;

    scene_clear_ext(scn, {0, 981}, 1);
    PhysBody *b = nullptr;

    // Floors

    b = scene->add(new PhysBody(Vec2(VIEW_W2 / 2.0 + 96, 158), Shape(Vec2(500, 16))));
    b->calc_mass(0);
    b->set_angle(DEG2RAD * (-8));

    b = scene->add(new PhysBody(Vec2(VIEW_W2 / 2.0 - 196, 330), Shape(Vec2(500, 16))));
    b->calc_mass(0);
    b->set_angle(DEG2RAD * (18));

    // b = scene->add(new PhysBody(Vec2(VIEW_W2 / 2.0 + 96, 530), Shape(Vec2(500, 16))));
    // b->calc_mass(0);
    // b->set_angle(DEG2RAD * (-30));

    // Balls

    b = scene->add(new PhysBody(Vec2(VIEW_W2 / 2.0 + 250, 32), Shape(18.0f)));
    b = scene->add(new PhysBody(Vec2(VIEW_W2 / 2.0 + 290, 0), Shape(24.0f)));
}

void test_springs(GameScene *scn) {
    PhysScene *scene = scn->physscene;
    GUI       *gui   = scn->gui;

    scene_clear_ext(scn, {0, 981}, 1);
    PhysBody *b = nullptr;

    float PARAM_DIST = 0;
    float PARAM_K    = 50;
    float PARAM_DAMP = 100;

    {
        // Curtain: grid of small bodies connected with springs

        const unsigned                       bw = 15;
        const unsigned                       bh = 10;
        std::vector<std::vector<PhysBody *>> curtain(bh, std::vector<PhysBody *>(bw, NULL));

        for (unsigned y = 0; y < bh; y++) {
            for (unsigned x = 0; x < bw; x++) {
                curtain[y][x] = new PhysBody(Vec2(300 + 20 * x, 160 + 16 * y), Shape(4));
                scene->add(curtain[y][x]);

                if (y == 0) {
                    curtain[y][x]->calc_mass(0);
                }
            }
        }
        for (unsigned y = 0; y < bh; y++) {
            for (unsigned x = 0; x < bw; x++) {
                if (y + 1 < bh)
                    scene->spring.push_back(Spring(curtain[y][x], curtain[y + 1][x], PARAM_DIST, PARAM_K, PARAM_DAMP));
                if (x + 1 < bw)
                    scene->spring.push_back(Spring(curtain[y][x], curtain[y][x + 1], PARAM_DIST, PARAM_K, PARAM_DAMP));
            }
        }
    }
}

void test_performance(GameScene *scn) {
    PhysScene *scene = scn->physscene;
    GUI       *gui   = scn->gui;

    scene_clear_ext(scn, {0, 981}, 1);
    PhysBody *b = nullptr;

    float PARAM_DIST = 1;
    float PARAM_K    = 1000;
    float PARAM_DAMP = 100;

    // Ground

    b = scene->add(new PhysBody(Vec2(8, VIEW_H2 / 2.0), Shape(Vec2(16, VIEW_H2))));
    b->calc_mass(0);
    b = scene->add(new PhysBody(Vec2(VIEW_W2 - 8, VIEW_H2 / 2.0), Shape(Vec2(16, VIEW_H2))));
    b->calc_mass(0);
    b = scene->add(new PhysBody(Vec2(VIEW_W2 / 2.0, VIEW_H2 - 8), Shape(Vec2(VIEW_W2, 16))));
    b->calc_mass(0);

    // Balls

    GUIButton *btn;
    btn = gui->add(new GUIButton(gui, {VIEW_W2 - 128.0, 48}, "Add 50 balls"));
    btn->reg_click_callback([](GUIButton *btn) {
        PhysScene *scene = btn->gui->scene->physscene;

        int K         = 50;
        int ball_size = 16;

        int L = 32;
        int R = VIEW_W2 - 32;
        int U = 32;
        int D = VIEW_H2 - 32;

        int balls_per_row = (R - L) / (int)(ball_size * 1);

        for (int i = 0; i < K; i++) {
            int row = i / balls_per_row;
            int col = i % balls_per_row;

            int x = L + col * (ball_size * 1) + (3 * i / balls_per_row);
            int y = U + row * (ball_size * 1);

            scene->add(new PhysBody(Vec2(x, y), Shape(ball_size)));
        }
    });
}

void test2(GameScene *gamescene) {
    gamescene->physscene->clear();
    gamescene->gui->clear();

    static std::vector<void (*)(GameScene *)> init_func_arr = {
        test_boxes_stacking, test_mass_proportions, test_rolling, test_springs, test_performance};

    if (scene_index >= init_func_arr.size())
        scene_index = 0;
    init_func_arr[scene_index](gamescene);
    gui_common(gamescene);
}