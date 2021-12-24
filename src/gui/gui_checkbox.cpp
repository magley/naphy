#include "gui_checkbox.h"
#include "gui.h"

GUICheckBox::GUICheckBox() {
	pos = Vec2(0, 0);
}

GUICheckBox::GUICheckBox(Vec2 pos, std::string hover_text) {
	this->pos = pos;
	this->hover_text = hover_text;
}

void GUICheckBox::draw(const Image& img_gui) {
	img_gui.draw(pos.x, pos.y, 24 * (checked ? 1 : 0), 0, 24, 24);
}