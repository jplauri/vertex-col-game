#ifndef MOVE_HPP
#define MOVE_HPP

struct move {
	move() { }
	move(int vertex, int color) : vertex_(vertex), color_(color) { }

	int vertex_{ -1 };
	int color_{ -1 };
};

#endif