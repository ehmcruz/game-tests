#include <iostream>

#include "game-world.h"

game_main_t *game_main = nullptr;

game_main_t::game_main_t ()
{
	ASSERT(game_main == nullptr)

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
	this->opengl_circle_factory_high_def = new opengl_circle_factory_t(CONFIG_OPENGL_HIGH_DEF_CIRCLES_TRIANGLES);

	this->game_world = nullptr;
	this->game_world = new game_world_t;
}

game_main_t::~game_main_t ()
{

}

void game_main_t::run ()
{
	while (1);
}

game_world_t::game_world_t ()
{
	this->screen_width = 16.0f;
	this->screen_height = 16.0f;

	this->opengl_program_triangle = new opengl_program_triangle_t;
	this->opengl_program_triangle->use_program();

	glGenVertexArrays(1, &(this->vao));
	glGenBuffers(1, &(this->vbo));
	
	this->bind_vertex_array();
	this->bind_vertex_buffer();

	this->player = new game_player_t;
}

game_world_t::~game_world_t ()
{
	delete this->player;
}

void game_world_t::bind_vertex_array ()
{
	glBindVertexArray(this->vao);
}

void game_world_t::bind_vertex_buffer ()
{
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
}

int main (int argc, char **argv)
{
	game_main = new game_main_t;

	game_main->run();

	return 0;
}