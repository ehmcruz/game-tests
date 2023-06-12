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

	dprint( "loaded shader (" << this->fname << ")" << std::endl )
	//dprint( buffer )
	
	const char *c_str = buffer.c_str();
	glShaderSource(this->shader_id, 1, ( const GLchar ** )&c_str, nullptr);
	glCompileShader(this->shader_id);

	GLint status;
	glGetShaderiv(this->shader_id, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		std::cout << this->fname << " shader compilation failed" << std::endl;

		GLint logSize = 0;
		glGetShaderiv(this->shader_id, GL_INFO_LOG_LENGTH, &logSize);

		char *berror = (char*)malloc(logSize);
		ASSERT(berror != nullptr);

		glGetShaderInfoLog(this->shader_id, logSize, nullptr, berror);

		printf("%s\n", berror);
		free(berror);

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
	static_assert(sizeof(gl_vertex_t) == 32);
	static_assert((sizeof(gl_vertex_t) / sizeof(GLfloat)) == 8);

	this->vs = new shader_t(GL_VERTEX_SHADER, "shaders/triangles.vert");
	this->vs->compile();

	this->fs = new shader_t(GL_FRAGMENT_SHADER, "shaders/triangles.frag");
	this->fs->compile();

	this->attach_shaders();

	glBindAttribLocation(this->program_id, attrib_position, "i_position");
	glBindAttribLocation(this->program_id, attrib_offset, "i_offset");
	glBindAttribLocation(this->program_id, attrib_color, "i_color");

	this->link_program();

	glGenVertexArrays(1, &(this->vao));
	glGenBuffers(1, &(this->vbo));
}

void opengl_program_triangle_t::bind_vertex_array ()
{
	glBindVertexArray(this->vao);
}

void opengl_program_triangle_t::bind_vertex_buffer ()
{
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
}

void opengl_program_triangle_t::setup_vertex_array ()
{
	uint32_t pos, length;

	glEnableVertexAttribArray( attrib_position );
	glEnableVertexAttribArray( attrib_offset );
	glEnableVertexAttribArray( attrib_color );

	pos = 0;
	length = 2;
	glVertexAttribPointer( attrib_position, length, GL_FLOAT, GL_FALSE, sizeof(gl_vertex_t), ( void * )(pos * sizeof(float)) );
	
	pos += length;
	length = 2;
	glVertexAttribPointer( attrib_offset, length, GL_FLOAT, GL_FALSE, sizeof(gl_vertex_t), ( void * )(pos * sizeof(float)) );
	
	pos += length;
	length = 4;
	glVertexAttribPointer( attrib_color, length, GL_FLOAT, GL_FALSE, sizeof(gl_vertex_t), ( void * )(pos * sizeof(float)) );
}

void opengl_program_triangle_t::upload_vertex_buffer ()
{
	uint32_t n = this->triangle_buffer.get_vertex_buffer_used();
	glBufferData(GL_ARRAY_BUFFER, sizeof(gl_vertex_t) * n, this->triangle_buffer.get_vertex_buffer(), GL_DYNAMIC_DRAW);
}

void opengl_program_triangle_t::upload_projection_matrix (projection_matrix_t& m)
{
	glUniformMatrix4fv( glGetUniformLocation(this->program_id, "u_projection_matrix"), 1, GL_FALSE, m.get_raw() );
	dprint( "projection matrix sent to GPU" << std::endl )
}

void opengl_program_triangle_t::draw ()
{
	uint32_t n = this->triangle_buffer.get_vertex_buffer_used();
	glDrawArrays(GL_TRIANGLES, 0, n);
}

void opengl_program_triangle_t::debug ()
{
	uint32_t n = this->triangle_buffer.get_vertex_buffer_used();

	for (uint32_t i=0; i<n; i++) {
		gl_vertex_t *v = this->triangle_buffer.get_vertex(i);

		if ((i % 3) == 0)
			std::cout << std::endl;

		std::cout << "vertex[" << i
			<< "] x=" << v->x
			<< " y= " << v->y
			<< " offset_x= " << v->offset_x
			<< " offset_y= " << v->offset_y
			<< " r= " << v->r
			<< " g= " << v->g
			<< " b= " << v->b
			<< " a= " << v->a
			<< std::endl;
	}
}

opengl_circle_factory_t::opengl_circle_factory_t (uint32_t n_triangles)
{
	double angle, delta;
	
	this->n_triangles = n_triangles;
	
	this->table_cos = new float[n_triangles];
	this->table_sin = new float[n_triangles];

	/*
		cos(angle) = x / radius
		sin(angle) = y / radius
		
		x = cos(angle) * radius
		y = sin(angle) * radius

		2*pi radians is equal to 360 degrees
	*/

	delta = (2.0 * MY_PI) / static_cast<double>(this->n_triangles);
	angle = delta;

/*
	dprint( std::endl )
	dprint( "delta = " << delta << std::endl )
	dprint( std::endl )
*/

	for (uint32_t i=0; i<this->n_triangles; i++) {
		this->table_cos[i] = static_cast<float>( cos(angle) );
		this->table_sin[i] = static_cast<float>( sin(angle) );

	/*
		dprint( "cos(" << angle << ") = " << this->table_cos[i] << std::endl )
		dprint( "sin(" << angle << ") = " << this->table_sin[i] << std::endl )
		dprint( std::endl )
	*/

		angle += delta;
	}
}

opengl_circle_factory_t::~opengl_circle_factory_t ()
{
	delete[] this->table_cos;
	delete[] this->table_sin;
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

void projection_matrix_t::setup (float left, float right, 
                                 float bottom, float top,
								 float znear, float zfar
								 )
{
	projection_matrix_t& m = *this;

	m(0,0) = 2.0f / (right - left);
	m(0,1) = 0.0f;
	m(0,2) = 0.0f;
	m(0,3) = 0.0f;

	m(1,0) = 0.0f;
	m(1,1) = 2.0f / (top - bottom);
	m(1,2) = 0.0f;
	m(1,3) = 0.0f;

	m(2,0) = 0.0f;
	m(2,1) = 0.0f;
	m(2,2) = -2.0f / (zfar - znear);
	m(2,3) = 0.0f;

	m(3,0) = -(right + left) / (right - left);
	m(3,1) = -(top + bottom) / (top - bottom);
	m(3,2) = -(zfar + znear) / (zfar - znear);
	m(3,3) = 1.0f;
}
