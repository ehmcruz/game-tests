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
#include <SDL_image.h>

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

//#include <GL/glu.h>
//#include <GL/gl.h>

#include <iostream>
#include <chrono>

bool alive = true;

typedef float t_mat4x4[16];

static void init_sdl_image ()
{
	int imgFlags = IMG_INIT_PNG;

	if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
		printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
		exit(1);
	}
}

SDL_Surface* loadSurface (char *fname)
{
	//The final optimized image
	//SDL_Surface *optimizedSurface = nullptr;

	//Load image at specified path
	SDL_Surface *loadedSurface = IMG_Load(fname);
	assert(loadedSurface != nullptr);

	if (loadedSurface == nullptr) {
		printf( "Unable to load image %s! SDL_image Error: %s\n", fname, IMG_GetError() );
		exit(1);
	}

	printf("loaded %s w=%i h=%i\n", fname, loadedSurface->w, loadedSurface->h);

	Uint32 rmask = 0x000000ff;
	Uint32 gmask = 0x0000ff00;
	Uint32 bmask = 0x00ff0000;
	Uint32 amask = 0xff000000;

	SDL_Surface *treatedSurface = SDL_CreateRGBSurface(0, loadedSurface->w, loadedSurface->h, 32, rmask, gmask, bmask, amask);

	assert(treatedSurface != nullptr);

	SDL_BlitSurface(loadedSurface, 0, treatedSurface, 0); // Blit onto a purely RGB Surface

	SDL_FreeSurface(loadedSurface);

	return treatedSurface;
	
	// Convert surface to screen format
	//optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, 0 );
	
/*
	if(optimizedSurface == nullptr) {
		printf( "Unable to optimize image %s! SDL Error: %s\n", fname, SDL_GetError() );
		exit(1);
	}

	// Get rid of old loaded surface
	SDL_FreeSurface( loadedSurface );

	return optimizedSurface;*/
}

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
	"#version 330 core\n"

	"in vec2 i_position;\n"
	"in vec2 i_tx_pos;\n"
	"in vec2 i_offset;\n"

	"out vec2 v_tx_pos;\n"
	
	"uniform mat4 u_projection_matrix;\n"
	
	"void main() {\n"
	"    v_tx_pos = i_tx_pos;\n"
	"    gl_Position = u_projection_matrix * vec4( (i_offset + i_position), 0.0, 1.0 );\n"
	"}\n";

static const char * fragment_shader =
	"#version 330 core\n"
	
	"in vec2 v_tx_pos;\n"
	
	"out vec4 o_color;\n"

	"uniform sampler2D u_tx_unit;\n"
	
	"void main() {\n"
	"    o_color = texture(u_tx_unit, v_tx_pos);\n"
	"}\n";

enum t_attrib_id {
	attrib_position,
	attrib_tx_pos,
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

	init_sdl_image();

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
	glBindAttribLocation( program, attrib_tx_pos, "i_tx_pos" );
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
	glEnableVertexAttribArray( attrib_tx_pos );
	glEnableVertexAttribArray( attrib_offset );

	struct gl_vertex_t {
		GLfloat tx_x; // texture x
		GLfloat tx_y; // texture y		
		GLfloat dummy; // just to have 8 elements in the struct
		GLfloat alpha;
		GLfloat x;
		GLfloat y;
		GLfloat offset_x;
		GLfloat offset_y;
	};

	glVertexAttribPointer( attrib_position, 2, GL_FLOAT, GL_FALSE, sizeof(gl_vertex_t), ( void * )(4 * sizeof(float)) );
	glVertexAttribPointer( attrib_tx_pos, 2, GL_FLOAT, GL_FALSE, sizeof(gl_vertex_t), 0 );
	glVertexAttribPointer( attrib_offset, 2, GL_FLOAT, GL_FALSE, sizeof(gl_vertex_t), ( void * )(6 * sizeof(float)) );

	// 2 triangles -> square
	gl_vertex_t g_vertex_buffer_data[6];

	// lets put the vertices order clockwise

