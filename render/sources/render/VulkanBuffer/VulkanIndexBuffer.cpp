#include "VulkanIndexBuffer.h"

#include "VulkanBuffer.h"
#include "core/CLog.h"
#include "core/PoolAllocator.h"
#include "render/VulkanCommandPool/VulkanCommandPool.h"
#include "render/VulkanDevice/VulkanDevice.h"

namespace Render
{
	VulkanIndexBuffer::VulkanIndexBuffer(VulkanDevice& device, VulkanCommandPool& commandPool,
	                                     std::vector<uint32_t>& indices)
	{
		buffer = Core::MemoryPool::Alloc<VulkanBuffer>();

		indexCount = static_cast<uint32_t>(indices.size());

		const vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

		VulkanBuffer stagingBuffer;
		stagingBuffer.Initialize(device, bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
		                         vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

		void* data;
		ASSERT(device->mapMemory(stagingBuffer.GetBufferMemory(), 0, bufferSize, vk::MemoryMapFlags{}, &data) ==
		       vk::Result:: eSuccess, "Failed to map vertex buffer memory. ", Core::ELogChannel::CLOG_RENDER);
		memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
		device->unmapMemory(stagingBuffer.GetBufferMemory());


		buffer->Initialize(device, bufferSize,
		                  vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
		                  vk::MemoryPropertyFlagBits::eDeviceLocal);


		commandPool.CopyBuffer(stagingBuffer, *buffer, bufferSize);
	}

    VulkanIndexBuffer::VulkanIndexBuffer(VulkanIndexBuffer&& other) noexcept:
        buffer(other.buffer), indexCount(other.indexCount)
    {
		other.buffer = nullptr;
    }

    VulkanIndexBuffer::~VulkanIndexBuffer()
    {
		if(buffer)
		    Core::MemoryPool::Free(buffer);
    }

    const vk::Buffer& VulkanIndexBuffer::GetBuffer() const
    {
        return buffer->GetBuffer();
    }
}
