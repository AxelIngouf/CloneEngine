#pragma once

#include "render/VulkanMacros.h"

namespace Render
{
	class VulkanDevice;

	class VulkanDescriptorSetLayout
	{
	public:
		[[nodiscard]] static vk::UniqueDescriptorSetLayout CreatePerFrameLayout(const VulkanDevice& device);
		[[nodiscard]] static vk::UniqueDescriptorSetLayout CreateLightingLayout(const VulkanDevice& device);
		[[nodiscard]] static vk::UniqueDescriptorSetLayout CreateImguiLayout(const VulkanDevice& device);
		[[nodiscard]] static vk::UniqueDescriptorSetLayout CreatePerObjectLayout(const VulkanDevice& device);
		[[nodiscard]] static vk::UniqueDescriptorSetLayout CreateDepthLayout(const VulkanDevice& device);
	};
}
