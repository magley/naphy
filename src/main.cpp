#include "SDL2/SDL_render.h"
#include "SDL2/SDL_video.h"
#include "test/test.h"
#include "test/test2.h"

static const unsigned WIN_W = 1002;
static const unsigned WIN_H = 810;
static const unsigned WIN_X = ((1920 - WIN_W) / 2);
static const unsigned WIN_Y = ((1080 - WIN_H) / 2);

int main(int, char **) {
    SDL_Window   *win;
    SDL_Renderer *rend;
    SDL_Event     ev;
    unsigned      running = 1;

    SDL_Init(SDL_INIT_VIDEO);
    win  = SDL_CreateWindow("naphy dev.2024.05.24", WIN_X, WIN_Y, WIN_W, WIN_H, SDL_WINDOW_OPENGL);
    rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
    SDL_RenderSetLogicalSize(rend, VIEW_W, VIEW_H);

    //----------------------------------------------------------------------------------- Resources

    Image font(rend, "../res/font.png");
    Image gui_atlas(rend, "../res/gui.png");
    Image img_atlas(rend, "../res/spr_atlas.png");
    Image gradient(rend, "../res/gradient1.png");
    sprites_init(&img_atlas);

    //-------------------------------------------------------------------------------- Engine stuff

    Input input(win);

    //--------------------------------------------------------------------------------------- Scene

    GameScene scene =
        GameScene(new PhysScene(rend, 1.0 / 60.0, Vec2(WIN_W, WIN_H), Vec2(WIN_W, WIN_H), Vec2(0, 981), 16),
                  new GUI(win, gui_atlas, font));
    test2(&scene);

    while (running) {
        PhysScene *const physscene = scene.physscene;
        GUI *const       gui       = scene.gui;

        //------------------------------------------------------------------------------ Pre-update

        physscene->pre_update();

        do {
            if (ev.type == SDL_QUIT)
                running = false;
        } while (SDL_PollEvent(&ev));

        input.update();
        gui->update(input);

        //---------------------------------------------------------------------------------- Update

        while (physscene->timing.accumulator >= physscene->timing.dt) {
            for (Entity &e : scene.entity) {
                e.update(&input);
            }

            physscene->update();
            physscene->timing.accumulator -= physscene->timing.dt;
            physscene->timing.total += physscene->timing.dt / physscene->timing.scale;
            physscene->timing.ticks_phys++;
        }
        physscene->timing.ticks++;

        //------------------------------------------------------------------------------------ Draw

        SDL_SetRenderDrawColor(rend, 19, 18, 37, 255);
        SDL_RenderClear(rend);
        SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);

        for (Entity &e : scene.entity) {
            e.draw(&scene, &img_atlas);
        }
        scene.flush_sprites();
        physscene->draw();
        gui->draw(input);

        {
            std::stringstream ss;
            ss << "naphy Testbed ";
            ss << (int)scene.physscene->timing.total << "s";
            SDL_SetWindowTitle(win, ss.str().c_str());

            if (input.key_press(SDL_SCANCODE_F1)) {
                SDL_Surface *s =
                    SDL_CreateRGBSurface(0, WIN_W, WIN_H, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
                SDL_RenderReadPixels(rend, NULL, SDL_PIXELFORMAT_ARGB8888, s->pixels, s->pitch);
                SDL_SaveBMP(s, ("../screenshot/" + ss.str() + ".bmp").c_str());
                SDL_FreeSurface(s);
            }
        }

        SDL_RenderPresent(rend);
    }

    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}