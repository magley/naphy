#include "gui_label.h"
#include "gui.h"
#include "rend/image.h"

GUILabel::GUILabel() {
	gui = NULL;
	pos = {0, 0};
	text = "";
	col_text = {255, 255, 255, 255};
	col_bg = {0, 0, 0, 0};
}

GUILabel::GUILabel(GUI* gui, Vec2 pos, std::string text) {
	this->gui = gui;
	this->pos = pos;
	this->text = text;
	this->col_text = {255, 255, 255, 255};
	this->col_bg = {0, 0, 0, 0};
}

GUILabel::GUILabel(GUI* gui, Vec2 pos, std::string text, SDL_Color col_text, SDL_Color col_bg) {
	this->gui = gui;
	this->pos = pos;
	this->text = text;
	this->col_text = col_text;
	this->col_bg = col_bg;
}

void GUILabel::draw(Image& img_font) {
	draw_text(pos.x * gui->scale, pos.y * gui->scale, gui->scale, img_font, text, col_text, col_bg);
}