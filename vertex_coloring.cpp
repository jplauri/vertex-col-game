#include "vertex_coloring.hpp"

#include <cassert>

void vertex_coloring::color_vertex(index_t u, index_t c) {
	assert(u >= 0 && u < g_.num_vertices());
	assert(c >= 0 && c < num_cols_);
	assert(!is_colored(u, c)); 
	check_invariant();

	col_[u] = c;
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

	col_[u] = unassigned_;
	--colored_vertices_;
	free_neighbors(g_.get_neighbors(u), c);

	assert(!is_colored(u, c));
	check_invariant();
}

index_t vertex_coloring::get_color(index_t u) const {
	assert(is_colored(u));
	assert(col_[u] != unassigned_);
	return col_[u];
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
	return col_[u] == c;
}

bool vertex_coloring::is_colored(index_t u) const {
	at_most_one_color_per_vertex();
	return col_[u] != unassigned_;
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
	return attack_[u][c] != 0;
}

bool vertex_coloring::equal(const vertex_coloring other) const {
	return col_ == other.col_;
}

std::size_t vertex_coloring::zobrist_hash() const {
	std::size_t h = 0;

	for (auto i = 0; i < col_.size(); ++i) {
		if (col_[i] != unassigned_) {
			h ^= zobrist_[i][col_[i]];
		}
	}

	return h;
}

void vertex_coloring::print() const {
	for (index_t i = 0; i < g_.num_vertices(); ++i) {
		if(col_[i] == unassigned_)
			std::cout << "c(" << i << ") = UNASSIGNED\n";
		else
			std::cout << "c(" << i << ") = " << col_[i] << "\n";
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
		assert(col_[i] == unassigned_ || true && "Invariant violated: AtMostOne");
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
#if defined(_MSC_VER)
	for (unsigned long j; adj != 0; adj &= ~(1ULL << j))
	{
		_BitScanForward64(&j, adj);
		++attack_[j][c];
	}
#elif defined(__GNUC__)
	while (adj != 0) {
		const auto j = __builtin_ctzll(adj);
		const auto lb = tmp & -tmp;
		adj ^= lb;

		++attack_[j][c];
	}
#endif
}

void vertex_coloring::free_neighbors(index_t adj, index_t c) {
#if defined(_MSC_VER)
	for (unsigned long j; adj != 0; adj &= ~(1ULL << j))
	{
		_BitScanForward64(&j, adj);
		--attack_[j][c];
	}
#elif defined(__GNUC__)
	while (adj != 0) {
		const auto j = __builtin_ctzll(adj);
		const auto lb = tmp & -tmp;
		adj ^= lb;

		--attack_[j][c];
	}
#endif
}

bool operator==(const vertex_coloring& c1, const vertex_coloring& c2) {
	return c1.equal(c2);
}