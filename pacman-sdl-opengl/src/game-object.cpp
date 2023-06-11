#include "game-world.h"
#include "game-object.h"

uint32_t shape_circle_t::get_n_vertices ()
{
	return this->factory->get_n_vertices();
}

void shape_circle_t::push_vertices (float *x, float *y, uint32_t stride)
{
	this->factory->fill_vertex_buffer(this->radius, x, y, stride);
}

game_player_t::game_player_t ()
{
	this->shape = new shape_circle_t( CONFIG_PACMAN_RADIUS, game_main->get_opengl_circle_factory_low_def() );
	this->update_x_pos(200);
	this->update_y_pos(200);

	dprint( "player created" << std::endl )
}

game_player_t::~game_player_t ()
{
	delete this->shape;
}

void game_player_t::render ()
{
	opengl_program_triangle_t::gl_vertex_t *vertices;
	opengl_program_triangle_t *opengl_program_triangle;
	uint32_t n_vertices;

	n_vertices = this->shape->get_n_vertices();
	dprint( "player allocating space for " << n_vertices << " vertices in vertex_buffer" << std::endl )

	opengl_program_triangle = this->game_world->get_opengl_program_triangle();
	vertices = opengl_program_triangle->alloc_vertices(n_vertices);

	this->shape->push_vertices( &(vertices->x), &(vertices->y), opengl_program_triangle->get_stride() );

	for (uint32_t i=0; i<n_vertices; i++) {
		vertices[i].offset_x = this->xf;
		vertices[i].offset_y = this->yf;
		vertices[i].r = 1.0f;
		vertices[i].g = 0.0f;
		vertices[i].b = 0.0f;
		vertices[i].a = 1.0f;
	}
}