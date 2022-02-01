#include "minimax.hpp"

#include "move.hpp"
#include "game_state.hpp"
#include "vertex_coloring.hpp"

#include <iomanip>
#include <vector>

std::pair<move, int> minimax(game_state& node, bool max_player, int alpha, int beta, int level) {
	if (node.col_.is_colored() && !node.col_.has_conflict()) {
		return { move(), 1 + level }; // max_player wins
	}

	if (node.col_.is_deadend() || node.col_.has_conflict()) {
		return { move(), -1 - level }; // min_player wins
	}

	const int value = max_player ? std::numeric_limits<int>::min() : std::numeric_limits<int>::max();
	std::pair<move, int> best_move(move(), value);

	// For each child node
	index_t uncols = node.uncols_;
#if defined(_MSC_VER)
	for (unsigned long v; uncols != 0; uncols &= ~(1ULL << v)) {
		_BitScanForward64(&v, uncols);
#elif defined(__GNUC__)
	for (uncols != 0) {
		const auto v = __builtin_ctzll(uncols);
		const auto lb = uncols & -uncols;
		uncols ^= lb;
#endif
		index_t col = node.col_.get_allowed_colors(v);

#if defined(_MSC_VER)
		for (unsigned long j; col != 0; col &= ~(1ULL << j))
		{
			// Now, (v, j) is a child node to consider
			_BitScanForward64(&j, col);
#elif defined(__GNUC__)
		while(col != 0) {
			const auto j = __builtin_ctzll(col);
			const auto lb_z = col & -col;
			col ^= lb_z;
#endif
			node.col_.color_vertex(v, j);
			node.remove(v);

			const std::pair<move, int> eval_score = minimax(node, !max_player, alpha, beta, level + 1);

			node.col_.uncolor_vertex(v, j);
			node.add(v);

			if (max_player) {
				if (eval_score.second > best_move.second) {
					best_move = std::make_pair(move(v, j), eval_score.second);
				}
				if (eval_score.second >= beta) {
					break;
				}
				alpha = std::max(alpha, eval_score.second);
			}
			else {
				if (eval_score.second < best_move.second) {
					best_move = std::make_pair(move(v, j), eval_score.second);
				}
				if (eval_score.second <= alpha) {
					break;
				}
				beta = std::min(beta, eval_score.second);
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
	//TranspositionTable t;

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

void print_gameplay(std::pair<Victory, std::queue<move>>& game) {
	const std::string players[2] = { "Alice", "Bob" };
	bool max_player = false;

	for (int round = 0; !game.second.empty(); ++round) {
		auto step = game.second.front();
		std::cout << "R" << round << " " << std::setw(5)
			<< players[static_cast<int>(max_player)]
			<< ", v = " << step.vertex_
			<< ", c = " << step.color_ << "\n";

		game.second.pop();
		max_player = !max_player;
	}

	if (game.first == Victory::Alice)
		std::cout << "Alice WINS!\n";
	else
		std::cout << "Bob WINS!\n";
}