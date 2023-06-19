#ifndef __PACMAN_SDL_OPENGL_GAMEWORLD_HEADER_H__
#define __PACMAN_SDL_OPENGL_GAMEWORLD_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include <stdint.h>

#include <vector>
#include <string>
#include <type_traits>

#include "lib.h"
#include "opengl.h"
#include "game-object.h"

// ---------------------------------------------------

class game_world_t;

// ---------------------------------------------------

class game_map_t
{
public:
	enum class cell_t {
		empty,
		wall,
		pacman_start
	};

protected:
	matrix_t<cell_t> *map;
	OO_ENCAPSULATE_READONLY(uint32_t, w)
	OO_ENCAPSULATE_READONLY(uint32_t, h)

public:
	game_map_t ();
	~game_map_t ();

	inline cell_t operator() (int row, int col)
	{
		return this->map->get(row, col);
	}
};

// ---------------------------------------------------

class game_main_t
{
public:
	enum class game_state_t {
		initializing,
		playing
	};

protected:
	OO_ENCAPSULATE(SDL_Window*, sdl_window)
	OO_ENCAPSULATE(SDL_GLContext, sdl_gl_context)
	OO_ENCAPSULATE(uint32_t, screen_width_px)
	OO_ENCAPSULATE(uint32_t, screen_height_px)
	OO_ENCAPSULATE(game_world_t*, game_world)
	OO_ENCAPSULATE(bool, alive)
	OO_ENCAPSULATE_READONLY(game_state_t, state)
	OO_ENCAPSULATE_READONLY(opengl_circle_factory_t*, opengl_circle_factory_low_def)
	OO_ENCAPSULATE_READONLY(opengl_circle_factory_t*, opengl_circle_factory_high_def)
	OO_ENCAPSULATE_READONLY(opengl_program_triangle_t*, opengl_program_triangle)

public:
	game_main_t ();
	~game_main_t ();
	void load ();
	void load_opengl_programs ();
	void run ();
	void cleanup ();
};

// ---------------------------------------------------

class game_world_t
{
protected:
	projection_matrix_t projection_matrix;

	// width and height of screen
	// the screen coordinates here are in game world coords (not opengl, neither pixels)
	// every unit corresponds to a tile
	OO_ENCAPSULATE_READONLY(float, w)
	OO_ENCAPSULATE_READONLY(float, h)
	//OO_ENCAPSULATE(float, world_to_opengl_conversion)

	OO_ENCAPSULATE_REFERENCE_READONLY(game_player_t*, player)

protected:
	std::vector< game_object_t* > objects;
	game_map_t& map;

public:
	game_world_t (game_map_t *map_);
	~game_world_t ();

	void bind_vertex_buffer ();

	inline void add_object (game_object_t *obj)
	{
		obj->set_game_world(this);
		this->objects.push_back(obj);
	}

	void event_keydown (SDL_Keycode key);
	void physics (float dt, const Uint8 *keys);
	void render (float dt);
};

// ---------------------------------------------------

extern game_main_t *game_main;

#endif