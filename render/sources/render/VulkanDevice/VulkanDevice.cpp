#include "VulkanDevice.h"

#include "core/CLog.h"
#include "render/VulkanConstants.h"

#include <set>

#include "core/PoolAllocator.h"

namespace Render
{
	VulkanDevice::VulkanDevice(const vk::PhysicalDevice physDev) :
		physicalDevice(physDev)
	{
		queueMutex = Core::MemoryPool::Alloc<std::mutex>();
	}

	uint32_t VulkanDevice::GetScoreForPresentSurface(vk::SurfaceKHR presentSurface)
	{
		if (!(FetchGraphicsFeatures() &&
			FetchExtensionProperties() &&
			FetchQueueFamilies(presentSurface) &&
			FetchSwapchainSupportParameters(presentSurface)))
			return 0;

		FetchGraphicsProperties();
		FetchMemoryProperties();

		const vk::PhysicalDeviceProperties& props = GetProperties();

		LOG(LOG_INFO, Core::CLog::FormatString("Found physical device %s (%s | Vulkan v%i.%i.%i | Device #%u)",
			    props.deviceName.data(),
			    to_string(props.deviceType).c_str(),
			    VK_VERSION_MAJOR(props.apiVersion),
			    VK_VERSION_MINOR(props.apiVersion),
			    VK_VERSION_PATCH(props.apiVersion),
			    props.deviceID).c_str(), Core::ELogChannel::CLOG_RENDER);

		return ComputeGraphicsScore();
	}

	uint32_t VulkanDevice::ComputeGraphicsScore() const
	{
		uint32_t score = 0;

		if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
			score += 10000;

		if (HasUnifiedGraphicsAndPresentQueues())
			score += 1000;

		score += properties.limits.maxImageDimension2D;

		score += static_cast<uint32_t>(properties.limits.maxSamplerAnisotropy);
		score += properties.limits.maxColorAttachments;
		score += properties.limits.maxPushConstantsSize;
		score += properties.limits.maxSamplerAllocationCount;
		score += properties.limits.maxMemoryAllocationCount;
		score += properties.limits.maxBoundDescriptorSets;
		score += properties.limits.maxVertexInputBindings;
		score += properties.limits.maxViewports;

		return score;
	}

	uint32_t VulkanDevice::FindMemoryType(const uint32_t typeFilter,
	                                      const vk::MemoryPropertyFlags memoryProperties) const
	{
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
			if ((typeFilter & 1 << i) && (memProperties.memoryTypes[i].propertyFlags & memoryProperties) ==
				memoryProperties)
				return i;

		throw std::runtime_error("failed to find suitable memory type!");
	}

