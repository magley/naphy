#pragma once

#include "rend/image.h"
#include "utility/input.h"
#include "gui/gui_checkbox.h"
#include "gui/gui_button.h"
#include <vector>

struct SDL_Window;

struct GUI {
	Image img_gui;
	Image img_font;

	double scale;

	SDL_Window* win;

	std::vector<GUICheckBox*> checkbox;
	std::vector<GUIButton*> button;

	GUI(SDL_Window* win, Image img_gui, Image img_font);
	~GUI();

	GUICheckBox* add(GUICheckBox* cb);
	GUIButton* add(GUIButton* btn);

	void update(const Input& input);
	void draw(const Input& input);
};