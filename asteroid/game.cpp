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

#define MAX_OBJS 1000
#define SCREEN_W 800
#define SCREEN_H 600
#define OBJ_SIZE 20.0

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

using namespace std;

volatile int alive = 1;

SDL_Window *screen;
SDL_Renderer *renderer;

int nobjs = 0;

enum object_type_t {
	OBJ_PLAYER,
	OBJ_BULLET,
	OBJ_ASTEROID,
	OBJ_DESTROYED,
};

class object_t
{
public:
	double vx;
	double vy;
	double x;
	double y;
	double size;
	object_type_t type;
	Uint8 r, g, b;

	object_t(object_type_t type, double x, double y, Uint8 r, Uint8 g, Uint8 b) {
		this->vx = 0.0;
		this->vy = 0.0;
		this->type = type;
		this->x = x;
		this->y = y;
		this->size = OBJ_SIZE;
		this->r = r;
		this->g = g;
		this->b = b;
	}
};

object_t *objs[MAX_OBJS];
object_t *player;

std::default_random_engine rgenerator;
std::uniform_real_distribution<double> rdistribution(0.0,1.0);

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

void destroy_asteroid (object_t *asteroid)
{
	asteroid->type = OBJ_DESTROYED;
	asteroid->r = 100;
	asteroid->g = 100;
	asteroid->b = 100;
	asteroid->size = OBJ_SIZE / 10.0;
}

void init_objs_list ()
{
	int i;

	for (i=0; i<MAX_OBJS; i++)
		objs[i] = NULL;
}

void add_obj (object_t *o)
{
	assert(nobjs < MAX_OBJS);
	objs[ nobjs++ ] = o;
}

void init_game ()
{
	init_objs_list();

	player = new object_t(OBJ_PLAYER, SCREEN_W / 2.0, SCREEN_H / 2.0, 200, 0, 0);

	add_obj(player);
}

void render ()
{
	int i;
	SDL_Rect rect;
	object_t *o;

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

void physics_check_collisions ()
{
	int i, j;
	object_t *a, *b;

	for (i=0; i<nobjs; i++) {
		a = objs[i];

		for (j=i+1; j<nobjs; j++) {
			b = objs[j];

			if (a->type == OBJ_BULLET && b->type == OBJ_ASTEROID && check_obj_collision(a, b))
				destroy_asteroid(b);
			else if (b->type == OBJ_BULLET && a->type == OBJ_ASTEROID && check_obj_collision(a, b))
				destroy_asteroid(a);
		}
	}
}

void physics (double t)
{
	int i;
	object_t *o;

	for (i=0; i<nobjs; i++) {
		o = objs[i];
		o->x += o->vx*t;
		o->y += o->vy*t;

		if (o->type != OBJ_BULLET) {
			if (o->x < 0.0) {
				o->vx *= -1.0;
				o->x = 0.0;
			}
			else if (o->x > ((double)SCREEN_W - o->size)) {
				o->vx *= -1.0;
				o->x = (double)SCREEN_W - o->size;
			}

			if (o->y < 0.0) {
				o->vy *= -1.0;
				o->y = 0.0;
			}
			else if (o->y > ((double)SCREEN_H - o->size)) {
				o->vy *= -1.0;
				o->y = (double)SCREEN_H - o->size;
			}
		}
	}

	physics_check_collisions();
}

int physics_thread_handler (void* data)
{
	chrono::high_resolution_clock::time_point tbegin, tend;
	double elapsed;
	int i;

	elapsed = 0.0;
	i++;

	while (1) {
		tbegin = chrono::high_resolution_clock::now();

		physics(elapsed);

		do {
			tend = chrono::high_resolution_clock::now();
			chrono::duration<double> elapsed_ = chrono::duration_cast<chrono::duration<double>>(tend - tbegin);
			elapsed = elapsed_.count();
		} while (elapsed < 0.0001);

		if (unlikely(i++ >= 1000)) {
			i = 0;
			cout << "elapsed: " << elapsed << endl;
		}
	}

	return 0;
}

double get_random_x_pos ()
{
	return rdistribution(rgenerator) * (double)SCREEN_W;
}

double get_random_y_pos ()
{
	return rdistribution(rgenerator) * (double)SCREEN_H;
}

double get_random_speed ()
{
	return (rdistribution(rgenerator) - rdistribution(rgenerator)) * 200.0;
}

int main (int argc, char **argv)
{
	SDL_Event event;
	const Uint8 *keyboard_state_array;
	SDL_Thread *physics_thread;

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

	init_game();

	physics_thread = SDL_CreateThread(physics_thread_handler, "PhysicsThread", NULL);
	
	if (physics_thread == NULL) {
		printf("SDL_CreateThread failed: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	keyboard_state_array = SDL_GetKeyboardState(NULL);
	
	while (alive) {
		double inc = 1.0;

		if (keyboard_state_array[SDL_SCANCODE_UP])
			player->vy -= inc;
		if (keyboard_state_array[SDL_SCANCODE_DOWN])
			player->vy += inc;
		if (keyboard_state_array[SDL_SCANCODE_LEFT])
			player->vx -= inc;
		if (keyboard_state_array[SDL_SCANCODE_RIGHT])
			player->vx += inc;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					alive = 0;
					break;

				case SDL_KEYDOWN: {
					switch (event.key.keysym.sym) {
						// case SDLK_LEFT:
						// 	player->vx -= inc;
						// 	break;
						// case SDLK_RIGHT:
						// 	player->vx += inc;
						// 	break;
						// case SDLK_UP:
						// 	player->vy -= inc;
						// 	break;
						// case SDLK_DOWN:
						// 	player->vy += inc;
						// 	break;
						case SDLK_SPACE: {
							object_t *bullet;
							bullet = new object_t(OBJ_BULLET, player->x, player->y, 0, 200, 0);
							bullet->size = OBJ_SIZE / 4.0;
							bullet->vx = 0;
							bullet->vy = player->vy - 100.0;
							add_obj(bullet);
							break;
						}
						case SDLK_a: {
							object_t *asteroid;
							asteroid = new object_t(OBJ_ASTEROID, get_random_x_pos(), get_random_y_pos(), 0, 000, 200);
							asteroid->size = OBJ_SIZE;
							asteroid->vx = get_random_speed();
							asteroid->vy = get_random_speed();
							add_obj(asteroid);
							break;
						}
					}
					break;
				}
			}
		}

		render();
	}

	SDL_Quit();
	
	return 0;
}