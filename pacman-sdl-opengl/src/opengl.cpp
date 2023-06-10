#include <iostream>
#include <fstream>
#include <sstream>

#include <stdlib.h>
#include <math.h>

#include "opengl.h"

#define MY_PI M_PI

shader_t::shader_t (GLenum shader_type, const char *fname)
{
	this->shader_type = shader_type;
	this->fname = fname;
	this->shader_id = glCreateShader(this->shader_type);
}

void shader_t::compile ()
{
	// First, read the whole shader file to memory.
	// I usually do this in C, but wanted to do in C++ for a change, but Jesus...

	std::ifstream t(this->fname);
	std::stringstream str_stream;
	str_stream << t.rdbuf();
	std::string buffer = str_stream.str();

	dprint( "loaded shader (" << this->fname << "):" << std::endl )
	dprint( buffer )
	
	const char *c_str = buffer.c_str();
	glShaderSource(this->shader_id, 1, ( const GLchar ** )&c_str, nullptr);
	glCompileShader(this->shader_id);

	GLint status;
	glGetShaderiv(this->shader_id, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		std::cout << "vertex shader compilation failed" << std::endl;
		exit(1);
	}
}

opengl_program_t::opengl_program_t ()
{
	this->vs = nullptr;
	this->fs = nullptr;
	this->program_id = glCreateProgram();
}

void opengl_program_t::attach_shaders ()
{
	glAttachShader(this->program_id, this->vs->shader_id);
	glAttachShader(this->program_id, this->fs->shader_id);
}

void opengl_program_t::link_program ()
{
	glLinkProgram(this->program_id);
}

void opengl_program_t::use_program ()
{
	glUseProgram(this->program_id);
}

opengl_program_triangle_t::opengl_program_triangle_t ()
	: opengl_program_t ()
{
	this->vs = new shader_t(GL_VERTEX_SHADER, "shaders/triangles.vert");
	this->vs->compile();

	this->fs = new shader_t(GL_FRAGMENT_SHADER, "shaders/triangles.frag");
	this->fs->compile();

	this->attach_shaders();

	glBindAttribLocation(this->program_id, attrib_position, "i_position" );
	glBindAttribLocation(this->program_id, attrib_offset, "i_offset" );
	glBindAttribLocation(this->program_id, attrib_color, "i_color" );

	this->link_program();

	glEnableVertexAttribArray( attrib_position );
	glEnableVertexAttribArray( attrib_offset );
	glEnableVertexAttribArray( attrib_color );
}

opengl_circle_factory_t::opengl_circle_factory_t (uint32_t n_triangles)
{
	double angle, delta;
	
	this->n_triangles = n_triangles;

	/*
		cos(angle) = x / radius
		sin(angle) = y / radius
		
		x = cos(angle) * radius
		y = sin(angle) * radius

		2*pi radians is equal to 360 degrees
	*/

	delta = (2.0 * MY_PI) / static_cast<double>(this->n_triangles);
	angle = delta;

	for (uint32_t i=0; i<this->n_triangles; i++) {
		this->table_cos[i] = static_cast<float>( cos(angle) );
		this->table_sin[i] = static_cast<float>( sin(angle) );
		
		angle += delta;
	}
}

void opengl_circle_factory_t::fill_vertex_buffer (float radius, float *x, float *y, uint32_t stride)
{
	uint32_t j;
	float previous_x, previous_y;

	/*
		For each triangle:
			- first vertex is the center (0.0f, 0.0f)
			- second vertex is the previous calculated vertex (from previous triangle)
			- third vertex is the new vertex
	*/

	// for the first triangle
	previous_x = radius;
	previous_y = 0.0f;

	j = 0;
	for (uint32_t i=0; i<this->n_triangles; i++) {
		// first vertex
		x[j] = 0.0f;
		y[j] = 0.0f;

		j += stride;

		// second vertex
		x[j] = previous_x;
		y[j] = previous_y;

		j += stride;

		// third vertex
		x[j] = this->table_cos[i] * radius;
		y[j] = this->table_sin[i] * radius;

		previous_x = x[j];
		previous_y = y[j];

		j += stride;
	}
}