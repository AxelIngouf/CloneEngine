#pragma once

#include "../../../../physic/sources/physic/PhysicsManager.h"

#include "render/VulkanMacros.h"

namespace Render
{
	struct PhysicsDebugVertex : Physics::DebugVertex
	{
		PhysicsDebugVertex(const DebugVertex& other)
		{
			position = other.position;
			color = other.color;
		}

		static vk::VertexInputBindingDescription GetBindingDescription()
		{
			vk::VertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(DebugVertex);
			bindingDescription.inputRate = vk::VertexInputRate::eVertex;

			return bindingDescription;
		}

		static std::vector<vk::VertexInputAttributeDescription> GetAttributeDescriptions()
		{
			std::vector<vk::VertexInputAttributeDescription> attributeDescriptions{};
			attributeDescriptions.resize(2);

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
			attributeDescriptions[0].offset = offsetof(DebugVertex, position);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
			attributeDescriptions[1].offset = offsetof(DebugVertex, color);

			return attributeDescriptions;
		}
	};
}
