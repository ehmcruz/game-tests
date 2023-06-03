#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

/*
    Minimal SDL2 + OpenGL3 example.
    Author: https://github.com/koute
    This file is in the public domain; you can do whatever you want with it.
    In case the concept of public domain doesn't exist in your jurisdiction
    you can also use this code under the terms of Creative Commons CC0 license,
    either version 1.0 or (at your option) any later version; for details see:
        http://creativecommons.org/publicdomain/zero/1.0/
    This software is distributed without any warranty whatsoever.
    Compile and run with: gcc opengl3_hello.c `sdl2-config --libs --cflags` -lGL -Wall && ./a.out
*/

#include <GL/glew.h>

#define GL_GLEXT_PROTOTYPES

#include <SDL.h>
#include <SDL_opengl.h>
#include <math.h>
//#include <GL/glu.h>
//#include <GL/gl.h>

#include <iostream>
#include <chrono>

bool alive = true;

typedef float t_mat4x4[16];

static inline void mat4x4_ortho(float *out, float left, float right, float bottom, float top, float znear, float zfar )
{
	#define T(a, b) (a * 4 + b)

	out[T(0,0)] = 2.0f / (right - left);
	out[T(0,1)] = 0.0f;
	out[T(0,2)] = 0.0f;
	out[T(0,3)] = 0.0f;

	out[T(1,1)] = 2.0f / (top - bottom);
	out[T(1,0)] = 0.0f;
	out[T(1,2)] = 0.0f;
	out[T(1,3)] = 0.0f;

	out[T(2,2)] = -2.0f / (zfar - znear);
	out[T(2,0)] = 0.0f;
	out[T(2,1)] = 0.0f;
	out[T(2,3)] = 0.0f;

	out[T(3,0)] = -(right + left) / (right - left);
	out[T(3,1)] = -(top + bottom) / (top - bottom);
	out[T(3,2)] = -(zfar + znear) / (zfar - znear);
	out[T(3,3)] = 1.0f;

	#undef T
}

static const char * vertex_shader =
	"#version 130\n"
	"in vec2 i_position;\n"
	"in vec4 i_color;\n"
	"in vec2 i_offset;\n"
	"out vec4 v_color;\n"
	"uniform mat4 u_projection_matrix;\n"
	"void main() {\n"
	"    v_color = i_color;\n"
	"    gl_Position = u_projection_matrix * vec4( (i_offset + i_position), 0.0, 1.0 );\n"
	"}\n";

static const char * fragment_shader =
	"#version 130\n"
	"in vec4 v_color;\n"
	"out vec4 o_color;\n"
	"void main() {\n"
	"    o_color = v_color;\n"
	"}\n";

enum t_attrib_id {
	attrib_position,
	attrib_color,
	attrib_offset
} t_attrib_id;

