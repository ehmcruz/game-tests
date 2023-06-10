#ifndef __PACMAN_SDL_OPENGL_OPENGL_HEADER_H__
#define __PACMAN_SDL_OPENGL_OPENGL_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <GL/glew.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include <stdint.h>

#include <string>

#include "lib.h"

class opengl_program_t;

class shader_t
{
private:
	OO_ENCAPSULATE(GLuint, shader_id)
	OO_ENCAPSULATE(GLenum, shader_type)
	OO_ENCAPSULATE_REFERENCE(std::string, fname)

public:
	shader_t (GLenum shader_type, const char *fname);
	void compile ();

	friend class opengl_program_t;
};

class opengl_program_t
{
private:
	OO_ENCAPSULATE(GLuint, program_id)
	OO_ENCAPSULATE(shader_t*, vs)
	OO_ENCAPSULATE(shader_t*, fs)

public:
	opengl_program_t (shader_t *vs, shader_t *fs);
};

#endif