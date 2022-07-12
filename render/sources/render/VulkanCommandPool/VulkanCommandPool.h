#pragma once

#include <mutex>

#include "../VulkanMacros.h"

class EditorUI;

namespace Render
{
	class VulkanVertexBuffer;
	class VulkanTextureSampler;
	class VulkanQueryPool;
	class VulkanBuffer;
	class VulkanPipeline;
	class VulkanSwapchain;
	class VulkanDevice;
	class Drawable;

	class VulkanCommandPool
	{
	public:
		VulkanCommandPool() = default;

		void ResetCommandPool();

		void CreateCommandPool(VulkanDevice& graphicsDevice);
		void CreateCommandBuffer();


		[[nodiscard]] vk::CommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(vk::CommandBuffer& buffer);

		void RecordCommandBuffer(const vk::Framebuffer& framebuffer,
		                         const vk::Framebuffer& offscreenFramebuffer,
		                         const vk::Framebuffer& depthFramebuffer,
		                         const VulkanSwapchain& swapchain,
		                         VulkanPipeline& pipeline,
		                         vk::DescriptorSet frameDescriptor,
		                         vk::DescriptorSet depthDescriptor,
		                         vk::DescriptorSet lightDescriptor,
		                         const vk::Extent2D& viewportSize,
		                         VulkanVertexBuffer& debugLines,
		                         const VulkanQueryPool& queryPool);

		void CopyBuffer(VulkanBuffer& src, VulkanBuffer& dst, vk::DeviceSize size, bool mutexLocked = false);

		[[nodiscard]] const vk::CommandBuffer* GetCommandBuffer() const
		{
			return &commandBuffer;
		}

		[[nodiscard]] vk::CommandPool GetCommandPool() const;

		[[nodiscard]] std::mutex& GetCommandPoolMutex() { return commandPoolMutex; }

	private:

		VulkanDevice* device = nullptr;

		vk::UniqueCommandPool commandPool;
		vk::CommandBuffer commandBuffer;

		std::mutex commandPoolMutex;

		VulkanVertexBuffer* cubemapVertices = nullptr;
	};
}
