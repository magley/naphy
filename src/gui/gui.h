#pragma once

#include "rend/image.h"
#include "utility/input.h"
#include "gui/gui_checkbox.h"
#include "gui/gui_button.h"
#include "gui/gui_label.h"
#include <vector>

struct SDL_Window;

// The GUI object is in charge of all GUI components. Use add() to add new components.
struct GUI {
	// GUI elements image.
	Image img_gui;
	// Font image.
	Image img_font;
	// Reference to the window which the GUI is "attached" to. We need to use this in order to keep
	// the GUI elements at a constant scale, regardless of window/viewport scale.
	SDL_Window* win;
	// The final scale after adjusting the GUI elements' sizes to alway be 1:1 to the screen.
	// Mathematically, this is equal to: viewport_scale / window_scale. 
	double scale;
	// All Checkboxes registered for this GUI.
	std::vector<GUICheckBox*> checkbox;
	// All Buttons registered for this GUI.
	std::vector<GUIButton*> button;
	// All Labels registered for this GUI.
	std::vector<GUILabel*> label;

	// Construct a new GUI with the provided sprite sheets and window handle.
	// Please note that all 3 params' internal SDL_Renderer pointer must be THE SAME.
	GUI(SDL_Window* win, Image img_gui, Image img_font);
	~GUI();

	GUICheckBox* add(GUICheckBox* cb);
	GUIButton* add(GUIButton* btn);
	GUILabel* add(GUILabel* lbl);

	// Update all GUI components. Right now, it only handles basic user interaction (clicking).
	void update(const Input& input);
	// Draw all GUI components on screen. If some components handle hovering, that happens here.
	void draw(const Input& input);
};