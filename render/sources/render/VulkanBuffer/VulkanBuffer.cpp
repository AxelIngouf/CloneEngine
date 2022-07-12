#include "VulkanBuffer.h"

#include "render/VulkanDevice/VulkanDevice.h"

namespace Render
{
	void VulkanBuffer::Initialize(VulkanDevice& device, const vk::DeviceSize& size,
	                              const vk::BufferUsageFlags& usage, const vk::MemoryPropertyFlags& properties)
	{
		vulkanDevice = &device;

		// Buffer creation
		vk::BufferCreateInfo bufferInfo = {};
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = vk::SharingMode::eExclusive;

		buffer = device->createBuffer(bufferInfo).value;
		ASSERT(buffer, "Could not create buffer. ", Core::ELogChannel::CLOG_RENDER);

		// Buffer memory creation
		vk::MemoryRequirements memoryRequirements;
		device->getBufferMemoryRequirements(buffer, &memoryRequirements);

		vk::MemoryAllocateInfo allocInfo = {};
		allocInfo.allocationSize = memoryRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(memoryRequirements.memoryTypeBits, properties,
		                                           device.GetMemoryProperties());

		const auto bufferMemoryAllocationResult = device->allocateMemory(allocInfo);
		bufferMemory = bufferMemoryAllocationResult.value;
		ASSERT(bufferMemory, "Could not create buffer memory. " + vk::to_string(bufferMemoryAllocationResult.result), Core::ELogChannel::CLOG_RENDER);

		// Buffer memory binding
		ASSERT(device->bindBufferMemory(buffer, bufferMemory, 0) == vk::Result::eSuccess,
		       "Could not bind buffer memory. ", Core::ELogChannel::CLOG_RENDER);
	}

	VulkanBuffer::VulkanBuffer(VulkanBuffer&& other) noexcept
	{
		vulkanDevice = other.vulkanDevice;
		buffer = other.buffer;
		bufferMemory = other.bufferMemory;

		other.buffer = vk::Buffer{};
		other.bufferMemory = vk::DeviceMemory{};
	}

	VulkanBuffer::~VulkanBuffer()
	{
		if (buffer)
			(*vulkanDevice)->destroyBuffer(buffer);

		if (bufferMemory)
			(*vulkanDevice)->freeMemory(bufferMemory);
	}

	const vk::Buffer& VulkanBuffer::GetBuffer() const
	{
		return buffer;
	}

	const vk::DeviceMemory& VulkanBuffer::GetBufferMemory() const
	{
		return bufferMemory;
	}

	uint32_t VulkanBuffer::FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties,
	                                      vk::PhysicalDeviceMemoryProperties memProperties)
	{
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		ASSERT(false, "Failed to find suitable memory type. ", Core::ELogChannel::CLOG_RENDER);
		return 0;
	}
}
