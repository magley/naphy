#pragma once

#include "rend/rend.h"
#include "rend/image.h"
#include "utility/mathutil.h"
#include <string>

struct PhysScene;
struct GUI;
struct GUIButton; // forward decl for the callback.

typedef void (*GUIButtonCallback)(GUIButton* btn);

// Clickable GUI button that fires a callback whenever it's pushed. 
struct GUIButton {
	// GUI::add() is in charge of setting this value.
	GUI* gui;
	// Native position on screen.
	Vec2 pos;
	// Native size.
	Vec2 size;
	// true if the button is held down, false otherwise.
	// Don't use this to check if the button is clicked.
	// To check for a click, register a click_callback.
	bool clicked;
	// Text that shows up under the cursor whenever the component is hovered.
	std::string hover_text;


	GUIButton();
	GUIButton(GUI* gui, Vec2 pos, std::string hover_text);

	void draw(const Image& img_gui);

	//=============================================================================================
	// Callbacks


	// Callback that gets invoked every time the button is clicked. NULL is a valid value.
	GUIButtonCallback click_callback;
	// Assign a new click callback to this component.
	void reg_click_callback(GUIButtonCallback func);
};