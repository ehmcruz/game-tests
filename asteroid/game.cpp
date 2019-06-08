#include <SDL.h>
#include <assert.h>

#define MAX_OBJS 1000
#define SCREEN_W 800
#define SCREEN_H 600
#define OBJ_SIZE 20.0

SDL_Window *screen;
SDL_Renderer *renderer;

int nobjs = 0;

enum object_type_t {
	OBJ_PLAYER,
	OBJ_BULLET
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
}

int main (int argc, char **argv)
{
	int gogogo = 1;
	SDL_Event event;
	unsigned int tbegin, tend;

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
	
	while (gogogo) {
		tbegin = SDL_GetTicks();

		while (SDL_PollEvent(&event)) {
		//SDL_WaitEvent(&event);
			switch (event.type) {
				case SDL_QUIT:
					gogogo = 0;
					break;

				case SDL_KEYDOWN: {
					double inc = 5.0;

					switch (event.key.keysym.sym) {
						case SDLK_LEFT:
							player->vx -= inc;
							break;
						case SDLK_RIGHT:
							player->vx += inc;
							break;
						case SDLK_UP:
							player->vy -= inc;
							break;
						case SDLK_DOWN:
							player->vy += inc;
							break;
						case SDLK_SPACE: {
							object_t *bullet;
							bullet = new object_t(OBJ_BULLET, player->x, player->y, 0, 200, 0);
							bullet->size = OBJ_SIZE / 4.0;
							bullet->vx = 0;
							bullet->vy = player->vy - 100.0;
							add_obj(bullet);
							break;
						}
					}
					break;
				}
			}
		}

		render();

		do {
			tend = SDL_GetTicks();
		} while (tend <= tbegin);

		printf("elapsed: %u\n", tend-tbegin);
		physics((double)(tend-tbegin) / 1000.0);
	}

	SDL_Quit();
	
	return 0;
}