#include "gui_button.h"
#include "gui.h"

GUIButton::GUIButton() {
	pos = Vec2(0, 0);
	click_callback = NULL;
	gui = NULL;
	scene = NULL;
}

GUIButton::GUIButton(Vec2 pos, std::string hover_text) {
	this->pos = pos;
	this->hover_text = hover_text;
	click_callback = NULL;
}

void GUIButton::draw(const Image& img_gui) {
	img_gui.draw(pos.x, pos.y, 0, 24 + 24 * (clicked ? 1 : 0), 48, 24);
}

void GUIButton::reg_click_callback(GUIButtonCallback func, GUI* gui, Scene* scene) {
	this->click_callback = func;
	this->gui = gui;
	this->scene = scene;
}