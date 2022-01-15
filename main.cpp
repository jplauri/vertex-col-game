#include "graph.hpp"
#include "tests.hpp"
#include "vertex_coloring.hpp"
#include "minimax.hpp"
#include "common.hpp"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <queue>
#include <chrono>
#include <fstream>
#include <string>
#include <unordered_set>

void print_gameplay(std::pair<Victory, std::queue<move>>& game);

void verify_g6_batch(const std::string& file, bool verbose = true);

const std::unordered_map<std::string, std::pair<int, int>> allowed_types = {
	{"planar", {4, 11}},
	{"outerplanar", {4, 11}}
};

constexpr const int NO_K = -1;
const std::string NO_TYPE = "no_graph_type";

int find_k_from_args(const std::unordered_set<std::string>& args);

std::pair<std::string, std::pair<int, int>> find_type_from_args(const std::unordered_set<std::string>& args);

std::string get_graph6_file(const std::string& family, int k);

int main(int argc, char** argv) 
{
	if (argc < 2 || argc > 5) {
		std::cout << "Usage: ./vertex-col-game <k> <type> [<all>] [<tests>]\n"
			<< "<k>:       the order of the family\n"
			<< "<type>:    the type of the family (e.g., outerplanar)\n"
			<< "<all>:     ignore order, run for all n of the specific family\n"
			<< "<tests>:   whether to only run tests\n";
		return EXIT_FAILURE;
	}
	
	const std::unordered_set<std::string> args(argv + 1, argv + argc);
	if (args.contains("tests")) {
		std::cout << "NOTE: assertions might be omitted in release builds\n";
		test_all();
		return EXIT_SUCCESS;
	}

	const bool for_all_n = args.contains("all");
	const int k = find_k_from_args(args);
	if (!for_all_n && k == NO_K) {
		std::cout << "ERROR: missing <k> from args\n";
		return EXIT_FAILURE;
	}
	const auto graph_type = find_type_from_args(args);
	if (graph_type.first == NO_TYPE) {
		std::cout << "ERROR: missing <type> from args\n";
		return EXIT_FAILURE;
	}

	auto [lo, hi] = graph_type.second;
	if (!for_all_n && (k < lo || k > hi)) {
		std::cout << "ERROR: " << k << " is out of bounds for " << graph_type.first
			<< ", must be between " << lo << " and " << hi << "\n";
		return EXIT_FAILURE;
	}

	if (for_all_n) {
		for (int n = lo; n <= hi; ++n) {
			const auto g6 = get_graph6_file(graph_type.first, n);
			verify_g6_batch(g6);
			std::cout << "###\n";
		}
	}
	else {
		const auto g6 = get_graph6_file(graph_type.first, k);
		verify_g6_batch(g6);
	}
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

void verify_g6_batch(const std::string& file, bool verbose) {
	std::ifstream ofs(file);
	std::string line;

	const int num_graphs = get_line_count(file);
	int curr_graph = 1;

	while (std::getline(ofs, line)) {
		graph g = read_graph6(line);

		if (verbose) {
			std::cerr << "Processing graph " << curr_graph << " / " << num_graphs << " ...\n";
		}

		int num_cols = 0;
		Victory win = Victory::Bob;
		do {
			++num_cols;
			const auto gameplay = play_optimally(g, num_cols);
			win = gameplay.first;
		} while (win != Victory::Alice);

		std::cout << line << " " << num_cols << "\n";
		++curr_graph;
	}
}

int find_k_from_args(const std::unordered_set<std::string>& args) {
	for (const auto& arg : args) {
		if (std::all_of(arg.cbegin(), arg.cend(), std::isdigit)) {
			return std::stoi(arg);
		}
	}

	return NO_K;
}

std::pair<std::string, std::pair<int, int>> find_type_from_args(const std::unordered_set<std::string>& args) {
	for (const auto& arg : args) {
		const auto it = allowed_types.find(arg);

		if (it != allowed_types.cend()) {
			return *it;
		}
	}

	return { NO_TYPE, {NO_K, NO_K} };
}

std::string get_graph6_file(const std::string& family, int k) {
	return "C:\\Dropbox\\code\\graph-data\\" + family + "\\" + family + "-n" + std::to_string(k) + ".dat";
}