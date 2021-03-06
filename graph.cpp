#include "graph.hpp"

#include <cassert>
#include <bit>
#include <numeric>

namespace {
	static constexpr int BIAS6 = 63;
	static constexpr int SMALLN = 62;
	static constexpr int SMALLISHN = 258047;
	static constexpr int TOPBIT6 = 32;

	int get_graph_size(const std::string& s)
	{
		auto p = (s[0] == ':' || s[0] == '&') ? s.cbegin() + 1 : s.cbegin();
		int n = *p++ - BIAS6;

		if (n > SMALLN)
		{
			n = *p++ - BIAS6;
			if (n > SMALLN)
			{
				n = *p++ - BIAS6;
				n = (n << 6) | (*p++ - BIAS6);
				n = (n << 6) | (*p++ - BIAS6);
				n = (n << 6) | (*p++ - BIAS6);
				n = (n << 6) | (*p++ - BIAS6);
				n = (n << 6) | (*p++ - BIAS6);
			}
			else
			{
				n = (n << 6) | (*p++ - BIAS6);
				n = (n << 6) | (*p++ - BIAS6);
			}
		}

		return n;
	}
}

void graph::add_edge(index_t u, index_t v) {
	assert(u >= 0 &&
		v >= 0 &&
		u != v &&
		u < adj_.size() &&
		v < adj_.size());

	adj_[u] |= (1ULL << v);
	adj_[v] |= (1ULL << u);
	++m_;
}

index_t graph::get_degree(index_t u) const {
	assert(u >= 0 && u < num_vertices());
	return std::popcount(adj_[u]);
}

index_t graph::num_vertices() const {
	return adj_.size();
}

index_t graph::num_edges() const {
	return m_;
}

index_t graph::get_neighbors(index_t u) const {
	assert(u >= 0 && u < num_vertices());
	return adj_[u];
}

bool graph::has_edge(index_t u, index_t v) const {
	assert(u >= 0 &&
		v >= 0 &&
		u != v &&
		u < adj_.size() &&
		v < adj_.size());

	return (adj_[u] >> v) & 1ULL;
}

bool has_triangle(const graph& g) {
	if (g.num_vertices() < 3 || g.num_edges() < 3) {
		return false;
	}

	std::vector<index_t> idx = { 0, 1, 2 };

	do {
		if (g.has_edge(idx[0], idx[1]) && g.has_edge(idx[0], idx[2]) && g.has_edge(idx[1], idx[2])) {
			return true;
		}
	} while (next_combination(idx.data(), g.num_vertices(), 3));

	return false;
}

bool has_k_four(const graph& g) {
	if (g.num_vertices() < 4 || g.num_edges() < 6) {
		return false;
	}

	std::vector<index_t> idx = { 0, 1, 2, 3 };

	do {
		if (
			g.has_edge(idx[0], idx[1]) && 
			g.has_edge(idx[0], idx[2]) && 
			g.has_edge(idx[0], idx[3]) &&
			g.has_edge(idx[1], idx[2]) &&
			g.has_edge(idx[1], idx[3]) &&
			g.has_edge(idx[2], idx[3]))
		{
			return true;
		}
	} while (next_combination(idx.data(), g.num_vertices(), 4));

	return false;
}

graph read_graph6(const std::string& s) {
	const int n = get_graph_size(s);
	graph g(n);

	#define SIZELEN(n) ((n)<=SMALLN?1:((n)<=SMALLISHN?4:8))
	auto p = s.cbegin() + (s[0] == ':' || s[0] == '&') + SIZELEN(n);

	int k = 0;
	int v = 0;
	int x = 0;

	if (s[0] != ':' && s[0] != '&') /* graph6 format */
	{
		k = 1;

		for (int j = 1; j < n; ++j)
		{
			for (int i = 0; i < j; ++i)
			{
				if (--k == 0)
				{
					k = 6;
					x = *(p++) - BIAS6;
				}

				if ((x & TOPBIT6))
				{
					g.add_edge(j, i);
				}
				x <<= 1;
			}
		}
	}
	else if (s[0] == '&')
	{
		k = 1;
		for (int i = 0; i < n; ++i)
		{
			for (int j = 0; j < n; ++j)
			{
				if (--k == 0)
				{
					k = 6;
					x = *(p++) - BIAS6;
				}

				if ((x & TOPBIT6))
				{
					g.add_edge(j, i);
				}
				x <<= 1;
			}
		}
	}

	return g;
}

graph get_complete_graph(int n) {
	assert(n > 0 && n <= 64);

	graph g(n);

	for (index_t i = 0; i < n; ++i) {
		for (index_t j = (i + 1); j < n; ++j) {
			g.add_edge(i, j);
		}
	}

	return g;
}

graph get_cycle(int n) {
	assert(n >= 4 && n <= 64);

	graph g(n);
	const int last_vertex = n - 1;

	for (int i = 0; i < last_vertex; ++i) {
		g.add_edge(i, i + 1);
	}

	g.add_edge(0, last_vertex);
	return g;
}

graph get_star(int n) {
	assert(n >= 3);

	graph g(n);
	for (int i = 1; i < n; ++i) {
		g.add_edge(0, i);
	}

	return g;
}