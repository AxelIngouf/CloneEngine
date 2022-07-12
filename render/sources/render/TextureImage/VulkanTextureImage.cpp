#include "VulkanTextureImage.h"


#include "render/VulkanDevice/VulkanDevice.h"

#include "core/ResourceManager.h"
#include "model/Texture.h"
#include "render/VulkanUtils.hpp"
#include "render/VulkanBuffer/VulkanBuffer.h"
#include "render/VulkanCommandPool/VulkanCommandPool.h"

namespace Render
{
	void VulkanTextureImage::Create(VulkanDevice& device, VulkanCommandPool& commandPool,
	                                const Model::Texture& texture)
	{
		CreateTextureImage(device, commandPool, texture);
		CreateTextureImageView(device);
	}

	void VulkanTextureImage::CreateCubemap(VulkanDevice& device, VulkanCommandPool& commandPool,
	                                       const std::array<Model::Texture*, 6>& textures)
	{
		CreateCubemapTextureImage(device, commandPool, textures);
		//todo make better
		CreateCubemapTextureImageView(device, vk::Format::eR8G8B8A8Srgb);
	}

	void VulkanTextureImage::Create(VulkanDevice& device, VulkanCommandPool& commandPool, const uint32_t width,
	                                const uint32_t height)
	{
		CreateTextureImage(device, commandPool, width, height);
		CreateTextureImageView(device);
	}

