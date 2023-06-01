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

#include <box2d/box2d.h>

using namespace std;

#define SCREEN_W 800
#define SCREEN_H 600
#define MAX_OBJS 1000

#define BALL_W     15.0
#define BALL_H     BALL_W
#define RACKET_W   70.0
#define RACKET_H   30.0

#define FIRST_STRIKE_SPEED_Y    400.0f

#define BALL_ENERGY_DROP        0.1f

#define MAX_BALL_SPEED_Y        500.0

enum obj_type_t {
	OBJ_BALL,
	OBJ_RACKET
};

class obj_t
{
public:
	b2Body *body;
	int w, h;
	Uint8 r, g, b;
	obj_type_t type;

	b2Fixture* get_fixture () {
		b2Fixture *f = this->body->GetFixtureList();
		assert(f != nullptr);
		return f;
	}
};

SDL_Window *screen;
SDL_Renderer *renderer;

obj_t *objs[MAX_OBJS];
int nobjs = 0;

obj_t *player;
obj_t *forrest;
obj_t *ball;

int alive = 1;

class MyContactListener : public b2ContactListener
{
	void BeginContact(b2Contact *contact) {
		obj_t *obj_a = (obj_t*)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
		obj_t *obj_b = (obj_t*)contact->GetFixtureB()->GetBody()->GetUserData().pointer;
		obj_t *obj;
		obj_t *racket;

		if (obj_a == ball)
			racket = obj_b;
		else if (obj_b == ball)
			racket = obj_a;
		else
			return;

		printf("xx\n");

		ball->r = 255;
		ball->g = 255;
		ball->b = 0;

		b2Vec2 ballv = ball->body->GetLinearVelocity();

		if (racket == forrest && ballv.y < 0.0f) {
			b2Vec2 new_ballv = ballv;
			new_ballv.y = ballv.y * -1.0f;
			ball->body->SetLinearVelocity(new_ballv);
			printf("yy\n");
		}
		
		if (racket == player && ballv.y > 0.0f) {
			b2Vec2 new_ballv = ballv;
			new_ballv.y = ballv.y * -1.0f;
			ball->body->SetLinearVelocity(new_ballv);
			printf("zz\n");
		}
	}

	void EndContact(b2Contact *contact) {
		obj_t *obj_a = (obj_t*)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
		obj_t *obj_b = (obj_t*)contact->GetFixtureB()->GetBody()->GetUserData().pointer;

		if (!(obj_a == ball || obj_b == ball))
			return;

		ball->r = 0;
		ball->g = 0;
		ball->b = 255;
	}
};

b2World *world;
MyContactListener myContactListenerInstance;

static void render ()
{
	int i;
	obj_t *o;
	SDL_Rect rect;

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	for (i=0; i<nobjs; i++) {
		o = objs[i];

		b2Fixture *f = o->get_fixture();
		b2Shape *shape = f->GetShape();
		b2PolygonShape *s = static_cast<b2PolygonShape*>(shape);
		
		rect.x = o->body->GetPosition().x - o->w/2;
		rect.y = o->body->GetPosition().y - o->h/2;
		rect.w = o->w;
		rect.h = o->h;

		SDL_SetRenderDrawColor(renderer, o->r, o->g, o->b, 255);
		SDL_RenderFillRect(renderer, &rect);
	}

	SDL_RenderPresent(renderer);
}

static void run_forrest (double t)
{
	//forrest->pos.x = ball->pos.x;
}

static void add_obj (obj_t *o)
{
	assert(nobjs < MAX_OBJS);
	
	objs[ nobjs++ ] = o;
}

