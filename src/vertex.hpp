#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <array>

namespace Renderer 
{

struct Vertex
{
	std::array<float, 3> position;
	std::array<float, 3> color;
	std::array<float, 2> texture;
};

}

#endif // VERTEX_HPP