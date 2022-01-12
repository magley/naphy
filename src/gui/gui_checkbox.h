#pragma once

#include "rend/rend.h"
#include "rend/image.h"
#include "utility/mathutil.h"
#include <string>

struct GUI;
struct PhysScene;
struct GUICheckBox;

typedef void (*GUICheckBoxCallback)(GUICheckBox* btn);

// Clickable GUI checkbox that toggles its state whenever it's pushed. 
struct GUICheckBox {
	// GUI::add() is in charge of setting this value.
	GUI* gui;
	// Native position on screen.
	Vec2 pos;
	// Native size.
	Vec2 size;
	// true if this checkbox is toggled, false otherwise.
	bool checked;
	// Text that shows up under the cursor whenever the component is hovered.
	std::string hover_text;

	GUICheckBox();
	GUICheckBox(GUI* gui, Vec2 pos, std::string hover_text = "");

	void draw(const Image& img_gui);

	// If clicking just toggles some value, you can pass that directly.
	bool* toggle_target;
	// Register a pointer to an integer whose value will be toggled on click.
	// You can use both this and a callback for the same or seperate logic.
	// @param target The integer that will be toggled based on 'checked'.
	// @param match_initially If true, set target's value to match 'checked'.
	void reg_toggle_target(bool* target, bool match_initially);

	// Callback that gets invoked every time the checkbox is toggled. NULL is a valid value.
	GUICheckBoxCallback click_callback;
	// Assign a new click callback to this component.
	void reg_click_callback(GUICheckBoxCallback func);
};