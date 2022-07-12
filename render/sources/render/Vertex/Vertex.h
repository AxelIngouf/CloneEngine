#pragma once

#include "model/Vertex.h"

#include "render/VulkanMacros.h"

namespace Render
{
	struct Vertex : Model::Vertex
	{
		Vertex(const Model::Vertex& other)
		{
			position = other.position;
			normal = other.normal;
			texCoords = other.texCoords;
		}

		static vk::VertexInputBindingDescription GetBindingDescription()
		{
			vk::VertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = vk::VertexInputRate::eVertex;

			return bindingDescription;
		}

		static std::vector<vk::VertexInputAttributeDescription> GetAttributeDescriptions()
		{
			std::vector<vk::VertexInputAttributeDescription> attributeDescriptions{};

			attributeDescriptions.resize(3);

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
			attributeDescriptions[0].offset = offsetof(Vertex, position);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
			attributeDescriptions[1].offset = offsetof(Vertex, normal);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = vk::Format::eR32G32Sfloat;
			attributeDescriptions[2].offset = offsetof(Vertex, texCoords);

			return attributeDescriptions;
		}
	};
}
