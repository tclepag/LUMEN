#pragma once

#include <cstdint>
#include <array>

struct Vertex {
	std::array<float, 3> position;
	std::array<float, 3> normal;
	std::array<float, 3> texcoord;
};

using Index = uint32_t;