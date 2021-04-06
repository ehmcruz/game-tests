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

static object_t *forrest;

static object_t *objs[MAX_OBJS];

static int nobjs = 0;

enum class game_state_t {
	before,
	running,
	finished
};

game_state_t game_state;

std::default_random_engine rgenerator;
std::uniform_real_distribution<double> rdistribution(0.0,1.0);

double get_random_0_1 ()
{
	return rdistribution(rgenerator);
}

double get_random (double max)
{
	return ((rdistribution(rgenerator) - 0.5) * 2.0 * max);
}

static void add_obj (object_t *o)
{
	assert(nobjs < MAX_OBJS);
	objs[ nobjs++ ] = o;
}

static void render ()
{
	int i;
	object_t *o;
	SDL_Rect rect;

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	for (i=0; i<nobjs; i++) {
		o = objs[i];
		
		rect.x = o->x;
		rect.y = o->y;
		rect.w = o->size;
		rect.h = o->size;

		SDL_SetRenderDrawColor(renderer, o->r, o->g, o->b, 255);
		SDL_RenderFillRect(renderer, &rect);
	}

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

inline int check_rect_horizontal_collision (object_t *left, object_t *right)
{
	return ((left->x + left->size) >= right->x);
}

inline int check_rect_vertical_collision (object_t *top, object_t *bottom)
{
	return ((top->y + top->size) >= bottom->y);
}

int check_obj_collision (object_t *a, object_t *b)
{
	int r = 0;

	if (a->x < b->x && check_rect_horizontal_collision(a, b)) {
		if (a->y < b->y && check_rect_vertical_collision(a, b))
			r = 1;
		else if (a->y >= b->y && check_rect_vertical_collision(b, a))
			r = 1;
	}
	else if (a->x >= b->x && check_rect_horizontal_collision(b, a)) {
		if (a->y < b->y && check_rect_vertical_collision(a, b))
			r = 1;
		else if (a->y >= b->y && check_rect_vertical_collision(b, a))
			r = 1;
	}

	return r;
}

void objs_collided (object_t *a, object_t *b)
{
	a->r = 0;
	a->g = 0;
	a->b = 255;

	b->r = 0;
	b->g = 0;
	b->b = 255;

	game_state = game_state_t::finished;
}

void physics_check_collisions ()
{
	int i, j;
	object_t *a, *b;

	for (i=0; i<nobjs; i++) {
		a = objs[i];

		for (j=i+1; j<nobjs; j++) {
			b = objs[j];

			if (check_obj_collision(a, b))
				objs_collided(a, b);
		}
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

	physics_check_collisions();
}

inline double absf (double v)
{
	return (v >= 0.0) ? v : (v * -1.0);
}

static void run_forrest ()
{
	double dx = forrest->x - player->x;
	double dy = forrest->y - player->y;

	forrest->vx += dx / 15.0;
	forrest->vy += dy / 15.0;

	if (absf(dx) <= 100.0 && absf(dy) <= 100.0) {
		if (get_random_0_1() <= 1.0) {
			forrest->vx += get_random(100.0);
			forrest->vy += get_random(100.0);
		}
	}
}

int main (int argc, char **argv)
{
	SDL_Event event;
	const Uint8 *keyboard_state_array;
	double elapsed, fps;
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

	elapsed = 0.0;
	fps = 100;

	game_state = game_state_t::before;

	player = new object_t;

	player->x = SCREEN_W / 2;
	player->y = SCREEN_H / 2;
	player->vx = 0.0;
	player->vy = 0.0;
	player->size = 50;
	player->r = 255;
	player->g = 0;
	player->b = 0;

	add_obj(player);

	forrest = new object_t;

	forrest->x = SCREEN_W / 4;
	forrest->y = SCREEN_H / 2;
	forrest->vx = 0.0;
	forrest->vy = 0.0;
	forrest->size = 30;
	forrest->r = 0;
	forrest->g = 255;
	forrest->b = 0;

	add_obj(forrest);

	while (alive) {
		tbegin = chrono::high_resolution_clock::now();

		/*
			a = dv / dt
			dv = a * dt
		*/

		double ac = 100.0;

		if (game_state == game_state_t::running) {
			if (keyboard_state_array[SDL_SCANCODE_UP])
				player->vy -= ac*elapsed;
			if (keyboard_state_array[SDL_SCANCODE_DOWN])
				player->vy += ac*elapsed;

			if (keyboard_state_array[SDL_SCANCODE_LEFT])
				player->vx -= ac*elapsed;
			if (keyboard_state_array[SDL_SCANCODE_RIGHT])
				player->vx += ac*elapsed;
			
			run_forrest();

			physics(elapsed);
		}

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					alive = 0;
					break;

				case SDL_KEYDOWN: {
					switch (event.key.keysym.sym) {
						case SDLK_SPACE: {
							game_state = game_state_t::running;
							break;
						}
					}

					break;
				}
			}
		}

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