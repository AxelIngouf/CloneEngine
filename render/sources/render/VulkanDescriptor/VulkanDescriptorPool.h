#pragma once

#include "render/VulkanMacros.h"

namespace Render
{
	class VulkanDescriptorPool
	{
	public:

		[[nodiscard]] static vk::UniqueDescriptorPool CreatePerFramePool(const class VulkanDevice& device);
		[[nodiscard]] static vk::UniqueDescriptorPool CreateLightingPool(const class VulkanDevice& device);
		[[nodiscard]] static vk::UniqueDescriptorPool CreateImguiPool(const class VulkanDevice& device);
		[[nodiscard]] static vk::UniqueDescriptorPool CreatePerObjectPool(const VulkanDevice& device);
		[[nodiscard]] static vk::UniqueDescriptorPool CreateDepthPool(const VulkanDevice& device);

		static vk::UniqueDescriptorPool imGuiPool;
	};
}
