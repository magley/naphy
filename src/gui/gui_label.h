#pragma once

#include <string>
#include "SDL2/SDL.h"
#include "rend/rend.h"
#include "rend/image.h"

struct GUI;

// Single line of text. Scales automatically. This is more or less draw_text() with extra steps.
struct GUILabel {
	// Poitner to the owning GUI.
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
	GUILabel(GUI* gui, Vec2 pos, std::string text);
	GUILabel(GUI* gui, Vec2 pos, std::string text, SDL_Color col_text, SDL_Color col_bg);

	void draw(Image& img_font);
};