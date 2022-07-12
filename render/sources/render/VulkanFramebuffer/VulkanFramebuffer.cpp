#include "VulkanFramebuffer.h"


#include "core/PoolAllocator.h"
#include "render/VulkanDevice/VulkanDevice.h"
#include "render/VulkanSwapchain/VulkanSwapchain.h"

namespace Render
{
	void VulkanFramebuffer::Initialize(const VulkanDevice& _device)
	{
		device = &_device;
	}

	void VulkanFramebuffer::InitializeOffscreenFramebuffer(const uint32_t width, const uint32_t height,
	                                                       const vk::RenderPass& renderPass)
	{
		attachmentImages.clear();
		attachmentMemory.clear();
		attachmentViews.clear();
		attachmentImages.resize(2);
		attachmentMemory.resize(2);
		attachmentViews.resize(2);

		{
			vk::ImageCreateInfo imageInfo = {};
			imageInfo.imageType = vk::ImageType::e2D;
			imageInfo.extent.width = width;
			imageInfo.extent.height = height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = vk::Format::eB8G8R8A8Srgb;
			imageInfo.tiling = vk::ImageTiling::eOptimal;
			imageInfo.initialLayout = vk::ImageLayout::eUndefined;
			imageInfo.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment;
			imageInfo.samples = vk::SampleCountFlagBits::e1;
			imageInfo.sharingMode = vk::SharingMode::eExclusive;

			// todo use vulkan allocator
			attachmentImages[0] = (*device)->createImageUnique(imageInfo).value;
			ASSERT(attachmentImages[0].get(), "Failed to create offscreen image. ", Core::ELogChannel::CLOG_RENDER);

			const vk::MemoryRequirements memRequirements = (*device)->getImageMemoryRequirements(
				attachmentImages[0].get());

			vk::MemoryAllocateInfo allocInfo = {};
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = device->FindMemoryType(memRequirements.memoryTypeBits,
			                                                   vk::MemoryPropertyFlagBits::eDeviceLocal);

			// todo use vulkan allocator
			attachmentMemory[0] = (*device)->allocateMemoryUnique(allocInfo).value;
			ASSERT(attachmentMemory[0].get(), "Failed to allocate offscreen image memory. ",
			       Core::ELogChannel::CLOG_RENDER);


			vk::Result err = (*device)->bindImageMemory(attachmentImages[0].get(), attachmentMemory[0].get(), 0);
			ASSERT(err == vk::Result::eSuccess,
			       "Failed to bind offscreen image memory : " + std::to_string(static_cast<int>(err)),
			       Core::ELogChannel::CLOG_RENDER);


			attachmentViews[0] = device->CreateImageView(attachmentImages[0].get(), imageInfo.format,
			                                             vk::ImageAspectFlagBits::eColor, 1);
		}

		ASSERT(attachmentViews[0].get(), "Failed to create color image view. ", Core::ELogChannel::CLOG_RENDER);

		{
			vk::ImageCreateInfo imageInfo{};
			imageInfo.imageType = vk::ImageType::e2D;
			imageInfo.extent.width = width;
			imageInfo.extent.height = height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = device->FindSupportedFormat(
				{vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
				vk::ImageTiling::eOptimal,
				vk::FormatFeatureFlagBits::eDepthStencilAttachment);
			imageInfo.tiling = vk::ImageTiling::eOptimal;
			imageInfo.initialLayout = vk::ImageLayout::eUndefined;
			imageInfo.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eDepthStencilAttachment;
			imageInfo.samples = vk::SampleCountFlagBits::e1;
			imageInfo.sharingMode = vk::SharingMode::eExclusive;

			attachmentImages[1] = (*device)->createImageUnique(imageInfo).value;

			const vk::MemoryRequirements memRequirements = (*device)->getImageMemoryRequirements(
				attachmentImages[1].get());

			vk::MemoryAllocateInfo allocInfo{};
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = device->FindMemoryType(memRequirements.memoryTypeBits,
			                                                   vk::MemoryPropertyFlagBits::eDeviceLocal);

			attachmentMemory[1] = (*device)->allocateMemoryUnique(allocInfo).value;
			ASSERT(
				(*device)->bindImageMemory(attachmentImages[1].get(), attachmentMemory[1].get(), 0) ==
				vk ::Result::eSuccess, "Failed to bind image memory. ", Core::ELogChannel::CLOG_RENDER);

			attachmentViews[1] = device->CreateImageView(attachmentImages[1].get(), imageInfo.format,
			                                             vk::ImageAspectFlagBits::eDepth, 1);
		}

		ASSERT(attachmentViews[1].get(), "Failed to create depth image view. ", Core::ELogChannel::CLOG_RENDER);

		std::vector<vk::ImageView> attachments = {attachmentViews[0].get(), attachmentViews[1].get()};

		vk::FramebufferCreateInfo fbInfo = {};
		fbInfo.renderPass = renderPass;
		fbInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		fbInfo.pAttachments = attachments.data();
		fbInfo.width = width;
		fbInfo.height = height;
		fbInfo.layers = 1;

		framebuffer = (*device)->createFramebufferUnique(fbInfo).value;
	}

	void VulkanFramebuffer::InitializeDepthFramebuffer(uint32_t width, uint32_t height,
	                                                   const vk::RenderPass& renderPass)
	{
		attachmentImages.clear();
		attachmentMemory.clear();
		attachmentViews.clear();
		attachmentImages.resize(1);
		attachmentMemory.resize(1);
		attachmentViews.resize(1);

		// For shadow mapping we only need a depth attachment
		vk::ImageCreateInfo image = {};
		image.imageType = vk::ImageType::e2D;
		image.extent.width = width;
		image.extent.height = height;
		image.extent.depth = 1;
		image.mipLevels = 1;
		image.arrayLayers = 1;
		image.samples = vk::SampleCountFlagBits::e1;
		image.tiling = vk::ImageTiling::eOptimal;
		image.format = vk::Format::eD32Sfloat; // Depth stencil attachment
		image.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled;
		// We will sample directly from the depth attachment for the shadow mapping

		attachmentImages[0] = (*device)->createImageUnique(image).value;

		vk::MemoryAllocateInfo memAlloc = {};
		vk::MemoryRequirements memReqs;
		memReqs = (*device)->getImageMemoryRequirements(attachmentImages[0].get());
		memAlloc.allocationSize = memReqs.size;
		memAlloc.memoryTypeIndex = device->FindMemoryType(memReqs.memoryTypeBits,
		                                                  vk::MemoryPropertyFlagBits::eDeviceLocal);

		attachmentMemory[0] = (*device)->allocateMemoryUnique(memAlloc).value;
		ASSERT(
			(*device)->bindImageMemory(attachmentImages[0].get(), attachmentMemory[0].get(), 0) ==
			vk::Result:: eSuccess, "Failed to bind image memory. ", Core::ELogChannel::CLOG_RENDER);


		vk::ImageViewCreateInfo depthStencilView = {};
		depthStencilView.viewType = vk::ImageViewType::e2D;
		depthStencilView.format = vk::Format::eD32Sfloat;
		depthStencilView.subresourceRange = vk::ImageSubresourceRange{};
		depthStencilView.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
		depthStencilView.subresourceRange.baseMipLevel = 0;
		depthStencilView.subresourceRange.levelCount = 1;
		depthStencilView.subresourceRange.baseArrayLayer = 0;
		depthStencilView.subresourceRange.layerCount = 1;
		depthStencilView.image = attachmentImages[0].get();

		attachmentViews[0] = (*device)->createImageViewUnique(depthStencilView).value;

		// Create sampler to sample from to depth attachment
		// Used to sample in the fragment shader for shadowed rendering
		vk::Filter shadowmapFilter = vk::Filter::eLinear;
		vk::SamplerCreateInfo sampler = {};
		sampler.magFilter = shadowmapFilter;
		sampler.minFilter = shadowmapFilter;
		sampler.mipmapMode = vk::SamplerMipmapMode::eLinear;
		sampler.addressModeU = vk::SamplerAddressMode::eClampToEdge;
		sampler.addressModeV = sampler.addressModeU;
		sampler.addressModeW = sampler.addressModeU;
		sampler.mipLodBias = 0.0f;
		sampler.maxAnisotropy = 1.0f;
		sampler.minLod = 0.0f;
		sampler.maxLod = 1.0f;
		sampler.borderColor = vk::BorderColor::eFloatOpaqueBlack;

		depthSampler = Core::MemoryPool::Alloc<vk::UniqueSampler>();

		*depthSampler = (*device)->createSamplerUnique(sampler).value;


		// Create frame buffer
		vk::FramebufferCreateInfo fbufCreateInfo = {};
		fbufCreateInfo.renderPass = renderPass;
		fbufCreateInfo.attachmentCount = 1;
		fbufCreateInfo.pAttachments = &attachmentViews[0].get();
		fbufCreateInfo.width = width;
		fbufCreateInfo.height = height;
		fbufCreateInfo.layers = 1;

		framebuffer = (*device)->createFramebufferUnique(fbufCreateInfo).value;
	}

	void VulkanFramebuffer::InitializeSwapchainFramebuffer(const int idx, const VulkanSwapchain& swapchain,
	                                                       const vk::RenderPass& renderPass)
	{
		vk::FramebufferCreateInfo fbInfo = {};

		std::vector<vk::ImageView> attachments{
			swapchain.GetColorAttachmentView(idx),
			swapchain.GetDepthAttachmentView()
		};


		fbInfo.renderPass = renderPass;
		fbInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		fbInfo.pAttachments = attachments.data();
		fbInfo.width = swapchain.GetSwapchainImageExtent().width;
		fbInfo.height = swapchain.GetSwapchainImageExtent().height;
		fbInfo.layers = 1;

		framebuffer = (*device)->createFramebufferUnique(fbInfo).value;
	}
}
