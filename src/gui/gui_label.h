#pragma once

#include <string>
#include "SDL2/SDL.h"
#include "rend/rend.h"
#include "rend/image.h"

struct GUI;

// Single line of text. Scales automatically. This is more or less draw_text() with extra steps.
struct GUILabel {
	// GUI::add() is in charge of setting this value.
	GUI* gui;
	// Native position on screen.
	Vec2 pos;
	// Text that's displayed.
	std::string text;
	// Color of the text.
	SDL_Color col_text;
	// Color of the background.
	SDL_Color col_bg;

	GUILabel();
	GUILabel(Vec2 pos, std::string text);
	GUILabel(Vec2 pos, std::string text, SDL_Color col_text, SDL_Color col_bg);

	// This should be called by the GUI.
	void draw(Image& img_font);
};