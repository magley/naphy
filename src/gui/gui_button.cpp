#include "gui_button.h"
#include "gui.h"

GUIButton::GUIButton() {
	pos = Vec2(0, 0);
	size = Vec2(48, 24);
	clicked = false;
	click_callback = NULL;
	gui = NULL;
}

GUIButton::GUIButton(GUI* gui, Vec2 pos, std::string hover_text) {
	this->pos = pos;
	this->size = Vec2(48, 24);
	this->hover_text = hover_text;
	this->clicked = false;
	this->click_callback = NULL;
	this->gui = NULL;
}

void GUIButton::draw(const Image& img_gui) {
	img_gui.draw(
		pos.x * gui->scale, pos.y * gui->scale,
		size.x * gui->scale, size.y * gui->scale,
		0, 24 + 24 * (int)clicked,
		48, 24
	);
}

void GUIButton::reg_click_callback(GUIButtonCallback func) {
	this->click_callback = func;
}