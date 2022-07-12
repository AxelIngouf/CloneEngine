#pragma once
#include <vector>

namespace vk
{
	class Buffer;
}

namespace Render
{
    class VulkanBuffer;
    class VulkanDevice;

    class VulkanIndexBuffer
	{
	public:
		VulkanIndexBuffer() = delete;
		VulkanIndexBuffer(VulkanDevice& vulkanDevice, class VulkanCommandPool& commandPool,
		                  std::vector<uint32_t>& indices);
		VulkanIndexBuffer(VulkanIndexBuffer& other) = delete;
		VulkanIndexBuffer& operator=(const VulkanIndexBuffer& other) = delete;

        VulkanIndexBuffer(VulkanIndexBuffer&& other) noexcept;

		~VulkanIndexBuffer();

		[[nodiscard]] uint32_t GetIndexCount() const
		{
			return indexCount;
		}

        [[nodiscard]] const vk::Buffer& GetBuffer() const;

	private:

		VulkanBuffer* buffer;
		uint32_t indexCount = 0;
	};
}
