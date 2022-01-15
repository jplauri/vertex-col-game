#ifndef MINIMAX_HPP
#define MINIMAX_HPP

#include <utility>
#include <queue>
#include <unordered_map>
#include "move.hpp"

struct game_state;
class graph;

enum class Victory {
	Alice = 0,
	Bob = 1
};

std::pair<move, int> minimax(game_state& node, bool max_player, int level = 0);

std::pair<Victory, std::queue<move>> play_optimally(const graph& g, int num_cols);

#endif