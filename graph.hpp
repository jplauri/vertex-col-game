#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "common.hpp"
#include <vector>
#include <cassert>

#include <iostream>

class graph {
  public:
	graph(int n) : adj_(n), m_(0) { }
	graph& operator=(const graph&) = delete;

	void add_edge(index_t u, index_t v);

	index_t get_degree(index_t u) const;

	index_t num_vertices() const;

	index_t num_edges() const;

	index_t get_neighbors(index_t u) const;

	void print() const;

  private:
	std::vector<index_t> adj_;
	int m_{ 0 };
};

graph read_graph6(const std::string& s);

graph get_complete_graph(int n);

graph get_cycle(int n);

graph get_star(int n);

#endif