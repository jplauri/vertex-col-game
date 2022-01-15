#include "vertex_coloring.hpp"

#include <cassert>

namespace {
	static constexpr index_t INVALID = 128;
	static_assert(INVALID >= BIT_LEN);
}

void vertex_coloring::color_vertex(index_t u, index_t c) {
	assert(u >= 0 && u < g_.num_vertices());
	assert(c >= 0 && c < num_cols_);
	assert(!is_colored(u, c)); 
	check_invariant();

	// Color vertex u with color c
	col_[u] |= (1ULL << c);
	++colored_vertices_;
	attack_neighbors(g_.get_neighbors(u), c);

	assert(is_colored(u, c));
	check_invariant();
}

void vertex_coloring::uncolor_vertex(index_t u, index_t c) {
	assert(u >= 0 && u < g_.num_vertices());
	assert(c >= 0 && c < num_cols_);
	assert(is_colored(u, c));
	check_invariant();

	// Uncolor vertex u with color c
	col_[u] &= ~(1ULL << c);
	--colored_vertices_;
	free_neighbors(g_.get_neighbors(u), c);

	assert(!is_colored(u, c));
	check_invariant();
}

index_t vertex_coloring::get_color(index_t u) const {
	assert(is_colored(u));

	index_t u_col = col_[u];
	unsigned long j = INVALID;

	_BitScanForward64(&j, u_col);
	assert(j != INVALID);

	return j;
}

index_t vertex_coloring::get_allowed_colors(index_t u) const {
	assert(u >= 0 && u < g_.num_vertices());

	index_t allowed = 0;
	for (index_t i = 0; i < attack_[u].size(); ++i) {
		if (attack_[u][i] == 0) {
			allowed |= 1ULL << i;
		}
	}

	return allowed;
}

bool vertex_coloring::has_free_color(index_t u) const {
	for (auto e : attack_[u]) {
		if (e == 0)
			return true;
	}

	return false;
}

int vertex_coloring::num_colored_vertices() const {
	return colored_vertices_;
}

int vertex_coloring::num_vertices() const {
	return g_.num_vertices();
}

bool vertex_coloring::is_colored() const {
	return colored_vertices_ == g_.num_vertices();
}

bool vertex_coloring::is_colored(index_t u, index_t c) const {
	return (col_[u] >> c) & 1ULL;
}

bool vertex_coloring::is_colored(index_t u) const {
	at_most_one_color_per_vertex();
	return std::has_single_bit(col_[u]);
}

bool vertex_coloring::is_allowed(index_t u, index_t c) const {
	return attack_[u][c] == 0;
}

bool vertex_coloring::is_deadend() const {
	for (index_t i = 0; i < col_.size(); ++i) {
		if (!is_colored(i) && !has_free_color(i)) {
			return true;
		}
	}

	return false;
}

bool vertex_coloring::has_conflict() const {
	for (index_t i = 0; i < g_.num_vertices(); ++i) {
		if (!is_colored(i))
			continue;

		const auto i_col = get_color(i);
		if (neighbor_has_color(i, i_col)) {
			return true;
		}
	}

	return false;
}

bool vertex_coloring::neighbor_has_color(index_t u, index_t c) const {
	index_t adj = g_.get_neighbors(u);

	for (unsigned long j; adj != 0; adj &= ~(1ULL << j))
	{
		_BitScanForward64(&j, adj);
		if ((col_[j] >> c) & 1U) {
			return true;
		}
	}

	return false;
}

void vertex_coloring::print() const {
	for (index_t i = 0; i < g_.num_vertices(); ++i) {
		std::vector<int> pos;
		get_bit_positions(col_[i], std::back_inserter(pos));

		if (pos.empty())
			std::cout << "c(" << i << ") = UNASSIGNED\n";
		else
			std::cout << "c(" << i << ") = " << pos[0] << "\n";

		
	}

	for (index_t i = 0; i < attack_.size(); ++i) {
		std::cout << "attack_[" << i << "] = ";
		for (auto e : attack_[i]) {
			std::cout << e << " ";
		}
		std::cout << "\n";
	}
}

void vertex_coloring::at_most_one_color_per_vertex() const {
	for (index_t i = 0; i < col_.size(); ++i) {
		assert(col_[i] == 0 || std::has_single_bit(col_[i]) && "Invariant violated: AtMostOne");
	}
}

void vertex_coloring::at_most_deg_attackers_per_vertex() const {
	for (index_t i = 0; i < attack_.size(); ++i) {
		for (auto e : attack_[i]) {
			assert(e >= 0);
			assert(e <= g_.get_degree(i));
		}
	}
}

void vertex_coloring::check_invariant() const {
	at_most_one_color_per_vertex();
	at_most_deg_attackers_per_vertex();

	assert(colored_vertices_ >= 0 && colored_vertices_ <= g_.num_vertices());
}

void vertex_coloring::attack_neighbors(index_t adj, index_t c) {
	for (unsigned long j; adj != 0; adj &= ~(1ULL << j))
	{
		_BitScanForward64(&j, adj);
		++attack_[j][c];
	}
}

void vertex_coloring::free_neighbors(index_t adj, index_t c) {
	for (unsigned long j; adj != 0; adj &= ~(1ULL << j))
	{
		_BitScanForward64(&j, adj);
		--attack_[j][c];
	}
}