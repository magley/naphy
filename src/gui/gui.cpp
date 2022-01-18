#include "gui.h"

#include <cassert>
#include <iostream>

#include "SDL2/SDL_mouse.h"

GUI::GUI(SDL_Window* win, Image img_gui, Image img_font) {
	assert(SDL_GetRenderer(win) == img_gui.rend && img_gui.rend == img_font.rend);

	this->scene = NULL;
	this->img_font = img_font;
	this->img_gui = img_gui;
	this->win = win;
	this->scale = 1;
}

GUI::~GUI() {
	clear();
}

void GUI::clear() {
	for (auto o : checkbox) delete o;
	checkbox.clear();
	for (auto o : button) delete o;
	button.clear();
	for (auto o : label) delete o;
	label.clear();
}

GUICheckBox* GUI::add(GUICheckBox* cb) {
	checkbox.push_back(cb);
	cb->gui = this;
	return cb;
}

GUIButton* GUI::add(GUIButton* btn) {
	button.push_back(btn);
	btn->gui = this;
	return btn;
}

GUILabel* GUI::add(GUILabel* lbl) {
	label.push_back(lbl);
	lbl->gui = this;
	return lbl;
}

void GUI::update(const Input& input) {
	// Viewport may have changed, recalculate scale.

	int win_w, win_h;
	int view_w, view_h;
	SDL_GetWindowSize(win, &win_w, &win_h);
	SDL_RenderGetLogicalSize(SDL_GetRenderer(win), &view_w, &view_h);
	scale = (double)view_w / (double)win_w;

	// Update CheckBox.

	for (GUICheckBox* o : checkbox) {
		const Vec2 opos = o->pos * scale;
		const Vec2 osize = o->size * scale;
		if (input.mouse_in_bounds(opos.x, opos.x + osize.x, opos.y, opos.y + osize.y) 
		&& input.mouse_press(SDL_BUTTON_LMASK)) {
			o->checked = !o->checked;

			if (o->click_callback)
				o->click_callback(o);

			if (o->toggle_target)
				*(o->toggle_target) = !*(o->toggle_target);
		}
	}

	// Update Button.

	for (GUIButton* o : button) {
		const Vec2 opos = o->pos * scale;
		const Vec2 osize = o->size * scale;
		if (input.mouse_in_bounds(opos.x, opos.x + osize.x, opos.y, opos.y + osize.y)) {
			if (o->clicked && input.mouse_up(SDL_BUTTON_LMASK)) {
				if (o->click_callback) 
					o->click_callback(o);
			}

			if (o->clicked)
				o->clicked = input.mouse_down(SDL_BUTTON_LMASK);
			else
				o->clicked = input.mouse_press(SDL_BUTTON_LMASK);
		} else {
			o->clicked = false;
		}
	}

	// Update Label (nothing to do).
}

void GUI::draw(const Input& input) {
	for (auto o : checkbox)
		o->draw(img_gui);
	for (auto o : button)
		o->draw(img_gui);
	for (auto o : label)
		o->draw(img_font);

	// CheckBox hover text.

	for (auto o : checkbox) {
		const Vec2 opos = o->pos * scale;
		const Vec2 osize = o->size * scale;
		if (input.mouse_in_bounds(opos.x, opos.x + osize.x, opos.y, opos.y + osize.y)) {
			if (o->hover_text != "") {
				draw_text(
					input.mouse_x + 12 * scale, input.mouse_y + 12 * scale, scale,
					img_font,
					o->hover_text,
					{255, 255, 0, 255},
					{0, 0, 128, 255});
			}
		}
	}

	// Button hover text.

	for (auto o : button) {
		const Vec2 opos = o->pos * scale;
		const Vec2 osize = o->size * scale;
		if (input.mouse_in_bounds(opos.x, opos.x + osize.x, opos.y, opos.y + osize.y)) {
			if (o->hover_text != "") {
				draw_text(
					input.mouse_x + 12 * scale, input.mouse_y + 12 * scale, scale,
					img_font,
					o->hover_text,
					{255, 255, 0, 255},
					{0, 0, 128, 255});
			}
		}
	}
}