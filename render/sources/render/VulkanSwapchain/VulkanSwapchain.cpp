#include "VulkanSwapchain.h"

#include "render/VulkanConstants.h"
#include "Core/CMath.h"
#include "render/EngineWindow/EngineWindow.h"
#include "render/VulkanDevice/VulkanDevice.h"

namespace Render
{
	bool VulkanSwapchain::Initialize(vk::Instance instance, VulkanDevice& graphicsDevice,
	                                 EngineWindow& window, const vk::SurfaceKHR presentSurface)
	{
		device = &graphicsDevice;

		surface = vk::UniqueSurfaceKHR(presentSurface, instance);

		ASSERT(surface.get(), "Surface unique handle creation failed", Core::ELogChannel::CLOG_RENDER);

		const vk::SwapchainCreateInfoKHR createInfo = GenerateSwapchainCreateInfo(
			graphicsDevice, window, presentSurface);

		swapchain = graphicsDevice->createSwapchainKHRUnique(createInfo).value;
		ASSERT(swapchain.get(), "Swapchain creation failed", Core::ELogChannel::CLOG_RENDER);


		vk::ImageCreateInfo imageInfo{};
		imageInfo.imageType = vk::ImageType::e2D;
		imageInfo.extent.width = createInfo.imageExtent.width;
		imageInfo.extent.height = createInfo.imageExtent.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = graphicsDevice.FindSupportedFormat(
			{vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
			vk::ImageTiling::eOptimal,
			vk::FormatFeatureFlagBits::eDepthStencilAttachment);
		imageInfo.tiling = vk::ImageTiling::eOptimal;
		imageInfo.initialLayout = vk::ImageLayout::eUndefined;
		imageInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
		imageInfo.samples = vk::SampleCountFlagBits::e1;
		imageInfo.sharingMode = vk::SharingMode::eExclusive;

		depthImage = graphicsDevice->createImageUnique(imageInfo).value;

		const vk::MemoryRequirements memRequirements = graphicsDevice->getImageMemoryRequirements(depthImage.get());

		vk::MemoryAllocateInfo allocInfo{};
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = graphicsDevice.FindMemoryType(memRequirements.memoryTypeBits,
		                                                          vk::MemoryPropertyFlagBits::eDeviceLocal);

		depthImageMemory = graphicsDevice->allocateMemoryUnique(allocInfo).value;
		ASSERT(graphicsDevice->bindImageMemory(depthImage.get(), depthImageMemory.get(), 0) == vk::Result::eSuccess,
		       "Failed to bind image memory. ", Core::ELogChannel::CLOG_RENDER);

		depthImageView = graphicsDevice.CreateImageView(depthImage.get(), imageInfo.format,
		                                                vk::ImageAspectFlagBits::eDepth, 1);

		ASSERT(InitializeFrameList(createInfo.imageFormat), "Frame list initialization failed",
		       Core::ELogChannel::CLOG_RENDER);

		imagesExtent = createInfo.imageExtent;
		imagesFormat = createInfo.imageFormat;

		return true;
	}

	void VulkanSwapchain::PrepareNewFrame()
	{
		swapchainFrames[currentFrameIdx].WaitForSubmitFence(*device);

		const vk::Result acquireResult = AcquireNextImage();

		switch (acquireResult)
		{
		case vk::Result::eSuccess:
			break;
		case vk::Result::eErrorOutOfDateKHR:
			LOG(LOG_WARNING, "Swapchain out of date. ", Core::ELogChannel::CLOG_RENDER);
			break;
		case vk::Result::eSuboptimalKHR:
			LOG(LOG_WARNING, "Presenting suboptimal swapchain. ", Core::ELogChannel::CLOG_RENDER);
			break;
		default:
			ASSERT(acquireResult == vk::Result::eSuccess, "AcquireNextImage unsuccessful. ",
			       Core::ELogChannel::CLOG_RENDER);
			break;
		}

		imagesInFlight[currentImageInFlightIdx].AcquireFence(*device, GetCurrentFrameQueueSubmitFence());
	}

	void VulkanSwapchain::SubmitCommandBuffers(const vk::CommandBuffer* commandBufferList,
	                                           const uint32_t listSize) const
	{
		ASSERT(commandBufferList != nullptr && listSize > 0, "Invalid command buffer list. ",
		       Core::ELogChannel::CLOG_RENDER);

		vk::SubmitInfo submitInfo{};

		submitInfo.pCommandBuffers = commandBufferList;
		submitInfo.commandBufferCount = listSize;

		submitInfo.pWaitSemaphores = GetCurrentFramePresentCompleteSemaphore();
		submitInfo.waitSemaphoreCount = 1;

		vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.pSignalSemaphores = GetCurrentFrameRenderCompleteSemaphore();
		submitInfo.signalSemaphoreCount = 1;

		vk::Fence inFlightImageFence = GetCurrentFrameQueueSubmitFence();
		ASSERT(Device()->resetFences(1, &inFlightImageFence) == vk::Result::eSuccess,
		       "Error resetting fence. ", Core::ELogChannel::CLOG_RENDER);


		std::lock_guard<std::mutex> lock(*Device().queueMutex);

		ASSERT(Device().GraphicsQueue().submit(1, &submitInfo, inFlightImageFence) == vk::Result::eSuccess,
		       "Error submitting graphics queue. ", Core::ELogChannel::CLOG_RENDER);
	}

	const vk::ImageView& VulkanSwapchain::GetColorAttachmentView(int imageIndex) const
	{
		ASSERT(imageIndex < imagesInFlight.size(), "Get color attachment view out of bounds. ",
		       Core::ELogChannel::CLOG_RENDER);
		return imagesInFlight[imageIndex].view.get();
	}

    const vk::ImageView& VulkanSwapchain::GetDepthAttachmentView() const
    {
        return depthImageView.get();
    }

	uint32_t VulkanSwapchain::GetSwapchainImagesKHR() const
	{
		const auto imagesKHR = Device()->getSwapchainImagesKHR(swapchain.get());

		ASSERT(imagesKHR.result == vk::Result::eSuccess, "Could not get swapchain images khr",
		       Core::ELogChannel::CLOG_RENDER);

		return static_cast<uint32_t>(imagesKHR.value.size());
	}

	void VulkanSwapchain::PresentFrame()
	{
		vk::PresentInfoKHR presentInfo{};

		presentInfo.pWaitSemaphores = GetCurrentFrameRenderCompleteSemaphore();
		presentInfo.waitSemaphoreCount = 1;

		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain.get();
		presentInfo.pImageIndices = &currentImageInFlightIdx;

		vk::Result result;
		std::lock_guard<std::mutex> queueLock(*device->queueMutex);
		{
			result = Device().PresentQueue().presentKHR(&presentInfo);
		}

		if (result != vk::Result::eSuccess)
		{
			LOG(LOG_WARNING, Core::CLog::FormatString("Error presenting khr: %s", vk::to_string(result).c_str()),
				Core::ELogChannel::CLOG_RENDER);
		}

		result = (*device)->waitIdle();

		if (result != vk::Result::eSuccess)
		{
			LOG(LOG_WARNING, Core::CLog::FormatString("Error waiting: %s", vk::to_string(result).c_str()),
			    Core::ELogChannel::CLOG_RENDER);
		}

		if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
		{
			LOG(LOG_WARNING, "Swapchain out of date. ", Core::ELogChannel::CLOG_RENDER);
			// todo recreate swapchain here
		}

		currentFrameIdx = (currentFrameIdx + 1) % VulkanConstants::maxFramesInFlight;
	}

	void VulkanSwapchain::RecreateSwapchain(EngineWindow& window)
	{
		const vk::SwapchainCreateInfoKHR createInfo = GenerateSwapchainCreateInfo(
			*device, window, *surface);

		swapchain = (*device)->createSwapchainKHRUnique(createInfo).value;
		ASSERT(swapchain.get(), "Swapchain creation failed", Core::ELogChannel::CLOG_RENDER);


		vk::ImageCreateInfo imageInfo{};
		imageInfo.imageType = vk::ImageType::e2D;
		imageInfo.extent.width = createInfo.imageExtent.width;
		imageInfo.extent.height = createInfo.imageExtent.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = device->FindSupportedFormat(
			{vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
			vk::ImageTiling::eOptimal,
			vk::FormatFeatureFlagBits::eDepthStencilAttachment);
		imageInfo.tiling = vk::ImageTiling::eOptimal;
		imageInfo.initialLayout = vk::ImageLayout::eUndefined;
		imageInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
		imageInfo.samples = vk::SampleCountFlagBits::e1;
		imageInfo.sharingMode = vk::SharingMode::eExclusive;

		depthImage = (*device)->createImageUnique(imageInfo).value;

		const vk::MemoryRequirements memRequirements = (*device)->getImageMemoryRequirements(depthImage.get());

		vk::MemoryAllocateInfo allocInfo{};
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = device->FindMemoryType(memRequirements.memoryTypeBits,
		                                                   vk::MemoryPropertyFlagBits::eDeviceLocal);

		depthImageMemory = (*device)->allocateMemoryUnique(allocInfo).value;
		ASSERT((*device)->bindImageMemory(depthImage.get(), depthImageMemory.get(), 0) == vk::Result::eSuccess,
		       "Failed to bind image memory. ", Core::ELogChannel::CLOG_RENDER);

		depthImageView = device->CreateImageView(depthImage.get(), imageInfo.format,
		                                         vk::ImageAspectFlagBits::eDepth, 1);

		ASSERT(InitializeFrameList(createInfo.imageFormat), "Frame list initialization failed",
		       Core::ELogChannel::CLOG_RENDER);

		imagesExtent = createInfo.imageExtent;
		imagesFormat = createInfo.imageFormat;
	}

	vk::Result VulkanSwapchain::AcquireNextImage()
	{
		static const auto NO_TIMEOUT = UINT64_MAX;

		return Device()->acquireNextImageKHR(swapchain.get(), NO_TIMEOUT,
		                                     *GetCurrentFramePresentCompleteSemaphore(), vk::Fence(),
		                                     &currentImageInFlightIdx);
	}

	const vk::Semaphore* VulkanSwapchain::GetCurrentFramePresentCompleteSemaphore() const
	{
		return &swapchainFrames[currentFrameIdx].presentCompleteSemaphore.get();
	}

	const vk::Semaphore* VulkanSwapchain::GetCurrentFrameRenderCompleteSemaphore() const
	{
		return &swapchainFrames[currentFrameIdx].renderCompleteSemaphore.get();
	}

	vk::Fence VulkanSwapchain::GetCurrentFrameQueueSubmitFence() const
	{
		return swapchainFrames[currentFrameIdx].queueSubmitFence.get();
	}

    const VulkanDevice& VulkanSwapchain::Device() const
    {
        ASSERT(device != nullptr, "Device does not exist. ", Core::ELogChannel::CLOG_RENDER);
        return *device;
    }

	bool VulkanSwapchain::InitializeFrameList(vk::Format imageFormat)
	{
		swapchainFrames.clear();
		swapchainFrames.reserve(imageCount);

		for (uint32_t i = 0; i < imageCount; i++)
			swapchainFrames.emplace_back(Device());

		auto newSwapchainImages = Device()->getSwapchainImagesKHR(swapchain.get()).value;
		ASSERT(!newSwapchainImages.empty(), "Failed to create swapchain images. ", Core::ELogChannel::CLOG_RENDER);

		imagesInFlight.clear();
		imagesInFlight.reserve(newSwapchainImages.size());
		for (auto scImage : newSwapchainImages)
		{
			imagesInFlight.emplace_back(Device(), scImage, imageFormat);
		}

		return true;
	}

	vk::SwapchainCreateInfoKHR VulkanSwapchain::GenerateSwapchainCreateInfo(
		VulkanDevice& compatibleDevice, EngineWindow& window, const vk::SurfaceKHR presentSurface)
	{
		// Swapchain image format
		const auto& swapchainParams = compatibleDevice.GetSwapchainSupportParameters(presentSurface);
		const vk::SurfaceFormatKHR surfaceFormat = PickSurfaceFormat(swapchainParams.availableSurfaceFormats);

		// Swapchain present mode (fifo/mailbox)
		const vk::PresentModeKHR presentMode = PickPresentMode(swapchainParams.availablePresentModes);

		// Calculate swapchain image size
		const vk::Extent2D swapExtent = ComputeSwapExtent(swapchainParams.surfaceCapabilities, window);

		// Pick number of swapchain images
		imageCount = PickImageCount(swapchainParams.surfaceCapabilities);

		vk::SwapchainCreateInfoKHR createInfo{};
		createInfo.surface = presentSurface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = swapExtent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

		auto queueFamilyIndices = compatibleDevice.GetQueueFamilyIndicesArray();
		if (compatibleDevice.HasUnifiedGraphicsAndPresentQueues())
		{
			createInfo.imageSharingMode = vk::SharingMode::eExclusive;
		}
		else
		{
			createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			createInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size());
			createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
		}

		createInfo.preTransform = swapchainParams.surfaceCapabilities.currentTransform;

		createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;

		createInfo.presentMode = presentMode;

		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = swapchain.get();

		return createInfo;
	}

	vk::SurfaceFormatKHR VulkanSwapchain::PickSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
	{
		ASSERT(!availableFormats.empty(), "No swapchain image formats found. ", Core::ELogChannel::CLOG_RENDER);

		for (const auto& format : availableFormats)
		{
			if (format.format == vk::Format::eB8G8R8A8Srgb &&
				format.colorSpace == vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear)
			{
				return format;
			}
		}

		return availableFormats[0];
	}

	vk::PresentModeKHR VulkanSwapchain::PickPresentMode(const std::vector<vk::PresentModeKHR>& availableModes)
	{
		ASSERT(!availableModes.empty(), "No swapchain present modes found. ", Core::ELogChannel::CLOG_RENDER);

		for (const auto& mode : availableModes)
		{
			if (mode == vk::PresentModeKHR::eMailbox)
			{
				return mode;
			}
		}

		return vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D VulkanSwapchain::ComputeSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities,
	                                                EngineWindow& window)
	{
		if (capabilities.currentExtent.width != UINT32_MAX)
			return capabilities.currentExtent;

		int width = 0;
		int height = 0;
		window.GetFramebufferSize(&width, &height);

		vk::Extent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = LibMath::Clamp(actualExtent.width,
		                                    capabilities.minImageExtent.width,
		                                    capabilities.maxImageExtent.width);
		actualExtent.height = LibMath::Clamp(actualExtent.height,
		                                     capabilities.minImageExtent.height,
		                                     capabilities.maxImageExtent.height);

		return actualExtent;
	}

	uint32_t VulkanSwapchain::PickImageCount(const vk::SurfaceCapabilitiesKHR& capabilities)
	{
		uint32_t imageCount = capabilities.minImageCount + 1;

		if (capabilities.maxImageCount > 0)
			imageCount = std::min(imageCount, capabilities.maxImageCount);

		return imageCount;
	}
}
