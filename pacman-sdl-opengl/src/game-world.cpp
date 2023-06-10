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

	shader_t *vs = new shader_t(GL_VERTEX_SHADER, "shaders/triangles.vert");
	vs->compile();

	shader_t *fs = new shader_t(GL_FRAGMENT_SHADER, "shaders/triangles.frag");
	fs->compile();

	this->opengl_program = new opengl_program_t(vs, fs);
}

game_world_t::~game_world_t ()
{

}

int main (int argc, char **argv)
{
	game_world = new game_world_t;

	game_world->run();

	return 0;
}