#include "VulkanVertexBuffer.h"

#include "VulkanBuffer.h"
#include "../../../../physic/sources/physic/PhysicsManager.h"
#include "core/PoolAllocator.h"
#include "render/VulkanCommandPool/VulkanCommandPool.h"
#include "render/VulkanDevice/VulkanDevice.h"
#include "render/Vertex/Vertex.h"

namespace Render
{
	VulkanVertexBuffer::VulkanVertexBuffer()
	{
		buffer = Core::MemoryPool::Alloc<VulkanBuffer>();
	}

	void VulkanVertexBuffer::Initialize(VulkanDevice& device, VulkanCommandPool& commandPool,
	                                    std::vector<Model::Vertex>& vertices)
	{
		vertexCount = (uint32_t)vertices.size();

		const vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		VulkanBuffer stagingBuffer;
		stagingBuffer.Initialize(device, bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
		                         vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

		void* data;
		ASSERT(device->mapMemory(stagingBuffer.GetBufferMemory(), 0, bufferSize, vk::MemoryMapFlags{}, &data) ==
		       vk::Result:: eSuccess, "Failed to map vertex buffer memory. ", Core::ELogChannel::CLOG_RENDER);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		device->unmapMemory(stagingBuffer.GetBufferMemory());


		buffer->Initialize(device, bufferSize,
		                   vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
		                   vk::MemoryPropertyFlagBits::eDeviceLocal);


		commandPool.CopyBuffer(stagingBuffer, *buffer, bufferSize);
	}

	void VulkanVertexBuffer::Initialize(VulkanDevice& device, VulkanCommandPool& commandPool,
	                                    std::vector<Physics::DebugVertex>& vertices)
	{
		if (vertices.empty())
		{
			vertexCount = 0;
			return;
		}

		vertexCount = (uint32_t)vertices.size();

		const vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		VulkanBuffer stagingBuffer;
		stagingBuffer.Initialize(device, bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
		                         vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

		void* data;
		ASSERT(device->mapMemory(stagingBuffer.GetBufferMemory(), 0, bufferSize, vk::MemoryMapFlags{}, &data) ==
		       vk::Result:: eSuccess, "Failed to map vertex buffer memory. ", Core::ELogChannel::CLOG_RENDER);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		device->unmapMemory(stagingBuffer.GetBufferMemory());


		buffer->Initialize(device, bufferSize,
		                   vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
		                   vk::MemoryPropertyFlagBits::eDeviceLocal);

		commandPool.CopyBuffer(stagingBuffer, *buffer, bufferSize, true);
	}

	void VulkanVertexBuffer::ClearBuffer()
	{
		vertexCount = 0;
		if (buffer)
		{
			Core::MemoryPool::Free(buffer);
			buffer = nullptr;
			buffer = Core::MemoryPool::Alloc<VulkanBuffer>();
		}
	}

	VulkanVertexBuffer::VulkanVertexBuffer(VulkanVertexBuffer&& other) noexcept:
		buffer(other.buffer), vertexCount(other.vertexCount)
	{
		other.buffer = nullptr;
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		if (buffer)
			Core::MemoryPool::Free(buffer);
	}

	const vk::Buffer& VulkanVertexBuffer::GetBuffer() const
	{
		return buffer->GetBuffer();
	}
}
