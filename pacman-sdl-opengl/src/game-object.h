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

public:
	virtual uint32_t get_n_vertices () = 0;
	virtual void push_vertices (float *x, float *y, uint32_t stride) = 0;
};

// ---------------------------------------------------

class shape_circle_t: public shape_t
{
protected:
	opengl_circle_factory_t *factory;

	OO_ENCAPSULATE(float, radius)
	OO_ENCAPSULATE(game_object_t*, object)

public:
	inline shape_circle_t (float radius, opengl_circle_factory_t *factory)
	{
		this->shape_type = shape_type_t::circle;
		this->factory = factory;
		this->radius = radius;

		dprint( "circle created r=" << this->radius << std::endl )
	}

	uint32_t get_n_vertices () override;
	void push_vertices (float *x, float *y, uint32_t stride) override;
};

// ---------------------------------------------------

class game_object_t
{
protected:
	/*
		In this particular game, we need to be careful becausa pacman and the other
		objects need to move only over a grid.
		The easier (and bugless) solution I believe is to have 2 game coordinates.
		One coord is the traditional float position.
		The other coord is an integer coord.
		The movement will be first computed with the integer-coord, and then it will
		be translated to the float coord.
		In this way, we can guarantee that pacman will move only over the grid.
	*/
	OO_ENCAPSULATE_READONLY(float, xf)
	OO_ENCAPSULATE_READONLY(float, yf)
	OO_ENCAPSULATE_READONLY(int32_t, xi)
	OO_ENCAPSULATE_READONLY(int32_t, yi)
	
	OO_ENCAPSULATE(game_world_t*, game_world)

public:
	#define TILE_MASK (CONFIG_TILE_SIZE - 1)

	inline void update_x_pos (uint32_t xi)
	{
		this->xi = xi;
		this->xf = static_cast<float>(xi >> CONFIG_TILE_BITS)
		         + static_cast<float>(xi & TILE_MASK) / static_cast<float>(CONFIG_TILE_SIZE);
	}

	inline void update_y_pos (uint32_t yi)
	{
		this->yi = yi;		
		this->yf = static_cast<float>(yi >> CONFIG_TILE_BITS)
		         + static_cast<float>(yi & TILE_MASK) / static_cast<float>(CONFIG_TILE_SIZE);
	}

	virtual void render () = 0;
};

// ---------------------------------------------------

class game_player_t: public game_object_t
{
protected:
	shape_circle_t *shape;

public:
	game_player_t ();
	~game_player_t ();

	void render () override;
};

#endif