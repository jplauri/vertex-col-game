#include "game_state.hpp"

#include "vertex_coloring.hpp"

game_state::game_state(vertex_coloring& col)
	: col_(col), 
	uncols_(ALL_ONES >> (BIT_LEN - col.num_vertices())) { }


void game_state::remove(index_t u) {
	uncols_ &= ~(1ULL << u);
}

void game_state::add(index_t u) {
	uncols_ |= 1ULL << u;
}