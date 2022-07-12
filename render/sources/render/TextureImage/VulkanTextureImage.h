#pragma once

#include "render/VulkanMacros.h"

namespace Model
{
	class Texture;
}

namespace Render
{
	class VulkanCommandPool;
	class VulkanDevice;

	class VulkanTextureImage
	{
	public:

		void Create(VulkanDevice& device, VulkanCommandPool& commandPool,
		            const Model::Texture& texture);

		void CreateCubemap(VulkanDevice& device, VulkanCommandPool& commandPool,
		                   const std::array<Model::Texture*, 6>& textures);

		void Create(VulkanDevice& device, VulkanCommandPool& commandPool, uint32_t width, uint32_t height);

		[[nodiscard]] const vk::ImageView& GetImageView() const
		{
			return textureImageView.get();
		}

	private:

		void CreateTextureImage(VulkanDevice& device, VulkanCommandPool& commandPool, const Model::Texture& texture);
		void CreateTextureImage(VulkanDevice& device, VulkanCommandPool& commandPool, uint32_t width, uint32_t height);
		void CreateImage(VulkanDevice& device, uint32_t width, uint32_t height, uint32_t layers, vk::Format format,
		                 vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties,
		                 vk::ImageCreateFlags flags = vk::ImageCreateFlags{});

		void TransitionImageLayout(VulkanCommandPool& commandPool, vk::Image image, uint32_t layerCount,
		                           vk::ImageLayout oldLayout, vk::ImageLayout newLayout) const;


		static void CopyBufferToImage(VulkanCommandPool& commandPool, vk::Buffer buf, vk::Image image,
		                              uint32_t width, uint32_t height, uint32_t layers = 1);

		void CreateTextureImageView(VulkanDevice& device);

		void CreateCubemapTextureImage(VulkanDevice& device, VulkanCommandPool& commandPool,
		                               const std::array<Model::Texture*, 6>& textures);
		void CreateCubemapTextureImageView(VulkanDevice& device, vk::Format format);

		vk::UniqueImage textureImage;
		vk::UniqueDeviceMemory textureImageMemory;

		vk::UniqueImageView textureImageView;
	};
}
