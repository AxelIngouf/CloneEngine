#pragma once
#include <string>
#include <utility>
#include <vector>

#include "Vertex.h"

namespace Model
{
	class Mesh
	{
	public:
		Mesh(std::vector<Vertex> vert, std::vector<unsigned int> idx, std::string materialName) :
			vertices(std::move(vert)), indices(std::move(idx)), materialName(std::move(materialName))
		{
		}

		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::string materialName;
	};
}
