#pragma once

#include "rend/rend.h"
#include "rend/image.h"
#include "utility/mathutil.h"
#include <string>

struct Scene;
struct GUI;
struct GUIButton;

typedef void (*GUIButtonCallback)(Scene* scene, GUIButton* btn);

struct GUIButton {
	Vec2 pos;
	// true if the button is held down, false otherwise.
	// To check for a single click, register a click_callback.
	bool clicked;
	std::string hover_text;


	GUIButton();
	GUIButton(GUI* gui, Vec2 pos, std::string hover_text = "");

	void draw(const Image& img_gui);

	//=============================================================================================
	// Callbacks

	GUI* gui;
	Scene* scene;

	GUIButtonCallback click_callback;
	void reg_click_callback(GUIButtonCallback func, Scene* scene);

};