static void init_game ()
{
	b2Vec2 gravity(0.0f, 0.0f);

	world = new b2World(gravity);
	world->SetContactListener(&myContactListenerInstance);

	{
		ball = new obj_t();

		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(SCREEN_W / 2, SCREEN_H / 2);
		bodyDef.userData.pointer = (uintptr_t)ball;

		ball->body = world->CreateBody(&bodyDef);

		b2PolygonShape dynamicBox;
		dynamicBox.SetAsBox(BALL_W / 2, BALL_H / 2);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &dynamicBox;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.0f;

		ball->body->CreateFixture(&fixtureDef);

		ball->w = BALL_W;
		ball->h = BALL_W;
		ball->r = 0;
		ball->g = 0;
		ball->b = 255;
		ball->type = OBJ_BALL;
		
		add_obj(ball);
	}

	{	
		player = new obj_t();

		b2BodyDef bodyDef;
		bodyDef.type = b2_kinematicBody;
		bodyDef.position.Set(SCREEN_W / 2, SCREEN_H - RACKET_H / 2);
		bodyDef.userData.pointer = (uintptr_t)player;

		player->body = world->CreateBody(&bodyDef);

		b2PolygonShape dynamicBox;
		dynamicBox.SetAsBox(RACKET_W / 2, RACKET_H / 2);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &dynamicBox;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.0f;

		player->body->CreateFixture(&fixtureDef);

		player->w = RACKET_W;
		player->h = RACKET_H;

		player->r = 0;
		player->g = 255;
		player->b = 0;
		player->type = OBJ_RACKET;
		
		add_obj(player);
	}

	// ---------------------------------------

	{	
		forrest = new obj_t();

		b2BodyDef bodyDef;
		bodyDef.type = b2_kinematicBody;
		bodyDef.position.Set(SCREEN_W / 2, RACKET_H / 2);
		bodyDef.userData.pointer = (uintptr_t)forrest;

		forrest->body = world->CreateBody(&bodyDef);

		b2PolygonShape dynamicBox;
		dynamicBox.SetAsBox(RACKET_W / 2, RACKET_H / 2);

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &dynamicBox;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.0f;

		forrest->body->CreateFixture(&fixtureDef);

		forrest->w = RACKET_W;
		forrest->h = RACKET_H;

		forrest->r = 0;
		forrest->g = 255;
		forrest->b = 0;
		forrest->type = OBJ_RACKET;
		
		add_obj(forrest);
	}
}

int main (int argc, char **argv)
{
	SDL_Event event;
	const Uint8 *keyboard_state_array;
	chrono::high_resolution_clock::time_point tbegin, tend;
	float elapsed, target_fps, target_elapsed, fps;

	cout << chrono::high_resolution_clock::period::den << endl;

	SDL_Init(SDL_INIT_EVERYTHING);

	screen = SDL_CreateWindow("My Game Window",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_W, SCREEN_H,
		SDL_WINDOW_OPENGL);

	renderer = SDL_CreateRenderer(screen, -1, 0);

	target_fps = 60.0f;
	target_elapsed = 1.0f / target_fps;
		
	init_game();

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
	
	keyboard_state_array = SDL_GetKeyboardState(NULL);
	
	elapsed = 0.0f;
	
	while (alive) {
		tbegin = chrono::high_resolution_clock::now();
		
		#define inc 200.0f

		b2Vec2 speed_player(0.0f, 0.0f);
		
		if (keyboard_state_array[SDL_SCANCODE_UP])
			speed_player.y = -inc;
		else if (keyboard_state_array[SDL_SCANCODE_DOWN])
			speed_player.y = inc;

		if (keyboard_state_array[SDL_SCANCODE_LEFT])
			speed_player.x = -inc;
		if (keyboard_state_array[SDL_SCANCODE_RIGHT])
			speed_player.x = inc;

		player->body->SetLinearVelocity(speed_player);
			
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					alive = 0;
					break;

				case SDL_KEYDOWN: {
					switch (event.key.keysym.sym) {
						case SDLK_SPACE: {
							ball->body->SetTransform( b2Vec2(player->body->GetPosition().x, SCREEN_H-100), 0.0f );
							ball->body->SetLinearVelocity( b2Vec2(0.0f, -200.0f) );
							printf("ballv.y = %.4f\n", ball->body->GetLinearVelocity().y);
							//exit(1);
							cout << "espaço apertado" << endl;
							break;
						}
					}
					break;
				}
				
			}
		}

		fps = 1.0f / elapsed;

		printf("frame ellapsed = %.4fs (%.1f fps)\nballv.y = %.4f\n", elapsed, fps, ball->body->GetLinearVelocity().y);
		
		run_forrest(elapsed);
//elapsed = 0.01f;
//		physics(elapsed);
		int32_t velocityIterations = 6;
		int32_t positionIterations = 2;
		world->Step(target_elapsed, velocityIterations, positionIterations);
		render();

		//printf("new frame elapsed = %.4f\n", elapsed);

		do {
			tend = chrono::high_resolution_clock::now();
			chrono::duration<float> elapsed_ = chrono::duration_cast<chrono::duration<float>>(tend - tbegin);
			elapsed = elapsed_.count();
		} while (elapsed < target_elapsed);
	}

	SDL_Quit();
	
	return 0;
}
