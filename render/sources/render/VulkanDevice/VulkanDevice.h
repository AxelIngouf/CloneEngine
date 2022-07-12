#pragma once

#include <optional>
#include <mutex>

#include "core/CLog.h"
#include "render/VulkanMacros.h"

namespace Render
{
	class VulkanDevice
	{
	public:
		VulkanDevice() = delete;
		VulkanDevice(VulkanDevice&& other) noexcept : physicalDevice(other.physicalDevice), queueMutex(other.queueMutex){}

		struct SwapchainSupportParameters
		{
			vk::SurfaceCapabilitiesKHR surfaceCapabilities;
			std::vector<vk::SurfaceFormatKHR> availableSurfaceFormats;
			std::vector<vk::PresentModeKHR> availablePresentModes;
		};

		explicit VulkanDevice(vk::PhysicalDevice physDev);

		[[nodiscard]] const vk::PhysicalDeviceProperties& GetProperties() const
		{
			return properties;
		}

		[[nodiscard]] const vk::PhysicalDeviceMemoryProperties& GetMemoryProperties() const
		{
			return memProperties;
		}

		[[nodiscard]] const vk::PhysicalDeviceFeatures& GetFeatures() const
		{
			return features;
		}

		[[nodiscard]] const SwapchainSupportParameters& GetSwapchainSupportParameters(const vk::SurfaceKHR surface)
		{
			FetchQueueFamilies(surface);
			FetchSwapchainSupportParameters(surface);

			return swapchainSupportParams;
		}

		uint32_t GetScoreForPresentSurface(vk::SurfaceKHR presentSurface);

		[[nodiscard]] bool HasUnifiedGraphicsAndPresentQueues() const;
		[[nodiscard]] bool HasRequiredGraphicsQueueFamilies() const;

		// [0] = graphics queue, [1] = present queue
		[[nodiscard]] std::array<uint32_t, 2> GetQueueFamilyIndicesArray() const
		{
			return {queueIndices.graphicsFamilyIdx.value(), queueIndices.presentFamilyIdx.value()};
		}

		bool CreateLogicalDevice();

		[[nodiscard]] vk::UniqueImageView CreateImageView(vk::Image image, vk::Format format,
		                                                  vk::ImageAspectFlagBits aspectFlags,
		                                                  uint32_t mipLevels) const;

		[[nodiscard]] vk::UniqueSampler CreateSampler(vk::Filter filter = vk::Filter::eLinear,
		                                              vk::SamplerAddressMode addressMode =
			                                              vk::SamplerAddressMode::eRepeat,
		                                              bool anisotropyEnable = true) const;

		const vk::Device* operator->() const
		{
			ASSERT(logicalDevice.get(), "No logical device found. ", Core::ELogChannel::CLOG_RENDER);

			return &logicalDevice.get();
		}

		vk::Device operator*() const
		{
			ASSERT(logicalDevice.get(), "No logical device found. ", Core::ELogChannel::CLOG_RENDER);

			return logicalDevice.get();
		}

		explicit operator vk::Device() const
		{
			return logicalDevice.get();
		}

		[[nodiscard]] vk::Queue GraphicsQueue() const
		{
			return graphicsQueue;
		}

		[[nodiscard]] vk::Queue PresentQueue() const
		{
			return presentQueue;
		}

		[[nodiscard]] vk::PhysicalDevice GetPhysicalDevice() const { return physicalDevice; }

		[[nodiscard]] uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags memoryProperties) const;

		[[nodiscard]] vk::Format FindSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling,
		                                             vk::FormatFeatureFlags formatFeatures) const;


		std::mutex* queueMutex;
	private:

		bool FetchGraphicsFeatures();
		static vk::PhysicalDeviceFeatures GetRequiredFeatures();
		void FetchGraphicsProperties();
		void FetchMemoryProperties();
		bool FetchQueueFamilies(vk::SurfaceKHR presentSurface);
		bool FetchExtensionProperties();
		bool FetchSwapchainSupportParameters(vk::SurfaceKHR presentSurface);

		[[nodiscard]] uint32_t ComputeGraphicsScore() const;

		using FamilyIndex = std::optional<uint32_t>;

		struct FamilyIndices
		{
			FamilyIndex graphicsFamilyIdx;
			FamilyIndex presentFamilyIdx;

			[[nodiscard]] bool IsComplete() const
			{
				return (
					graphicsFamilyIdx.has_value() &&
					presentFamilyIdx.has_value()
				);
			}
		};


		vk::PhysicalDevice physicalDevice;
		vk::UniqueDevice logicalDevice;

		vk::PhysicalDeviceProperties properties;
		std::vector<vk::ExtensionProperties> extensionProperties;
		vk::PhysicalDeviceFeatures features;
		std::vector<vk::QueueFamilyProperties> queueFamilyProps;
		vk::PhysicalDeviceMemoryProperties memProperties;

		SwapchainSupportParameters swapchainSupportParams{};

		FamilyIndices queueIndices;
		vk::Queue graphicsQueue;
		vk::Queue presentQueue;
	};
}