	void VulkanTextureImage::CreateTextureImage(VulkanDevice& device, VulkanCommandPool& commandPool,
	                                            const Model::Texture& texture)
	{
		const vk::DeviceSize imageSize =
			static_cast<vk::DeviceSize>(texture.width) * static_cast<vk::DeviceSize>(texture.height) * texture.channels;

		VulkanBuffer stagingBuffer;

		std::lock_guard<std::mutex> lock(commandPool.GetCommandPoolMutex());

		stagingBuffer.Initialize(device, imageSize, vk::BufferUsageFlagBits::eTransferSrc,
		                         vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);


		void* data = device->mapMemory(stagingBuffer.GetBufferMemory(), 0, imageSize).value;
		memcpy(data, texture.data.data(), static_cast<size_t>(imageSize));
		device->unmapMemory(stagingBuffer.GetBufferMemory());

		CreateImage(device, texture.width, texture.height, 1, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal,
		            vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
		            vk::MemoryPropertyFlagBits::eDeviceLocal);


		TransitionImageLayout(commandPool, textureImage.get(), 1, vk::ImageLayout::eUndefined,
		                      vk::ImageLayout::eTransferDstOptimal);

		CopyBufferToImage(commandPool, stagingBuffer.GetBuffer(), textureImage.get(),
		                  static_cast<uint32_t>(texture.width), static_cast<uint32_t>(texture.height));

		TransitionImageLayout(commandPool, textureImage.get(), 1,
		                      vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
	}

	void VulkanTextureImage::CreateTextureImage(VulkanDevice& device, VulkanCommandPool& commandPool,
	                                            const uint32_t width, const uint32_t height)
	{
		std::lock_guard<std::mutex> lock(commandPool.GetCommandPoolMutex());

		CreateImage(device, width, height, 1, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal,
		            vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
		            vk::MemoryPropertyFlagBits::eDeviceLocal);

		TransitionImageLayout(commandPool, textureImage.get(), 1, vk::ImageLayout::eUndefined,
		                      vk::ImageLayout::eTransferDstOptimal);

		TransitionImageLayout(commandPool, textureImage.get(), 1, vk::ImageLayout::eTransferDstOptimal,
		                      vk::ImageLayout::eShaderReadOnlyOptimal);
	}

	void VulkanTextureImage::CreateImage(VulkanDevice& device, const uint32_t width, const uint32_t height,
	                                     const uint32_t layers,
	                                     const vk::Format format, const vk::ImageTiling tiling,
	                                     const vk::ImageUsageFlags usage,
	                                     const vk::MemoryPropertyFlags properties, const vk::ImageCreateFlags flags)
	{
		vk::ImageCreateInfo imageInfo = {};
		imageInfo.imageType = vk::ImageType::e2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = layers;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = vk::ImageLayout::eUndefined;
		imageInfo.usage = usage;
		imageInfo.samples = vk::SampleCountFlagBits::e1;
		imageInfo.sharingMode = vk::SharingMode::eExclusive;
		imageInfo.flags = flags;

		// todo use vulkan allocator
		textureImage = device->createImageUnique(imageInfo).value;
		ASSERT(textureImage.get(), "Failed to create texture image. ", Core::ELogChannel::CLOG_RENDER);

		const vk::MemoryRequirements memRequirements = device->getImageMemoryRequirements(textureImage.get());

		vk::MemoryAllocateInfo allocInfo = {};
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = device.FindMemoryType(memRequirements.memoryTypeBits, properties);

		// todo use vulkan allocator
		textureImageMemory = device->allocateMemoryUnique(allocInfo).value;
		ASSERT(textureImageMemory.get(), "Failed to allocate image memory. ", Core::ELogChannel::CLOG_RENDER);


		vk::Result err = device->bindImageMemory(textureImage.get(), textureImageMemory.get(), 0);
		ASSERT(err == vk::Result::eSuccess, "Failed to bind image memory : " + vk::to_string(err),
		       Core::ELogChannel::CLOG_RENDER);
	}

	void VulkanTextureImage::TransitionImageLayout(VulkanCommandPool& commandPool, const vk::Image image,
	                                               const uint32_t layerCount,
	                                               const vk::ImageLayout oldLayout,
	                                               const vk::ImageLayout newLayout) const
	{
		auto commandBuffer = commandPool.BeginSingleTimeCommands();

		vk::ImageSubresourceRange subresourceRange;

		subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		subresourceRange.baseMipLevel = 0;
		subresourceRange.levelCount = 1;
		subresourceRange.baseArrayLayer = 0;
		subresourceRange.layerCount = layerCount;

		vk::PipelineStageFlags sourceStage;
		vk::PipelineStageFlags destinationStage;

		if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
		{
			sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
			destinationStage = vk::PipelineStageFlagBits::eTransfer;
		}
		else if (oldLayout == vk::ImageLayout::eTransferDstOptimal &&
			newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
		{
			sourceStage = vk::PipelineStageFlagBits::eTransfer;
			destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
		}
		else
		{
			LOG(LOG_ERROR,
			    Core::CLog::FormatString("Unsupported layout transition: from %s to %s ",
				    vk::to_string(oldLayout).c_str(), vk::to_string(newLayout).c_str()),
			    Core::ELogChannel::CLOG_RENDER);
		}


		VulkanUtils::SetImageLayout(commandBuffer, image, oldLayout, newLayout, subresourceRange,
		                            sourceStage, destinationStage);

		commandPool.EndSingleTimeCommands(commandBuffer);
	}

	void VulkanTextureImage::CopyBufferToImage(VulkanCommandPool& commandPool, const vk::Buffer buf,
	                                           const vk::Image image, const uint32_t width, const uint32_t height,
	                                           const uint32_t layers)
	{
		auto commandBuffer = commandPool.BeginSingleTimeCommands();

		vk::BufferImageCopy region;
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = layers;
		region.imageOffset = vk::Offset3D{0, 0, 0};
		region.imageExtent = vk::Extent3D{width, height, 1};

		commandBuffer.copyBufferToImage(buf, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

		commandPool.EndSingleTimeCommands(commandBuffer);
	}

	void VulkanTextureImage::CreateTextureImageView(VulkanDevice& device)
	{
		vk::ImageViewCreateInfo viewInfo{};
		viewInfo.image = textureImage.get();
		viewInfo.viewType = vk::ImageViewType::e2D;
		viewInfo.format = vk::Format::eR8G8B8A8Srgb;
		viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		textureImageView = device->createImageViewUnique(viewInfo).value;
		ASSERT(textureImageView.get(), "Failed to create texture image view. ", Core::ELogChannel::CLOG_RENDER);
	}

	void VulkanTextureImage::CreateCubemapTextureImage(VulkanDevice& device, VulkanCommandPool& commandPool,
	                                                   const std::array<Model::Texture*, 6>& textures)
	{
		const int width = textures[0]->width;
		const int height = textures[0]->height;
		const int numberOfChannels = textures[0]->channels;


		const VkDeviceSize layerSize = width * height * numberOfChannels;
		const VkDeviceSize imageSize = layerSize * 6;


		VulkanBuffer stagingBuffer;

		std::lock_guard<std::mutex> lock(commandPool.GetCommandPoolMutex());

		stagingBuffer.Initialize(device, imageSize, vk::BufferUsageFlagBits::eTransferSrc,
		                         vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);


		void* data = device->mapMemory(stagingBuffer.GetBufferMemory(), 0, imageSize).value;
		for (int i = 0; i < 6; i++)
		{
			memcpy((static_cast<char*>(data) + (layerSize * i)), textures[i]->data.data(),
			       static_cast<size_t>(layerSize));
		}
		device->unmapMemory(stagingBuffer.GetBufferMemory());

		vk::Format format;

		if (numberOfChannels == 3)
			format = vk::Format::eR8G8B8Srgb;
		else
			format = vk::Format::eR8G8B8A8Srgb;


		CreateImage(device, width, height, 6, format, vk::ImageTiling::eOptimal,
		            vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
		            vk::MemoryPropertyFlagBits::eDeviceLocal, vk::ImageCreateFlagBits::eCubeCompatible);


		TransitionImageLayout(commandPool, textureImage.get(), 6, vk::ImageLayout::eUndefined,
		                      vk::ImageLayout::eTransferDstOptimal);

		CopyBufferToImage(commandPool, stagingBuffer.GetBuffer(), textureImage.get(),
		                  static_cast<uint32_t>(width), static_cast<uint32_t>(height), 6);

		TransitionImageLayout(commandPool, textureImage.get(), 6, vk::ImageLayout::eTransferDstOptimal,
		                      vk::ImageLayout::eShaderReadOnlyOptimal);
	}

	void VulkanTextureImage::CreateCubemapTextureImageView(VulkanDevice& device, vk::Format format)
	{
		vk::ImageViewCreateInfo viewInfo{};
		viewInfo.image = textureImage.get();
		viewInfo.viewType = vk::ImageViewType::eCube;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 6;

		textureImageView = device->createImageViewUnique(viewInfo).value;
		ASSERT(textureImageView.get(), "Failed to create texture image view. ", Core::ELogChannel::CLOG_RENDER);
	}
}
