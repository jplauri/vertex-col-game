#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#include "common.hpp"

#include <limits>

class vertex_coloring;

struct game_state {
	game_state() = delete;
	game_state(vertex_coloring& col);

	void remove(index_t u);

	void add(index_t u);

	vertex_coloring& col_;
	index_t uncols_;
};

#endif