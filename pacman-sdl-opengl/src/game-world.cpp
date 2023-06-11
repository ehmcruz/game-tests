#include <iostream>
#include <iomanip>

#include "game-world.h"

game_main_t *game_main = nullptr;

game_main_t::game_main_t ()
{
	ASSERT(game_main == nullptr)
}

game_main_t::~game_main_t ()
{
}

void game_main_t::load ()
{
	std::cout << std::setprecision(2);
	std::cout << std::fixed;

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

	this->screen_width_px = 600;
	this->screen_height_px = 600;

	this->sdl_window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->screen_width_px, this->screen_height_px, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	this->sdl_gl_context = SDL_GL_CreateContext(this->sdl_window);

	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
		exit(1);
	}

	std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;

	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glViewport(0, 0, this->screen_width_px, this->screen_height_px);

	this->opengl_circle_factory_low_def = new opengl_circle_factory_t(CONFIG_OPENGL_LOW_DEF_CIRCLES_TRIANGLES);
	//this->opengl_circle_factory_high_def = new opengl_circle_factory_t(CONFIG_OPENGL_HIGH_DEF_CIRCLES_TRIANGLES);

	dprint( "loaded opengl stuff" << std::endl )

	this->game_world = nullptr;
	this->game_world = new game_world_t;

	dprint( "loaded world" << std::endl )

	this->alive = true;
}

void game_main_t::run ()
{
	SDL_Event event;

	while (this->alive) {
		while ( SDL_PollEvent( &event ) ) {
			switch (event.type) {
				case SDL_QUIT:
					this->alive = false;
					break;
			}
		}

		glClear( GL_COLOR_BUFFER_BIT );

		//glBufferData( GL_ARRAY_BUFFER, sizeof(gl_vertex_t) * circle_factory.get_n_vertices(), g_vertex_buffer_data, GL_DYNAMIC_DRAW );

		//glBindVertexArray( vao );
		//glDrawArrays( GL_TRIANGLES, 0, circle_factory.get_n_vertices() );

		this->game_world->render();

		SDL_GL_SwapWindow(this->sdl_window);
	}
}

void game_main_t::cleanup ()
{
	SDL_GL_DeleteContext(this->sdl_gl_context);
	SDL_DestroyWindow(this->sdl_window);
	SDL_Quit();
}

game_world_t::game_world_t ()
{
	this->screen_width = 16.0f;
	this->screen_height = 16.0f;

	this->opengl_program_triangle = new opengl_program_triangle_t;

	dprint( "loaded opengl triangle program" << std::endl )

	this->opengl_program_triangle->use_program();
	
	this->opengl_program_triangle->bind_vertex_array();
	this->opengl_program_triangle->bind_vertex_buffer();

	this->opengl_program_triangle->setup_vertex_array();

	dprint( "generated and binded opengl world vertex array/buffer" << std::endl )

	this->load_matrices();

	this->player = new game_player_t;
	this->add_object(player);
}

game_world_t::~game_world_t ()
{
	delete this->player;
}

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

void game_world_t::load_matrices ()
{
	float projection_matrix[4][4];
	mat4x4_ortho( (float*)projection_matrix, 0.0f, this->screen_width, this->screen_height, 0.0f, 0.0f, 100.0f );
	glUniformMatrix4fv( glGetUniformLocation(opengl_program_triangle->get_program_id() , "u_projection_matrix" ), 1, GL_FALSE, (float*)projection_matrix );

	dprint( "projection matrix sent to GPU" << std::endl )
}

void game_world_t::render ()
{
	dprint( "start new frame render" << std::endl )

	this->opengl_program_triangle->clear();

	for (game_object_t *obj: this->objects) {
		obj->render();
	}

	//this->opengl_program_triangle->debug(); exit(1);

	this->opengl_program_triangle->upload_vertex_buffer();
	this->opengl_program_triangle->draw();
}

int main (int argc, char **argv)
{
	game_main = new game_main_t;
	
	game_main->load();
	game_main->run();
	game_main->cleanup();

	delete game_main;

	return 0;
}