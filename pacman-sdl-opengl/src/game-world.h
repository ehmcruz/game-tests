#ifndef __PACMAN_SDL_OPENGL_GAMEWORLD_HEADER_H__
#define __PACMAN_SDL_OPENGL_GAMEWORLD_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include <stdint.h>

#include "lib.h"
#include "opengl.h"

class game_main_t
{
private:
	OO_ENCAPSULATE(SDL_Window*, sdl_window)
	OO_ENCAPSULATE(SDL_GLContext, sdl_gl_context)
	OO_ENCAPSULATE(uint32_t, screen_width_px)
	OO_ENCAPSULATE(uint32_t, screen_height_px)
	OO_ENCAPSULATE(game_world_t*, game_world)

public:
	game_main_t ();
	~game_main_t ();
	void run ();
};

class game_world_t
{
private:
	opengl_program_t *ogl_program;

	// the screen coordinates here are in game world coords (not opengl, neither pixels)
	// every unit corresponds to a tile
	OO_ENCAPSULATE(float, screen_width)
	OO_ENCAPSULATE(float, screen_height)
	//OO_ENCAPSULATE(float, world_to_opengl_conversion)

	OO_ENCAPSULATE(opengl_program_t*, opengl_program)

public:
	game_world_t ();
	~game_world_t ();
};

extern game_main_t *game_main;

#endif