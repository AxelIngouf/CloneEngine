#pragma once

#include "core/CLog.h"

#include "VulkanSwapchainFrame.h"

namespace Render
{
	struct VulkanSwapchainImage;
	struct VulkanSwapchainFrame;
	class EngineWindow;
	class VulkanDevice;

	class VulkanSwapchain
	{
	public:

		bool Initialize(vk::Instance vulkanInstance, VulkanDevice& graphicsDevice, EngineWindow& engineWindow,
		                vk::SurfaceKHR presentSurface);

		void PrepareNewFrame();

		void SubmitCommandBuffers(const vk::CommandBuffer* commandBufferList, uint32_t listSize) const;

		[[nodiscard]] const vk::Extent2D& GetSwapchainImageExtent() const
		{
			return imagesExtent;
		}

		[[nodiscard]] const vk::Format& GetSwapchainImageFormat() const
		{
			return imagesFormat;
		}

		[[nodiscard]] uint32_t GetImagesInFlightCount() const
		{
			return static_cast<uint32_t>(imagesInFlight.size());
		}

		[[nodiscard]] uint32_t GetSwapchainImageCount() const
		{
			return imageCount;
		}

		[[nodiscard]] uint32_t GetImageInFlightIndex() const
		{
			return currentImageInFlightIdx;
		}

		[[nodiscard]] uint32_t GetFrameIndex() const
		{
			return currentFrameIdx;
		}

		[[nodiscard]] const vk::ImageView& GetColorAttachmentView(int imageIndex) const;

        [[nodiscard]] const vk::ImageView& GetDepthAttachmentView() const;

		[[nodiscard]] uint32_t GetSwapchainImagesKHR() const;

		void PresentFrame();
		void RecreateSwapchain(EngineWindow& window);

	private:

		vk::Result AcquireNextImage();

		[[nodiscard]] const vk::Semaphore* GetCurrentFramePresentCompleteSemaphore() const;

		[[nodiscard]] const vk::Semaphore* GetCurrentFrameRenderCompleteSemaphore() const;

		[[nodiscard]] vk::Fence GetCurrentFrameQueueSubmitFence() const;

        [[nodiscard]] const VulkanDevice& Device() const;

		bool InitializeFrameList(vk::Format imageFormat);

		vk::SwapchainCreateInfoKHR GenerateSwapchainCreateInfo(VulkanDevice& compatibleDevice,
		                                                       EngineWindow& window,
		                                                       vk::SurfaceKHR presentSurface);
		static vk::SurfaceFormatKHR PickSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
		static vk::PresentModeKHR PickPresentMode(const std::vector<vk::PresentModeKHR>& availableModes);
		static vk::Extent2D ComputeSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, EngineWindow& window);
		static uint32_t PickImageCount(const vk::SurfaceCapabilitiesKHR& capabilities);


		vk::UniqueSurfaceKHR surface;
		vk::UniqueSwapchainKHR swapchain;

		std::vector<VulkanSwapchainFrame> swapchainFrames;
		uint32_t currentFrameIdx = 0;

		std::vector<VulkanSwapchainImage> imagesInFlight;
		uint32_t currentImageInFlightIdx = 0;

		vk::Extent2D imagesExtent{};
		vk::Format imagesFormat{};

		vk::UniqueImage depthImage;
		vk::UniqueDeviceMemory depthImageMemory;
		vk::UniqueImageView depthImageView;

		uint32_t imageCount = 0;

		VulkanDevice* device = nullptr;
	};
}
