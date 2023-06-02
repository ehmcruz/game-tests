#ifndef __sogal_header_engine_h__
#define __sogal_header_engine_h__

// SOGAL - Simple OpenGL Game Library

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <GL/glew.h>

#define GL_GLEXT_PROTOTYPES

#include <SDL.h>
#include <SDL_opengl.h>

#include "lib.h"

namespace sogal
{
	class vector_t
	{
		OO_ENCAPSULATE(float, x)
		OO_ENCAPSULATE(float, y)

	public:
		inline static float inner_product (vector_t& v1, vector_t& v2)
		{
			return v1.x*v2.x + v1.y*v2.y;
		}

		inline float module ()
		{
			return fast_sqrt( vector_t::prod_int(*this, *this) );
		}
	};

	class point_t
	{
		OO_ENCAPSULATE(float, x)
		OO_ENCAPSULATE(float, y)

	public:
	};

	struct color_t {
		float r;
		float g;
		float b;
		float alpha;
	};

	struct gl_vertex_t {
		GLfloat r;
		GLfloat g;
		GLfloat b;
		GLfloat alpha;
		GLfloat x;
		GLfloat y;
	};

	class shape_triangle_t
	{
	public:
		point_t orig_points[3];
		point_t actual_points[3];
		color_t colors[3];

		void copy_to_opengl_vertex_buffer ();
	};

	class obj_t
	{
	private:
		point_t pos;
		vector_t velocity;

	public:
		virtual void render () = 0;
	};

	class obj_triangle_t: public obj_t
	{
	private:
		shape_triangle_t shape;

	public:
		void render () override;
	};
}

#endif