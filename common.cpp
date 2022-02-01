#include "common.hpp"

#include <cassert>
#include <fstream>

int get_line_count(const std::string& file) {
	std::ifstream ifs(file);
	std::string line;

	int line_count = 0;
	for (; std::getline(ifs, line); ++line_count) {}

	return line_count;
}

bool next_combination(index_t* c, index_t n, index_t k) {
	if (c[k - 1] < n - 1) {
		++c[k - 1];
		return true;
	}

	std::int64_t j;

	for (j = k - 2; j >= 0; --j) {
		if (c[j] < n - k + j)
		{
			break;
		}
	}

	if (j < 0) {
		assert(c[0] == n - k);
		return false;
	}

	++c[j];

	for (; j < k - 1; ++j)
		c[j + 1] = c[j] + 1;

	return true;
}