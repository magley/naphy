#pragma once

#include "rend/image.h"
#include "utility/input.h"
#include "gui/gui_checkbox.h"
#include "gui/gui_button.h"
#include "gui/gui_label.h"
#include <vector>

struct GameScene;
struct SDL_Window;

struct GUI {
	GameScene* scene; // GameScene::GameScene() is in charge of setting this value.
	SDL_Window* win;
	
	Image img_gui;
	Image img_font;
	double scale;	// Mathematically, this is: viewport_scale / window_scale. 

	std::vector<GUICheckBox*> checkbox;
	std::vector<GUIButton*> button;
	std::vector<GUILabel*> label;

	// All 3 params' internal SDL_Renderer pointer must be THE SAME.
	GUI(SDL_Window* win, Image img_gui, Image img_font);
	~GUI();

	GUICheckBox* add(GUICheckBox* cb);
	GUIButton* add(GUIButton* btn);
	GUILabel* add(GUILabel* lbl);

	void update(const Input& input);
	void draw(const Input& input);
	void clear();
};