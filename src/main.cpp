#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include <string>
#include <iostream>

#include "naphy/shape.h"
#include "naphy/physbody.h"
#include "rend/rend.h"
#include "rend/image.h"


void draw_text(int x, int y, Image& font_sheet, std::string text) {
	const unsigned char_w = 12;
	const unsigned char_h = 20;
	const unsigned chars_per_line = font_sheet.w / char_w;
	for (unsigned i = 0; i < text.size(); i++) {

		const char c = text[i];

		const int char_x = (c % chars_per_line) * char_w;
		const int char_y = (c / chars_per_line) * char_h;
		font_sheet.draw(x + i * char_w, y, char_x, char_y, char_w, char_h);
	}
}


int main(int, char**) {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* win = SDL_CreateWindow("", 100, 100, 640, 480, SDL_WINDOW_OPENGL);
	SDL_Renderer* rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);


	PhysBody bod = PhysBody(Vec2(80, 80), Shape(40));
	Image font(rend, "res/font.png");

	while (1) {
		SDL_Event ev;
		SDL_PollEvent(&ev);

		if (ev.type == SDL_QUIT)
			break;


		SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
		SDL_RenderClear(rend);
		SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);


		draw_text(0, 0, font, "123456 qwerty <>?.{}[]'\\ *()_+^%$#@!~`");
		bod.draw(rend);
		

		SDL_RenderPresent(rend);
	}


	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;
}