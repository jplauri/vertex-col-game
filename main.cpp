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

int main(int argc, char** argv)
{
	const std::string input_g(argv[1]);
	const graph g = read_graph6(input_g);
	int num_cols = 0;

	if (has_k_four(g)) {
		num_cols = 3;
	}
	else {
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

	std::cout << input_g << " " << num_cols << "\n";
}