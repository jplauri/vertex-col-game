#include "common.hpp"

#include <fstream>

int get_line_count(const std::string& file) {
	std::ifstream ifs(file);
	std::string line;

	int line_count = 0;
	for (; std::getline(ifs, line); ++line_count) {}

	return line_count;
}