	// first triangle
	g_vertex_buffer_data[0] = {0.0f, 0.0f, 0.0f, 1.0f, -50.0f, -50.0f, 200.0f, 200.0f};
	g_vertex_buffer_data[1] = {1.0f, 1.0f, 1.0f, 0.0f, 250.0f, 250.0f, 200.0f, 200.0f};
	g_vertex_buffer_data[2] = {0.0f, 1.0f, 0.0f, 0.0f, -50.0f, 250.0f, 200.0f, 200.0f};
	
	// second triangle
	g_vertex_buffer_data[3] = {0.0f, 0.0f, 0.0f, 1.0f, -50.0f, -50.0f, 200.0f, 200.0f};
	g_vertex_buffer_data[4] = {1.0f, 0.0f, 0.0f, 1.0f, 250.0f, -50.0f, 200.0f, 200.0f};
	g_vertex_buffer_data[5] = {1.0f, 1.0f, 1.0f, 0.0f, 250.0f, 250.0f, 200.0f, 200.0f};

	std::cout << "check c size " << sizeof(g_vertex_buffer_data) << std::endl;

	// ------------------------------------------

	// texture load

	GLuint texture_id;

	glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture

	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	SDL_Surface *figure = loadSurface((char*)"figure.png");
	//SDL_SaveBMP(figure, "figure-test.bmp");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, figure->w, figure->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, figure->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	SDL_FreeSurface(figure);

	GLint uniform_tx_unit_location = glGetUniformLocation(program, "u_tx_unit");
	glUniform1i(uniform_tx_unit_location, 0); // set shader to use texture unit 0

	// ------------------------------------------

	glBufferData( GL_ARRAY_BUFFER, sizeof( g_vertex_buffer_data ), g_vertex_buffer_data, GL_DYNAMIC_DRAW );

	float projection_matrix[4][4];
	mat4x4_ortho( (float*)projection_matrix, 0.0f, (float)width, (float)height, 0.0f, 0.0f, 0.1f );
	glUniformMatrix4fv( glGetUniformLocation( program, "u_projection_matrix" ), 1, GL_FALSE, (float*)projection_matrix );

	elapsed = 0.0f;
	float rotate_by = 0.0f;

	while (alive) {
		tbegin = std::chrono::steady_clock::now();

		float disp = 100.0f * elapsed;

		#define PI 3.1415f

		if (keyboard_state_array[SDL_SCANCODE_UP]) {
			for (int i=0; i<6; i++)
				g_vertex_buffer_data[i].offset_y -= disp;
		}
		if (keyboard_state_array[SDL_SCANCODE_DOWN]) {
			for (int i=0; i<6; i++)
				g_vertex_buffer_data[i].offset_y += disp;
		}
		if (keyboard_state_array[SDL_SCANCODE_LEFT]) {
			for (int i=0; i<6; i++)
				g_vertex_buffer_data[i].offset_x -= disp;
		}
		if (keyboard_state_array[SDL_SCANCODE_RIGHT]) {
			for (int i=0; i<6; i++)
				g_vertex_buffer_data[i].offset_x += disp;
		}
		if (keyboard_state_array[SDL_SCANCODE_R]) {
			rotate_by = PI / 100.0f;
			float s = sin(rotate_by);
			float c = cos(rotate_by);
			for (int i=0; i<6; i++) {
				g_vertex_buffer_data[i].x = g_vertex_buffer_data[i].x*c - g_vertex_buffer_data[i].y*s;
				g_vertex_buffer_data[i].y = g_vertex_buffer_data[i].x*s + g_vertex_buffer_data[i].y*c;
			}
		}
		if (keyboard_state_array[SDL_SCANCODE_T]) {
			rotate_by = PI / -100.0f;
			float s = sin(rotate_by);
			float c = cos(rotate_by);
			for (int i=0; i<6; i++) {
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

		//glClear( GL_COLOR_BUFFER_BIT );
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBufferData( GL_ARRAY_BUFFER, sizeof( g_vertex_buffer_data ), g_vertex_buffer_data, GL_DYNAMIC_DRAW );

		//glBindVertexArray( vao );
		glDrawArrays( GL_TRIANGLES, 0, 6 );

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