	vk::Format VulkanDevice::FindSupportedFormat(const std::vector<vk::Format>& candidates,
	                                             const vk::ImageTiling tiling,
	                                             const vk::FormatFeatureFlags formatFeatures) const
	{
		for (vk::Format format : candidates)
		{
			vk::FormatProperties props = physicalDevice.getFormatProperties(format);

			if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & formatFeatures) == formatFeatures)
			{
				return format;
			}
			if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & formatFeatures) == formatFeatures)
			{
				return format;
			}
		}

		ASSERT(false, "Could not find supported format. ", Core::ELogChannel::CLOG_RENDER);
		return vk::Format{};
	}

	bool VulkanDevice::FetchGraphicsFeatures()
	{
		features = physicalDevice.getFeatures();

		bool hasAllRequiredFeatures = (
			features.geometryShader &&
			features.tessellationShader &&
			features.multiDrawIndirect &&
			features.multiViewport &&
			features.depthClamp &&
			features.depthBiasClamp &&
			features.fillModeNonSolid &&
			features.samplerAnisotropy &&
			features.sampleRateShading);

		if constexpr (VulkanConstants::enableValidationLayers)
			hasAllRequiredFeatures &= static_cast<bool>(features.pipelineStatisticsQuery);

		return hasAllRequiredFeatures;
	}

	vk::PhysicalDeviceFeatures VulkanDevice::GetRequiredFeatures()
	{
		vk::PhysicalDeviceFeatures deviceFeatures{};

		deviceFeatures.geometryShader = true;
		deviceFeatures.tessellationShader = true;
		deviceFeatures.multiDrawIndirect = true;
		deviceFeatures.multiViewport = true;
		deviceFeatures.depthClamp = true;
		deviceFeatures.depthBiasClamp = true;
		deviceFeatures.fillModeNonSolid = true;
		deviceFeatures.samplerAnisotropy = true;
		deviceFeatures.sampleRateShading = true;


		if constexpr (VulkanConstants::enableValidationLayers)
			deviceFeatures.pipelineStatisticsQuery = true;

		return deviceFeatures;
	}

	void VulkanDevice::FetchGraphicsProperties()
	{
		properties = physicalDevice.getProperties();
	}

	void VulkanDevice::FetchMemoryProperties()
	{
		memProperties = physicalDevice.getMemoryProperties();
	}

	bool VulkanDevice::FetchQueueFamilies(vk::SurfaceKHR presentSurface)
	{
		queueFamilyProps = physicalDevice.getQueueFamilyProperties();

		for (int i = 0; i < queueFamilyProps.size(); i++)
		{
			if (queueFamilyProps[i].queueFlags & vk::QueueFlagBits::eGraphics)
				queueIndices.graphicsFamilyIdx = i;

			VkBool32 presentSupported = false;

			ASSERT(physicalDevice.getSurfaceSupportKHR(i, presentSurface, &presentSupported) == vk::Result::eSuccess,
			       "Error getting surface support. ", Core::ELogChannel::CLOG_RENDER);

			if (presentSupported)
				queueIndices.presentFamilyIdx = i;

			if (queueIndices.IsComplete())
				break;
		}

		return queueIndices.IsComplete();
	}

	bool VulkanDevice::FetchExtensionProperties()
	{
		extensionProperties = physicalDevice.enumerateDeviceExtensionProperties().value;

		for (auto& ext : VulkanConstants::deviceExtensions)
		{
			auto found = std::find_if(
				extensionProperties.begin(),
				extensionProperties.end(),
				[ext](auto& extProp) { return (strcmp(ext, extProp.extensionName) == 0); });

			if (found == extensionProperties.end())
				return false;
		}

		return true;
	}

	bool VulkanDevice::FetchSwapchainSupportParameters(vk::SurfaceKHR presentSurface)
	{
		swapchainSupportParams.surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(presentSurface).value;
		swapchainSupportParams.availableSurfaceFormats = physicalDevice.getSurfaceFormatsKHR(presentSurface).value;
		swapchainSupportParams.availablePresentModes = physicalDevice.getSurfacePresentModesKHR(presentSurface).value;

		return (!swapchainSupportParams.availableSurfaceFormats.empty() &&
			!swapchainSupportParams.availablePresentModes.empty());
	}

	bool VulkanDevice::HasUnifiedGraphicsAndPresentQueues() const
	{
		return queueIndices.IsComplete() &&
			queueIndices.graphicsFamilyIdx == queueIndices.presentFamilyIdx;
	}

	bool VulkanDevice::HasRequiredGraphicsQueueFamilies() const
	{
		return queueIndices.IsComplete();
	}

	bool VulkanDevice::CreateLogicalDevice()
	{
		ASSERT(HasRequiredGraphicsQueueFamilies(), "Device does not have the required queue families",
		       Core::ELogChannel::CLOG_RENDER);

		static const float queuePriority = 1.f;

		std::set<uint32_t> uniqueQueueFamilies = {
			queueIndices.graphicsFamilyIdx.value(),
			queueIndices.presentFamilyIdx.value()
		};

		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos(uniqueQueueFamilies.size());
		int i = 0;

		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			auto& queueCreateInfo = queueCreateInfos[i];
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			i++;
		}

		vk::DeviceCreateInfo deviceCreateInfo{};

		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

		vk::PhysicalDeviceFeatures deviceFeatures = GetRequiredFeatures();
		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(VulkanConstants::deviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = VulkanConstants::deviceExtensions.data();

		if constexpr (VulkanConstants::enableValidationLayers)
		{
			deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(VulkanConstants::validationLayers.size());
			deviceCreateInfo.ppEnabledLayerNames = VulkanConstants::validationLayers.data();
		}

		// todo use vulkan allocator
		logicalDevice = physicalDevice.createDeviceUnique(deviceCreateInfo).value;

		ASSERT(logicalDevice.get(), "Logical device creation failed. ", Core::ELogChannel::CLOG_RENDER);

		graphicsQueue = logicalDevice->getQueue(queueIndices.graphicsFamilyIdx.value(), 0);
		presentQueue = logicalDevice->getQueue(queueIndices.presentFamilyIdx.value(), 0);

		return true;
	}

	vk::UniqueImageView VulkanDevice::CreateImageView(vk::Image image, vk::Format format,
	                                                  vk::ImageAspectFlagBits aspectFlags, uint32_t mipLevels) const
	{
		vk::ImageViewCreateInfo createInfo{};
		createInfo.image = image;
		createInfo.viewType = vk::ImageViewType::e2D;
		createInfo.format = format;
		createInfo.subresourceRange.aspectMask = aspectFlags;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = mipLevels;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		// todo use vulkan allocator
		vk::UniqueImageView imageView = logicalDevice->createImageViewUnique(createInfo).value;
		ASSERT(imageView.get(), "Failed to create image view. ", Core::ELogChannel::CLOG_RENDER);

		return imageView;
	}

	vk::UniqueSampler VulkanDevice::CreateSampler(vk::Filter filter, vk::SamplerAddressMode addressMode,
	                                              bool anisotropyEnable) const
	{
		vk::SamplerCreateInfo samplerInfo = {};
		samplerInfo.magFilter = filter;
		samplerInfo.minFilter = filter;
		samplerInfo.addressModeU = addressMode;
		samplerInfo.addressModeV = addressMode;
		samplerInfo.addressModeW = addressMode;
		samplerInfo.anisotropyEnable = anisotropyEnable;
		samplerInfo.maxAnisotropy = physicalDevice.getProperties().limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = vk::CompareOp::eAlways;
		samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		// todo use vulkan allocator
		vk::UniqueSampler sampler = logicalDevice->createSamplerUnique(samplerInfo).value;
		ASSERT(sampler.get(), "Failed to create sampler. ", Core::ELogChannel::CLOG_RENDER);

		return sampler;
	}
}
