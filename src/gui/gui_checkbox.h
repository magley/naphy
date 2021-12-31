#pragma once

#include "rend/rend.h"
#include "rend/image.h"
#include "utility/mathutil.h"
#include <string>

struct GUI;

// Clickable GUI checkbox that toggles its state whenever it's pushed. 
struct GUICheckBox {
	// Pointer to the owning GUI.
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
};