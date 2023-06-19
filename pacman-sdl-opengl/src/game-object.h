#ifndef __PACMAN_SDL_OPENGL_GAMEOBJECT_HEADER_H__
#define __PACMAN_SDL_OPENGL_GAMEOBJECT_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include <stdint.h>

#include "config.h"
#include "lib.h"
#include "opengl.h"

// ---------------------------------------------------

class game_world_t;
class game_object_t;

enum class shape_type_t {
	circle
};

// ---------------------------------------------------

class shape_t
{
protected:
	OO_ENCAPSULATE_READONLY(shape_type_t, shape_type)
	OO_ENCAPSULATE(game_object_t*, object)

	// distance from the center of the shape to the center of the object
	OO_ENCAPSULATE(float, dx)
	OO_ENCAPSULATE(float, dy)

public:
	inline shape_t (shape_type_t shape_type, game_object_t *object)
	{
		this->shape_type = shape_type;
		this->object = object;
		this->dx = 0.0f;
		this->dy = 0.0f;
	}

	inline shape_t (shape_type_t shape_type)
		: shape_t (shape_type, nullptr)
	{
	}

	virtual uint32_t get_n_vertices () = 0;
	virtual void push_vertices (float *x, float *y, uint32_t stride) = 0;
};

// ---------------------------------------------------

class shape_circle_t: public shape_t
{
protected:
	opengl_circle_factory_t *factory;

	OO_ENCAPSULATE(float, radius)

public:
	inline shape_circle_t (game_object_t *object, float radius, opengl_circle_factory_t *factory)
		: shape_t (shape_type_t::circle, object)
	{
		this->factory = factory;
		this->radius = radius;

		dprint( "circle created r=" << this->radius << std::endl )
	}

	inline shape_circle_t (float radius, opengl_circle_factory_t *factory)
		: shape_circle_t (nullptr, radius, factory)
	{
	}

	uint32_t get_n_vertices () override;
	void push_vertices (float *x, float *y, uint32_t stride) override;
};

// ---------------------------------------------------

class game_object_t
{
protected:
	OO_ENCAPSULATE(float, x)
	OO_ENCAPSULATE(float, y)
	OO_ENCAPSULATE(float, vx)
	OO_ENCAPSULATE(float, vy)
	OO_ENCAPSULATE(game_world_t*, game_world)

public:
	void physics (float dt);
	virtual void render (float dt) = 0;
};

// ---------------------------------------------------

class game_player_t: public game_object_t
{
protected:
	shape_circle_t *shape;

public:
	game_player_t ();
	~game_player_t ();

	void render (float dt) override;
};

#endif