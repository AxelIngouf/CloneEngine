#pragma once

#include "render/VulkanBuffer/VulkanBuffer.h"
#include "Vector/Vector4.h"

namespace vk
{
	class Buffer;
}

namespace Render
{
	class VulkanDevice;

	class VulkanLightBuffers
	{
	public:
		VulkanLightBuffers(VulkanDevice& device);

		void Update(const VulkanDevice& device);

		[[nodiscard]] const vk::Buffer& GetBuffer() const { return buffer.GetBuffer(); }

		struct DirectionalLightStorage
		{
			LibMath::Vector4 direction;
			LibMath::Vector4 ambient;
			LibMath::Vector4 diffuse;
			LibMath::Vector4 specular;

			uint32_t isActive = false;
		};

	private:

		VulkanBuffer buffer;

		DirectionalLightStorage dirLight;
	};
}
