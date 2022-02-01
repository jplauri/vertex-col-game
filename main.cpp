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
#include <random>

void verify_g6_batch(const std::string& file, const std::string& out, bool verbose = true);

const std::unordered_map<std::string, std::pair<int, int>> allowed_types = {
	{"planar", {4, 11}},
	{"outerplanar", {4, 11}},
	{"simp", {3, 10}}
};

constexpr const int NO_K = -1;
const std::string NO_TYPE = "no_graph_type";

const std::string OUTPUT_DESTINATION = "C:\\Dropbox\\code\\vertex-col-game\\results\\";

int find_k_from_args(const std::unordered_set<std::string>& args);

std::pair<std::string, std::pair<int, int>> find_type_from_args(const std::unordered_set<std::string>& args);

bool contains_result(const std::string& file, const std::string& g);

std::string get_graph6_file(const std::string& family, int k);

std::string get_graph6_output(const std::string& family, int k);

int main(int argc, char** argv)
{
	//test_all();

	if (argc < 2 || argc > 4) {
		std::cout << "Usage: ./vertex-col-game <k> <type> [<all>] [<tests>]\n"
			<< "<k>:       the order of the family\n"
			<< "<type>:    the type of the family (e.g., outerplanar)\n"
			<< "<tests>:   whether to only run tests\n";
		return EXIT_FAILURE;
	}
	
	const std::unordered_set<std::string> args(argv + 1, argv + argc);
	if (args.contains("tests")) {
		std::cout << "NOTE: assertions might be omitted in release builds\n";
		test_all();
		return EXIT_SUCCESS;
	}

	const int k = find_k_from_args(args);
	if (k == NO_K) {
		std::cout << "ERROR: missing <k> from args\n";
		return EXIT_FAILURE;
	}
	const auto graph_type = find_type_from_args(args);
	if (graph_type.first == NO_TYPE) {
		std::cout << "ERROR: unrecognized or missing <type> from args\n";
		return EXIT_FAILURE;
	}

	auto [lo, hi] = graph_type.second;
	if (k < lo || k > hi) {
		std::cout << "ERROR: " << k << " is out of bounds for " << graph_type.first
			<< ", must be between " << lo << " and " << hi << "\n";
		return EXIT_FAILURE;
	}

	const auto g6 = get_graph6_file(graph_type.first, k);
	const auto out = get_graph6_output(graph_type.first, k);

	verify_g6_batch(g6, out);
}

bool contains_result(const std::string& file, const std::string& g) {
	std::ifstream ifs(file);
	std::string line;

	while (std::getline(ifs, line)) {
		const auto cutoff = std::string(line.begin(), std::find(line.begin(), line.end(), ' '));
		if (cutoff == g)
			return true;
	}

	return false;
}

void verify_g6_batch(const std::string& file, const std::string& out, bool verbose) {
	std::ifstream ifs(file);
	std::string line;

	const int num_graphs = get_line_count(file);
	int curr_graph = 0;

	while (std::getline(ifs, line)) {
		++curr_graph;
		graph g = read_graph6(line);

		if (verbose) {
			std::cerr << "Processing graph " << curr_graph << " / " << num_graphs << " ...\n";
		}

		if (contains_result(out, line)) {
			continue;
		}

		int num_cols = 0;

		// Start from 4 colors (note increment)
		if (has_k_four(g)) {
			num_cols = 3;
		}
		else {
			// Start from 3 colors (note increment)
			if (has_triangle(g)) {
				num_cols = 2;
			}
		}

		Victory win = Victory::Bob;
		do {
			++num_cols;
			const auto gameplay = play_optimally(g, num_cols);
			win = gameplay.first;
		} while (win != Victory::Alice);

		std::cout << line << " " << num_cols << "\n";
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

std::string get_graph6_output(const std::string& family, int k) {
	return OUTPUT_DESTINATION + family + "-n" + std::to_string(k) + ".result";
}