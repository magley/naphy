#include "gui_button.h"
#include "gui.h"

GUIButton::GUIButton() {
	pos = Vec2(0, 0);
	click_callback = NULL;
	gui = NULL;
	scene = NULL;
	clicked = false;
}

GUIButton::GUIButton(GUI* gui, Vec2 pos, std::string hover_text) {
	this->pos = pos;
	this->hover_text = hover_text;
	this->click_callback = NULL;
	this->clicked = false;
	this->gui = gui;
}

void GUIButton::draw(const Image& img_gui) {
	img_gui.draw(pos.x, pos.y, 1.0 / gui->scale, 0, 24 + 24 * (clicked ? 1 : 0), 48, 24);
}

void GUIButton::reg_click_callback(GUIButtonCallback func, Scene* scene) {
	this->click_callback = func;
	this->scene = scene;
}