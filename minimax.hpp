#ifndef MINIMAX_HPP
#define MINIMAX_HPP

#include <utility>
#include <queue>
#include <unordered_map>
#include <array>
#include "move.hpp"

#include "vertex_coloring.hpp"

struct game_state;
class graph;

enum class Victory {
	Alice = 0,
	Bob = 1
};

// See https://levelup.gitconnected.com/improving-minimax-performance-fc82bc337dfd
// Question: should we include more info than just vertex_coloring?

//typedef std::array<std::unordered_map<vertex_coloring, int>, 2> TranspositionTable;

std::pair<move, int> minimax(
	game_state& node, bool max_player, 
	int alpha = std::numeric_limits<int>::min(), 
	int beta = std::numeric_limits<int>::max(), 
	int level = 0);

std::pair<Victory, std::queue<move>> play_optimally(const graph& g, int num_cols);

void print_gameplay(std::pair<Victory, std::queue<move>>& game);

#endif