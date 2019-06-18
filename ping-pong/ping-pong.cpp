#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <SDL.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <random>
#include <iostream>
#include <chrono>

using namespace std;

#define SCREEN_W 800
#define SCREEN_H 600

SDL_Window *screen;
SDL_Renderer *renderer;

int alive = 1;

int main (int argc, char **argv)
{
	SDL_Event event;
	const Uint8 *keyboard_state_array;

	cout << chrono::high_resolution_clock::period::den << endl;

	SDL_Init(SDL_INIT_EVERYTHING);

	screen = SDL_CreateWindow("My Game Window",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_W, SCREEN_H,
		SDL_WINDOW_OPENGL);

	renderer = SDL_CreateRenderer(screen, -1, 0);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
	
	keyboard_state_array = SDL_GetKeyboardState(NULL);
	
	SDL_Rect rect;
	rect.x = 50;
	rect.y = 100;
	rect.w = 30;
	rect.h = 30;
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &rect);
	SDL_RenderPresent(renderer);
	
	while (alive) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					alive = 0;
					break;

				case SDL_KEYDOWN: {
					switch (event.key.keysym.sym) {
						case SDLK_SPACE: {
							cout << "espaço apertado" << endl;
							break;
						}
					}
					break;
				}
				
			}
		}
	}

	SDL_Quit();
	
	return 0;
}
