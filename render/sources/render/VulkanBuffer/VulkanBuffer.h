#pragma once

#include "render/VulkanMacros.h"

namespace Render
{
	class VulkanDevice;

	class VulkanBuffer
	{
	public:

		void Initialize(VulkanDevice& device, const vk::DeviceSize& size,
		                const vk::BufferUsageFlags& usage, const vk::MemoryPropertyFlags& properties);
		VulkanBuffer() = default;
		VulkanBuffer(VulkanBuffer& other) = delete;
		VulkanBuffer& operator=(const VulkanBuffer& other) = delete;

		VulkanBuffer(VulkanBuffer&& other) noexcept;

		~VulkanBuffer();

		[[nodiscard]] const vk::Buffer& GetBuffer() const;

		[[nodiscard]] const vk::DeviceMemory& GetBufferMemory() const;

	private:

		static uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties,
		                               vk::PhysicalDeviceMemoryProperties memProperties);

		VulkanDevice* vulkanDevice = nullptr;

		vk::Buffer buffer;
		vk::DeviceMemory bufferMemory;
	};
}
