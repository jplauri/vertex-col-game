#include "minimax.hpp"

#include "move.hpp"
#include "game_state.hpp"
#include "vertex_coloring.hpp"

#include <vector>

std::pair<move, int> minimax(game_state& node, bool max_player, int level) {
	if (node.col_.is_colored() && !node.col_.has_conflict()) {
		return std::make_pair(move(), 1); // max_player wins
	}

	if (node.col_.is_deadend() || node.col_.has_conflict()) {
		return std::make_pair(move(), -1); // min_player wins
	}

	const int value = max_player ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
	std::pair<move, int> best_move(move(), value);

	// For each child node
	index_t uncols = node.uncols_;
	for (unsigned long v; uncols != 0; uncols &= ~(1ULL << v)) {
		_BitScanForward64(&v, uncols);
		index_t col = node.col_.get_allowed_colors(v);

		for (unsigned long j; col != 0; col &= ~(1ULL << j))
		{
			// Now, (v, j) is a child node to consider
			_BitScanForward64(&j, col);

			node.col_.color_vertex(v, j);
			node.remove(v);

			std::pair<move, int> eval_score = minimax(node, !max_player, level + 1);

			node.col_.uncolor_vertex(v, j);
			node.add(v);

			if (max_player) {
				if (eval_score.second > best_move.second) {
					best_move = std::make_pair(move(v, j), eval_score.second);
					break;
				}
			}
			else {
				if (eval_score.second < best_move.second) {
					best_move = std::make_pair(move(v, j), eval_score.second);
					break;
				}
			}
		}
	}

	return best_move;
}

std::pair<Victory, std::queue<move>> play_optimally(const graph& g, int num_cols) {
	vertex_coloring col(g, num_cols);
	bool max_player = true;
	game_state master(col);

	std::queue<move> moves;

	for (int i = 0; i < g.num_vertices(); ++i) {
		const auto best_move = minimax(master, max_player);

		auto [vertex, color] = best_move.first;

		moves.emplace(move(vertex, color));
		max_player = !max_player;

		master.remove(best_move.first.vertex_);
		master.col_.color_vertex(best_move.first.vertex_, best_move.first.color_);

		if (master.col_.has_conflict() || master.col_.is_deadend()) {
			break;
		}
	}

	if (master.col_.is_colored() && !master.col_.has_conflict())
		return std::make_pair(Victory::Alice, moves);
	else
		return std::make_pair(Victory::Bob, moves);
}