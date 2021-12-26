#include "gui.h"
#include "SDL2/SDL_mouse.h"
#include <iostream>

GUI::GUI(Image img_gui, Image img_font) {
	this->img_font = img_font;
	this->img_gui = img_gui;
}

GUI::~GUI() {
	for (auto o : checkbox)
		delete o;
	checkbox.clear();
}

GUICheckBox* GUI::add(GUICheckBox* cb) {
	checkbox.push_back(cb);
	return cb;
}

GUIButton* GUI::add(GUIButton* btn) {
	button.push_back(btn);
	return btn;
}

void GUI::update(const Input& input) {
	for (GUICheckBox* o : checkbox) {
		if (input.mouse_in_bounds(o->pos.x, o->pos.x + 24, o->pos.y, o->pos.y + 24)
		&& input.mouse_press(SDL_BUTTON_LMASK)) {
			o->checked = !o->checked;
		}
	}

	for (GUIButton* o : button) {
		if (input.mouse_in_bounds(o->pos.x, o->pos.x + 48, o->pos.y, o->pos.y + 24)) {
			if (o->clicked && input.mouse_up(SDL_BUTTON_LMASK)) {
				if (o->click_callback != NULL) {
					o->click_callback(o->gui, o->scene, o);
				}
			}

			if (o->clicked)
				o->clicked = input.mouse_down(SDL_BUTTON_LMASK);
			else
				o->clicked = input.mouse_press(SDL_BUTTON_LMASK);
		} else {
			o->clicked = false;
		}
	}
}

void GUI::draw(const Input& input) {
	for (auto o : checkbox) 
		o->draw(img_gui);
	for (auto o : button) 
		o->draw(img_gui);

	for (auto o : checkbox) {
		if (input.mouse_in_bounds(o->pos.x, o->pos.x + 24, o->pos.y, o->pos.y + 24)) {
			if (o->hover_text != "") {
				draw_text(input.mouse_x + 12, input.mouse_y + 12, img_font, o->hover_text, {255, 255, 0, 255}, {0, 0, 128, 255});
			}
		}
	}
	for (auto o : button) {
		if (input.mouse_in_bounds(o->pos.x, o->pos.x + 48, o->pos.y, o->pos.y + 24)) {
			if (o->hover_text != "") {
				draw_text(input.mouse_x + 12, input.mouse_y + 12, img_font, o->hover_text, {255, 255, 0, 255}, {0, 0, 128, 255});
			}
		}
	}
}