#pragma once

#include "render/VulkanMacros.h"

namespace Render
{
	class VulkanDevice;

	struct VulkanSwapchainImage
	{
		VulkanSwapchainImage(const VulkanDevice& device, vk::Image swapchainImage, vk::Format imageFormat);

		void AcquireFence(const VulkanDevice& device, vk::Fence newInFlightFence);

		vk::Image image;
		vk::UniqueImageView view;
		vk::Fence inFlightFence;
	};

	struct VulkanSwapchainFrame
	{
		VulkanSwapchainFrame(const VulkanDevice& device);
		bool WaitForSubmitFence(const VulkanDevice& device);

		vk::UniqueSemaphore presentCompleteSemaphore{};
		vk::UniqueSemaphore renderCompleteSemaphore{};
		vk::UniqueFence queueSubmitFence{};
	};
}
