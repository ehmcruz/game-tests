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
#define MAX_OBJS 1000

struct point_t {
	double x, y;
};

typedef point_t vector_t;

class obj_t
{
public:
	point_t pos;
	vector_t speed;
	double w, h;
};

SDL_Window *screen;
SDL_Renderer *renderer;

obj_t *objs[MAX_OBJS];
int nobjs = 0;

obj_t *player;

int alive = 1;

static void render ()
{
	int i;
	obj_t *o;
	SDL_Rect rect;

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	for (i=0; i<nobjs; i++) {
		o = objs[i];
		
		rect.x = o->pos.x;
		rect.y = o->pos.y;
		rect.w = o->w;
		rect.h = o->h;

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(renderer, &rect);
	}

	SDL_RenderPresent(renderer);
}

static void check_collision_boundaries (obj_t *o)
{
	if (o->pos.x < 0.0) {
		o->pos.x = 0.0;
		o->speed.x *= -1.0;
	}
	else if ((o->pos.x + o->w) > (double)SCREEN_W) {
		o->pos.x = (double)SCREEN_W - o->w;
		o->speed.x *= -1.0;
	}
	
	if (o->pos.y < 0.0) {
		o->pos.y = 0.0;
		o->speed.y *= -1.0;
	}
	else if ((o->pos.y + o->h) > (double)SCREEN_H) {
		o->pos.y = (double)SCREEN_H - o->h;
		o->speed.y *= -1.0;
	}
}

static void physics (double t)
{
	int i;
	obj_t *o;
	
	for (i=0; i<nobjs; i++) {
		o = objs[i];
		
		o->pos.x += o->speed.x * t;
		o->pos.y += o->speed.y * t;
		
		check_collision_boundaries(o);
	}
}

static void add_obj (obj_t *o)
{
	assert(nobjs < MAX_OBJS);
	
	objs[ nobjs++ ] = o;
}

static void init_game ()
{
	player = new obj_t();
	player->pos.x = SCREEN_W / 2;
	player->pos.y = SCREEN_H / 2;
	player->speed.x = 0.0;
	player->speed.y = 0.0;
	player->w = 30.0;
	player->h = 30.0;
	
	add_obj(player);
}

int main (int argc, char **argv)
{
	SDL_Event event;
	const Uint8 *keyboard_state_array;
	chrono::high_resolution_clock::time_point tbegin, tend;
	double elapsed;

	cout << chrono::high_resolution_clock::period::den << endl;

	SDL_Init(SDL_INIT_EVERYTHING);

	screen = SDL_CreateWindow("My Game Window",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_W, SCREEN_H,
		SDL_WINDOW_OPENGL);

	renderer = SDL_CreateRenderer(screen, -1, 0);
	
	init_game();

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
	
	keyboard_state_array = SDL_GetKeyboardState(NULL);
		
	while (alive) {
		tbegin = chrono::high_resolution_clock::now();
		
		#define inc 0.5
		
		if (keyboard_state_array[SDL_SCANCODE_UP])
			player->speed.y -= inc;
		if (keyboard_state_array[SDL_SCANCODE_DOWN])
			player->speed.y += inc;
		if (keyboard_state_array[SDL_SCANCODE_LEFT])
			player->speed.x -= inc;
		if (keyboard_state_array[SDL_SCANCODE_RIGHT])
			player->speed.x += inc;
			
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
		
		do {
			tend = chrono::high_resolution_clock::now();
			chrono::duration<double> elapsed_ = chrono::duration_cast<chrono::duration<double>>(tend - tbegin);
			elapsed = elapsed_.count();
		} while (elapsed < 0.01);
		
		physics(elapsed);
		render();
	}

	SDL_Quit();
	
	return 0;
}
