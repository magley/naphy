#include "gui_checkbox.h"
#include "gui.h"

GUICheckBox::GUICheckBox() {
	pos = Vec2(0, 0);
	size = Vec2(24, 24);
	checked = false;
	click_callback = NULL;
	gui = NULL;
	toggle_target = NULL;
}

GUICheckBox::GUICheckBox(GUI* gui, Vec2 pos, std::string hover_text) {
	this->pos = pos;
	this->size = Vec2(24, 24);
	this->hover_text = hover_text;
	this->checked = false;
	this->click_callback = NULL;
	this->gui = NULL;
	this->toggle_target = NULL;
}

void GUICheckBox::draw(const Image& img_gui) {
	img_gui.draw(
		pos.x * gui->scale, pos.y * gui->scale,
		size.x * gui->scale, size.y * gui->scale,
		24 * (int)checked, 0,
		24, 24
	);
}

void GUICheckBox::reg_click_callback(GUICheckBoxCallback func) {
	this->click_callback = func;
}

void GUICheckBox::reg_toggle_target(bool* target, bool match_initially) {
	this->toggle_target = target;
	if (match_initially)
		*target = checked;
}