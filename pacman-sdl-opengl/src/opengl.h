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
protected:
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
protected:
	OO_ENCAPSULATE(GLuint, program_id)
	OO_ENCAPSULATE(shader_t*, vs)
	OO_ENCAPSULATE(shader_t*, fs)

public:
	opengl_program_t ();
	void attach_shaders ();
	void link_program ();
	void use_program ();
};

class opengl_program_triangle_t: public opengl_program_t
{
protected:
	enum gl_attrib_t {
		attrib_position,
		attrib_offset,
		attrib_color
	} t_attrib_id;

	struct gl_vertex_t {
		GLfloat x; // local x,y coords
		GLfloat y;
		GLfloat offset_x; // global x,y coords, which are added to the local coords
		GLfloat offset_y;
		GLfloat r;
		GLfloat g;
		GLfloat b;
		GLfloat a; // alpha
	};

public:
	opengl_program_triangle_t ();
};

// ---------------------------------------------------

class opengl_circle_factory_t
{
private:
	float *table_sin;
	float *table_cos;
	uint32_t n_triangles;

public:
	opengl_circle_factory_t (uint32_t n_triangles);
	~opengl_circle_factory_t ();

	inline uint32_t get_n_vertices ()
	{
		return (3 * this->n_triangles);
	}

	void fill_vertex_buffer (float radius, float *x, float *y, uint32_t stride);
};

#endif