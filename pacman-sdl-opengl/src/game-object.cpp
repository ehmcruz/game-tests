#include "game-world.h"
#include "game-object.h"

uint32_t shape_circle_t::get_n_vertices ()
{
	return this->factory->get_n_vertices();
}

game_player_t::game_player_t ()
{
	this->shape = new shape_circle_t( CONFIG_PACMAN_RADIUS, game_main->get_opengl_circle_factory_low_def() );
	this->update_x_pos(5);
	this->update_y_pos(5);

	dprint( "player created" << std::endl )
}

game_player_t::~game_player_t ()
{
	delete this->shape;
}