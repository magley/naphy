#pragma once

#include "rend/rend.h"
#include "rend/image.h"
#include "utility/mathutil.h"
#include <string>

struct GUICheckBox {
	Vec2 pos;
	bool checked;
	std::string hover_text;

	GUICheckBox();
	GUICheckBox(Vec2 pos, std::string hover_text = "");

	void draw(const Image& img_gui);
};