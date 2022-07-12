#pragma once

#include "Vector/Vector.h"

namespace Model
{
	struct Vertex
	{
		Vertex() = default;

		Vertex(const LibMath::Vector3 pos, const LibMath::Vector3 norm, const LibMath::Vector2 tex) :
			position(pos), normal(norm), texCoords(tex)
		{
		}

		LibMath::Vector3 position;
		LibMath::Vector3 normal;
		LibMath::Vector2 texCoords;
	};
}