int main( int argc, char **argv )
{
	//std::chrono::high_resolution_clock::time_point tbegin, tend;
	std::chrono::steady_clock::time_point tbegin, tend;
	float elapsed;
	const Uint8 *keyboard_state_array;

	std::cout << "chorno resolution " << ((double)std::chrono::high_resolution_clock::period::num / (double)std::chrono::high_resolution_clock::period::den) << std::endl;

	SDL_Init( SDL_INIT_EVERYTHING );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );

	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

	static const int width = 800;
	static const int height = 600;

	keyboard_state_array = SDL_GetKeyboardState(NULL);

	SDL_Window * window = SDL_CreateWindow( "", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

	SDL_GLContext context = SDL_GL_CreateContext( window );

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		printf("Error: %s\n", glewGetErrorString(err));
		exit(1);
	}

	std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;

	GLuint vs, fs, program;

	vs = glCreateShader( GL_VERTEX_SHADER );
	fs = glCreateShader( GL_FRAGMENT_SHADER );

	int length = strlen( vertex_shader );
	glShaderSource( vs, 1, ( const GLchar ** )&vertex_shader, &length );
	glCompileShader( vs );

	GLint status;
	glGetShaderiv( vs, GL_COMPILE_STATUS, &status );
	if( status == GL_FALSE ) {
		std::cout << "vertex shader compilation failed" << std::endl;
		return 1;
	}

	length = strlen( fragment_shader );
	glShaderSource( fs, 1, ( const GLchar ** )&fragment_shader, &length );
	glCompileShader( fs );

	glGetShaderiv( fs, GL_COMPILE_STATUS, &status );
	if( status == GL_FALSE ) {
		std::cout << "fragment shader compilation failed" << std::endl;
		return 1;
	}

	program = glCreateProgram();
	glAttachShader( program, vs );
	glAttachShader( program, fs );

	glBindAttribLocation( program, attrib_position, "i_position" );
	glBindAttribLocation( program, attrib_color, "i_color" );
	glBindAttribLocation( program, attrib_offset, "i_offset" );
	glLinkProgram( program );

	glUseProgram( program );

	glDisable( GL_DEPTH_TEST );
	glClearColor( 0.5, 0.0, 0.0, 0.0 );
	glViewport( 0, 0, width, height );

	GLuint vao, vbo;

	glGenVertexArrays( 1, &vao );
	glGenBuffers( 1, &vbo );
	glBindVertexArray( vao );
	glBindBuffer( GL_ARRAY_BUFFER, vbo );

	glEnableVertexAttribArray( attrib_position );
	glEnableVertexAttribArray( attrib_color );
	glEnableVertexAttribArray( attrib_offset );

	struct gl_vertex_t {
		GLfloat r;
		GLfloat g;
		GLfloat b;
		GLfloat alpha;
		GLfloat x;
		GLfloat y;
		GLfloat offset_x;
		GLfloat offset_y;
	};

	glVertexAttribPointer( attrib_position, 2, GL_FLOAT, GL_FALSE, sizeof(gl_vertex_t), ( void * )(4 * sizeof(float)) );
	glVertexAttribPointer( attrib_color, 4, GL_FLOAT, GL_FALSE, sizeof(gl_vertex_t), 0 );
	glVertexAttribPointer( attrib_offset, 2, GL_FLOAT, GL_FALSE, sizeof(gl_vertex_t), ( void * )(6 * sizeof(float)) );

	gl_vertex_t g_vertex_buffer_data[3];

	g_vertex_buffer_data[0] = {1.0f, 0.0f, 0.0f, 1.0f, -50.0f, -50.0f, 200.0f, 200.0f};
	g_vertex_buffer_data[1] = {0.0f, 1.0f, 0.0f, 0.0f, -50.0f, 50.0f, 200.0f, 200.0f};
	g_vertex_buffer_data[2] = {0.0f, 0.0f, 1.0f, 0.0f, 50.0f, 50.0f, 200.0f, 200.0f};

	std::cout << "check c size " << sizeof(g_vertex_buffer_data) << std::endl;

	glBufferData( GL_ARRAY_BUFFER, sizeof( g_vertex_buffer_data ), g_vertex_buffer_data, GL_DYNAMIC_DRAW );

	float projection_matrix[4][4];
	mat4x4_ortho( (float*)projection_matrix, 0.0f, (float)width, (float)height, 0.0f, 0.0f, 100.0f );
	glUniformMatrix4fv( glGetUniformLocation( program, "u_projection_matrix" ), 1, GL_FALSE, (float*)projection_matrix );

	elapsed = 0.0f;
	float rotate_by = 0.0f;

	while (alive) {
		tbegin = std::chrono::steady_clock::now();

		float disp = 100.0f * elapsed;

		#define PI 3.1415f

		if (keyboard_state_array[SDL_SCANCODE_UP]) {
			for (int i=0; i<3; i++)
				g_vertex_buffer_data[i].offset_y -= disp;
		}
		if (keyboard_state_array[SDL_SCANCODE_DOWN]) {
			for (int i=0; i<3; i++)
				g_vertex_buffer_data[i].offset_y += disp;
		}
		if (keyboard_state_array[SDL_SCANCODE_LEFT]) {
			for (int i=0; i<3; i++)
				g_vertex_buffer_data[i].offset_x -= disp;
		}
		if (keyboard_state_array[SDL_SCANCODE_RIGHT]) {
			for (int i=0; i<3; i++)
				g_vertex_buffer_data[i].offset_x += disp;
		}
		if (keyboard_state_array[SDL_SCANCODE_R]) {
			rotate_by = PI / 100.0f;
			float s = sin(rotate_by);
			float c = cos(rotate_by);
			for (int i=0; i<3; i++) {
				g_vertex_buffer_data[i].x = g_vertex_buffer_data[i].x*c - g_vertex_buffer_data[i].y*s;
				g_vertex_buffer_data[i].y = g_vertex_buffer_data[i].x*s + g_vertex_buffer_data[i].y*c;
			}
		}
		if (keyboard_state_array[SDL_SCANCODE_T]) {
			rotate_by = PI / -100.0f;
			float s = sin(rotate_by);
			float c = cos(rotate_by);
			for (int i=0; i<3; i++) {
				g_vertex_buffer_data[i].x = g_vertex_buffer_data[i].x*c - g_vertex_buffer_data[i].y*s;
				g_vertex_buffer_data[i].y = g_vertex_buffer_data[i].x*s + g_vertex_buffer_data[i].y*c;
			}
		}

		SDL_Event event;
		while( SDL_PollEvent( &event ) ) {
			switch( event.type ) {
				case SDL_QUIT:
					alive = false;
					break;
			}
		}

		glClear( GL_COLOR_BUFFER_BIT );

		glBufferData( GL_ARRAY_BUFFER, sizeof( g_vertex_buffer_data ), g_vertex_buffer_data, GL_DYNAMIC_DRAW );

		//glBindVertexArray( vao );
		glDrawArrays( GL_TRIANGLES, 0, 3 );

		SDL_GL_SwapWindow( window );

		do {
			tend = std::chrono::steady_clock::now();
			std::chrono::duration<float> elapsed_ = std::chrono::duration_cast<std::chrono::duration<float>>(tend - tbegin);
			elapsed = elapsed_.count();
		} while (elapsed < 0.001f);
	}

	SDL_GL_DeleteContext( context );
	SDL_DestroyWindow( window );
	SDL_Quit();

	return 0;
}
