#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "game/cdrifter.h"
#include "gui/gui.h"
#include "gui/gui_checkbox.h"
#include "gui/gui_label.h"
#include "naphy/physscene.h"
#include "rend/image.h"
#include "rend/rend.h"
#include "utility/input.h"

#include "game/gamescene.h"


static unsigned VIEW_W = 100;
static unsigned VIEW_H = 100;

void start_scene(GameScene* gamescene);