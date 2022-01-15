#ifndef VERTEX_COLORING_HPP
#define VERTEX_COLORING_HPP

#include "common.hpp"
#include "graph.hpp"

#include <algorithm>
#include <cassert>
#include <bitset>

class vertex_coloring {
  public:
    vertex_coloring(const graph& g, int num_cols) 
        : g_(g), 
        num_cols_(num_cols), 
        col_(g.num_vertices(), 0), 
        attack_(g.num_vertices(), std::vector<index_t>(num_cols))
    { 
        assert(num_cols_ > 0 && num_cols_ < BIT_LEN);

        check_invariant();
    }

    void color_vertex(index_t u, index_t c);
    void uncolor_vertex(index_t u, index_t c);

    index_t get_color(index_t u) const;
    index_t get_allowed_colors(index_t u) const;
    bool has_free_color(index_t u) const;

    int num_colored_vertices() const;
    int num_vertices() const;

    bool is_colored() const;
    bool is_colored(index_t u, index_t c) const;
    bool is_colored(index_t u) const;
    
    bool is_allowed(index_t u, index_t c) const;
    bool is_deadend() const;
    bool has_conflict() const;
    bool neighbor_has_color(index_t u, index_t c) const;

    void print() const;

  private:
    void at_most_one_color_per_vertex() const;
    void at_most_deg_attackers_per_vertex() const;
    void check_invariant() const;

    void attack_neighbors(index_t adj, index_t c);
    void free_neighbors(index_t adj, index_t c);

    const graph& g_;
    std::vector<index_t> col_;
    std::vector<std::vector<index_t>> attack_;
    const int num_cols_;
    int colored_vertices_{ 0 };
};

#endif