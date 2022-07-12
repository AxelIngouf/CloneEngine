#include "VulkanSwapchainFrame.h"

#include "render/VulkanDevice/VulkanDevice.h"

namespace Render
{
	VulkanSwapchainImage::VulkanSwapchainImage(const VulkanDevice& device, vk::Image swapchainImage,
	                                           vk::Format imageFormat)
	{
		image = swapchainImage;
		view = device.CreateImageView(image, imageFormat, vk::ImageAspectFlagBits::eColor, 1);
	}

	void VulkanSwapchainImage::AcquireFence(const VulkanDevice& device, vk::Fence newInFlightFence)
	{
		if (inFlightFence)
		{
			static const auto NO_TIMEOUT = UINT64_MAX;
			static const auto WAIT_ALL = VK_TRUE;
			ASSERT(device->waitForFences(1, &inFlightFence, WAIT_ALL, NO_TIMEOUT) == vk::Result::eSuccess,
			       "Error waiting for fence. ", Core::ELogChannel::CLOG_RENDER);
		}

		inFlightFence = newInFlightFence;
	}

	VulkanSwapchainFrame::VulkanSwapchainFrame(const VulkanDevice& device)
	{
		renderCompleteSemaphore = device->createSemaphoreUnique(vk::SemaphoreCreateInfo{}).value;
		presentCompleteSemaphore = device->createSemaphoreUnique(vk::SemaphoreCreateInfo{}).value;

		vk::FenceCreateInfo createInfo{};

		createInfo.flags = vk::FenceCreateFlagBits::eSignaled;
		queueSubmitFence = device->createFenceUnique(createInfo).value;
	}

	bool VulkanSwapchainFrame::WaitForSubmitFence(const VulkanDevice& device)
	{
		static const auto NO_TIMEOUT = UINT64_MAX;
		static const auto WAIT_ALL = VK_TRUE;

		vk::Fence* imageSubmitFence = &queueSubmitFence.get();
		ASSERT(device->waitForFences(1, imageSubmitFence, WAIT_ALL, NO_TIMEOUT) == vk::Result::eSuccess,
		       "Error waiting for fence. ", Core::ELogChannel::CLOG_RENDER);

		return true;
	}
}
