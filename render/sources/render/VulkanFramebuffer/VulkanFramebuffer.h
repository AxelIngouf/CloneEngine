#pragma once

#include "../VulkanMacros.h"


namespace Render
{
	class VulkanDevice;
	class VulkanSwapchain;

	class VulkanFramebuffer
	{
	public:

		void Initialize(const VulkanDevice& _device);

		void InitializeOffscreenFramebuffer(uint32_t width, uint32_t height,
		                                    const vk::RenderPass& renderPass);
		void InitializeDepthFramebuffer(uint32_t width, uint32_t height,
		                                const vk::RenderPass& renderPass);
		void InitializeSwapchainFramebuffer(int idx, const VulkanSwapchain& swapchain,
		                                    const vk::RenderPass& renderPass);

		[[nodiscard]] const vk::Framebuffer& GetFramebuffer() const { return framebuffer.get(); }
		[[nodiscard]] const vk::ImageView& GetImageView() const { return attachmentViews[0].get(); }
		[[nodiscard]] const vk::ImageView& GetDepthView() const { return attachmentViews[1].get(); }


	private:
		vk::UniqueFramebuffer framebuffer;

		const VulkanDevice* device = nullptr;

		std::vector<vk::UniqueImage> attachmentImages;
		std::vector<vk::UniqueDeviceMemory> attachmentMemory;
		std::vector<vk::UniqueImageView> attachmentViews;

		vk::UniqueSampler* depthSampler = nullptr;
	};
}
