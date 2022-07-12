#pragma once
#include <vector>

namespace Physics
{
	struct DebugVertex;
}

namespace Model
{
	struct Vertex;
}

namespace vk
{
	class Buffer;
}

namespace Render
{
	class VulkanCommandPool;
	class VulkanBuffer;
	class VulkanDevice;

	class VulkanVertexBuffer
	{
	public:
		VulkanVertexBuffer();

		void Initialize(VulkanDevice& device, VulkanCommandPool& commandPool,
		                std::vector<Model::Vertex>& vertices);
		void Initialize(VulkanDevice& device, VulkanCommandPool& commandPool,
		                std::vector<Physics::DebugVertex>& vertices);

		void ClearBuffer();

		VulkanVertexBuffer(VulkanVertexBuffer& other) = delete;
		VulkanVertexBuffer& operator=(const VulkanVertexBuffer& other) = delete;

		VulkanVertexBuffer(VulkanVertexBuffer&& other) noexcept;

		~VulkanVertexBuffer();

		[[nodiscard]] uint32_t GetVertexCount() const
		{
			return vertexCount;
		}

		[[nodiscard]] const vk::Buffer& GetBuffer() const;

	private:

		VulkanBuffer* buffer = nullptr;
		uint32_t vertexCount = 0;
	};
}
