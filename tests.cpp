#include "tests.hpp"
#include "graph.hpp"
#include "vertex_coloring.hpp"
#include "common.hpp"
#include "minimax.hpp"

#include <cassert>
#include <bitset>

namespace {
	// A 4-cycle with a chord and a pendant
	graph get_test_graph() {
		graph g(5);
		g.add_edge(0, 1);
		g.add_edge(0, 2);
		g.add_edge(0, 3);
		g.add_edge(1, 2);
		g.add_edge(2, 3);
		g.add_edge(2, 4);

		return g;
	}
}

void test_all() {
	test_graph();
	test_color_and_uncolor();
	test_full_coloring();
	test_deadend();
	test_minimax();
}

void test_graph() {
	std::cout << "Testing graph functionalities ... ";

	{
		graph g = get_test_graph();

		assert(g.num_vertices() == 5 && "Unexpected number of vertices");
		assert(g.num_edges() == 6 && "Unexpected number of edges");

		const std::vector<index_t> degs = { 3, 2, 4, 2, 1 };
		assert(degs.size() == g.num_vertices());

		for (index_t i = 0; i < degs.size(); ++i) {
			assert(degs[i] == g.get_degree(i) && "Unexpected degree");
		}
	}

	{
		for (int i = 3; i < 64; ++i) {
			graph g = get_complete_graph(i);

			assert(g.num_vertices() == i);
			assert(BINOMIAL[i] == g.num_edges());
		}
	}

	{
		for (int i = 4; i < 64; ++i) {
			graph g = get_cycle(i);

			assert(g.num_vertices() == i);
			assert(g.num_edges() == i);
		}
	}
	std::cout << "OK\n";
}

void test_color_and_uncolor() {
	std::cout << "Testing color and uncolor functionalities ... ";

	graph g = get_test_graph();
	const int NUM_COLS = 3;
	vertex_coloring col(g, NUM_COLS);

	for (index_t i = 0; i < g.num_vertices(); ++i) {
		assert(std::popcount(col.get_allowed_colors(i)) == NUM_COLS);
	}

	col.color_vertex(0, 0);

	assert(std::popcount(col.get_allowed_colors(1)) == NUM_COLS - 1);
	assert(std::popcount(col.get_allowed_colors(2)) == NUM_COLS - 1);
	assert(std::popcount(col.get_allowed_colors(3)) == NUM_COLS - 1);
	assert(std::popcount(col.get_allowed_colors(4)) == NUM_COLS);

	col.uncolor_vertex(0, 0);
	assert(std::popcount(col.get_allowed_colors(0)) == NUM_COLS);

	for (index_t i = 1; i < g.num_vertices(); ++i) {
		assert(std::popcount(col.get_allowed_colors(i)) == NUM_COLS);
	}

	col.color_vertex(0, 0);

	assert(std::popcount(col.get_allowed_colors(1)) == NUM_COLS - 1);
	assert(std::popcount(col.get_allowed_colors(2)) == NUM_COLS - 1);
	assert(std::popcount(col.get_allowed_colors(3)) == NUM_COLS - 1);
	assert(std::popcount(col.get_allowed_colors(4)) == NUM_COLS);

	std::cout << "OK\n";
}

void test_full_coloring() {
	std::cout << "Testing full graph coloring ... ";

	graph g = get_test_graph();
	const int NUM_COLS = 3;
	vertex_coloring col(g, NUM_COLS);

	// Color each vertex & ensure the graph is fully colored.
	// Backtrack by uncoloring the first vertex and re-check.

	col.color_vertex(0, 0);
	assert(!col.is_colored());
	col.color_vertex(1, 1);
	assert(!col.is_colored());
	col.color_vertex(2, 2);
	assert(!col.is_colored());
	col.color_vertex(3, 1);
	assert(!col.is_colored());
	col.color_vertex(4, 0);
	assert(col.is_colored());

	col.uncolor_vertex(0, 0);
	assert(!col.is_colored());

	std::vector<index_t> zero_cols;
	get_bit_positions(col.get_allowed_colors(0), std::back_inserter(zero_cols));

	assert(zero_cols.size() == 1 && zero_cols[0] == 0);

	col.color_vertex(0, 0);
	assert(col.is_colored());

	std::cout << "OK\n";
}

