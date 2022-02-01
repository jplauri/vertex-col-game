#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstdint>
#include <limits>
#include <string>

typedef std::uint64_t index_t;

static constexpr index_t BIT_LEN = sizeof(index_t) * 8;
static constexpr index_t ALL_ONES = std::numeric_limits<index_t>::max();

static constexpr index_t BINOMIAL[64] = {
	0, 0, 1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 66, 78, 91, 105, 120, 136,
	153, 171, 190, 210, 231, 253, 276, 300, 325, 351, 378, 406, 435, 465,
	496, 528, 561, 595, 630, 666, 703, 741, 780, 820, 861, 903, 946, 990,
	1035, 1081, 1128, 1176, 1225, 1275, 1326, 1378, 1431, 1485, 1540,
	1596, 1653, 1711, 1770, 1830, 1891, 1953
};



[[nodiscard]] int get_line_count(const std::string& file);

bool next_combination(index_t* c, index_t n, index_t k);

#endif