#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <SDL.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <random>
#include <iostream>
#include <chrono>

using namespace std;

#define SCREEN_W 1300
#define SCREEN_H 900

#define MAX_OBJS 1000

SDL_Window *screen;
SDL_Renderer *renderer;

static bool alive = true;

struct object_t {
	double x, y, size, vx, vy;
	Uint8 r, g, b;
};

static object_t *player;

static object_t *objs[MAX_OBJS];

static int nobjs = 0;

static void add_obj (object_t *o)
{
	assert(nobjs < MAX_OBJS);
	objs[ nobjs++ ] = o;
}

static void render ()
{
	SDL_Rect rect;

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	rect.x = player->x;
	rect.y = player->y;
	rect.w = player->size;
	rect.h = player->size;

	SDL_SetRenderDrawColor(renderer, player->r, player->g, player->b, 255);
	SDL_RenderFillRect(renderer, &rect);

	SDL_RenderPresent(renderer);
}

static void check_collision_boundaries (object_t *o)
{
	if (o->x < 0.0) {
		o->x = 0.0;
		o->vx *= -1.0;
	}
	else if ((o->x + o->size) > (double)SCREEN_W) {
		o->x = (double)SCREEN_W - o->size;
		o->vx *= -1.0;
	}
	
	if (o->y < 0.0) {
		o->y = 0.0;
		o->vy *= -1.0;
	}
	else if ((o->y + o->size) > (double)SCREEN_H) {
		o->y = (double)SCREEN_H - o->size;
		o->vy *= -1.0;
	}
}

static void physics (double t)
{
	int i;
	object_t *o;

	for (i=0; i<nobjs; i++) {
		o = objs[i];
		o->x += o->vx*t;
		o->y += o->vy*t;

		check_collision_boundaries(o);
	}
}

int main (int argc, char **argv)
{
	SDL_Event event;
	const Uint8 *keyboard_state_array;
	double elapsed, ds, speed, fps;
	chrono::high_resolution_clock::time_point tbegin, tend;

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);
	auto width = DM.w;
	auto height = DM.h;

	cout << "w: " << width << endl;
	cout << "h: " << height << endl;

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

	fps = 100;

	player = new object_t;

	add_obj(player);

	player->x = SCREEN_W / 2;
	player->y = SCREEN_H / 2;
	player->vx = 0.0;
	player->vy = 0.0;
	player->size = 50;
	player->r = 255;
	player->g = 0;
	player->b = 0;

	speed = 400;

	while (alive) {
		tbegin = chrono::high_resolution_clock::now();

		/*
			a = dv / dt
			dv = a * dt
		*/

		double ac = 100.0;

		if (keyboard_state_array[SDL_SCANCODE_UP])
			player->vy -= ac*elapsed;
		if (keyboard_state_array[SDL_SCANCODE_DOWN])
			player->vy += ac*elapsed;

		if (keyboard_state_array[SDL_SCANCODE_LEFT])
			player->vx -= ac*elapsed;
		if (keyboard_state_array[SDL_SCANCODE_RIGHT])
			player->vx += ac*elapsed;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					alive = 0;
					break;
			}
		}

		physics(elapsed);
		render();

		do {
			tend = chrono::high_resolution_clock::now();
			chrono::duration<double> elapsed_ = chrono::duration_cast<chrono::duration<double>>(tend - tbegin);
			elapsed = elapsed_.count();
		} while (elapsed < (1.0 / fps));
	}

	SDL_Quit();

	return 0;
}