void test_deadend() {
	std::cout << "Testing graph coloring deadend ... ";

	{
		graph g(4);
		g.add_edge(2, 0);
		g.add_edge(3, 1);
		g.add_edge(3, 2);

		const int NUM_COLS = 4;
		vertex_coloring col(g, NUM_COLS);

		col.color_vertex(0, 0);
		col.color_vertex(1, 0);

		assert(!col.is_colored());
		assert(!col.is_deadend());
		assert(!col.has_conflict());
	}

	{
		graph g = get_complete_graph(3);
		const int NUM_COLS = 2;
		vertex_coloring col(g, NUM_COLS);

		col.color_vertex(0, 0);
		col.color_vertex(1, 1);

		assert(col.is_deadend());
	}

	{
		graph g = get_complete_graph(3);
		const int NUM_COLS = 3;
		vertex_coloring col(g, NUM_COLS);

		col.color_vertex(0, 0);
		assert(!col.is_deadend());
		assert(!col.is_colored());

		col.color_vertex(1, 1);
		assert(!col.is_deadend());
		assert(!col.is_colored());

		col.color_vertex(2, 2);
		assert(col.is_colored());
		assert(!col.is_deadend());
	}

	{
		// k-chromatic cliques
		for (int i = 3; i < 64; ++i) {
			graph g = get_complete_graph(i);
			const int NUM_COLS = i;
			vertex_coloring col(g, NUM_COLS);

			// At first, any vertex can be colored with any color
			for (index_t i = 0; i < g.num_vertices(); ++i) {
				for (index_t c = 0; c < NUM_COLS; ++c) {
					assert(col.is_allowed(i, c));
				}
			}

			col.color_vertex(0, 0);

			for (index_t i = 1; i < g.num_vertices(); ++i) {
				assert(!col.is_allowed(i, 0));
			}


			col.uncolor_vertex(0, 0);
			for (index_t i = 0; i < g.num_vertices(); ++i) {
				col.color_vertex(i, i);
			}

			for(index_t i = 0; i < g.num_vertices(); ++i)
				assert(!col.neighbor_has_color(i, i));

			assert(col.is_colored());
			assert(!col.has_conflict());
		}
	}

	{
		// 2-coloring even cycles
		const int NUM_COLS = 2;

		for (int i = 4; i < 64; ++i) {
			if (i % 2 == 0) {
				graph g = get_cycle(i);
				vertex_coloring col(g, NUM_COLS);

				bool c = false;
				for (int j = 0; j < g.num_vertices(); ++j) {
					col.color_vertex(j, static_cast<int>(c));
					c = !c;
				}

				assert(col.is_colored());
			}
		}
	}

	{
		graph g(4);
		g.add_edge(0, 1);
		g.add_edge(0, 2);
		g.add_edge(0, 3);

		const int NUM_COLS = 2;
		vertex_coloring col(g, NUM_COLS);

		col.color_vertex(0, 0);
		col.color_vertex(1, 0);
		col.color_vertex(2, 0);
		col.color_vertex(3, 0);
		assert(col.has_conflict());

		col.uncolor_vertex(0, 0);

		col.color_vertex(0, 1);
		assert(!col.has_conflict());
	}

	{
		graph g(3);
		g.add_edge(0, 1);
		g.add_edge(1, 2);

		const int NUM_COLS = 2;
		vertex_coloring col(g, NUM_COLS);

		// Endpoints of P_3 colored in distinct colors
		col.color_vertex(0, 0);
		col.color_vertex(2, 1);

		// At a deadend, but no conflict yet
		assert(col.is_deadend());
		assert(!col.is_colored());
		assert(!col.has_conflict());
	}

	{
		graph g(3);
		g.add_edge(0, 1);
		g.add_edge(1, 2);

		const int NUM_COLS = 2;
		vertex_coloring col(g, NUM_COLS);

		col.color_vertex(0, 0);
		col.color_vertex(1, 1);

		assert(!col.is_deadend());
		assert(!col.is_colored());
		assert(!col.has_conflict());
	}

	{
		graph g(6);

		g.add_edge(0, 1);
		g.add_edge(0, 2);
		g.add_edge(0, 3);
		g.add_edge(1, 4);
		g.add_edge(1, 5);

		const int NUM_COLS = 2;
		vertex_coloring col(g, NUM_COLS);


	}

	std::cout << "OK\n";
}

void test_minimax() {
	std::cout << "Testing minimax ... ";

	{
		// Alice wins with k colors
		graph g(4);
		g.add_edge(0, 1);
		g.add_edge(1, 2);
		g.add_edge(2, 3);

		const int NUM_COLS = 4;
		vertex_coloring col(g, NUM_COLS);

		auto gameplay = play_optimally(g, NUM_COLS);
		assert(gameplay.first == Victory::Alice);
	}

	{
		// Alice wins on a star by coloring the center 
		for (int i = 3; i < 8; ++i) {
			graph g = get_star(i);
			const int NUM_COLS = 2;
			vertex_coloring col(g, NUM_COLS);

			auto gameplay = play_optimally(g, NUM_COLS);
			assert(gameplay.first == Victory::Alice);
		}
	}

	std::cout << "OK\n";
}