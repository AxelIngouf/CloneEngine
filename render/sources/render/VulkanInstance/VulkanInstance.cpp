#include "VulkanInstance.h"

#include "core/CLog.h"
#include "render/VulkanConstants.h"
#include "render/VulkanDebugMessenger/VulkanDebugMessenger.h"

namespace Render
{
    const vk::Instance& VulkanInstance::Instance() const
    {
        return instance.get();
    }

    bool VulkanInstance::Initialize(CreationParams& instanceParams)
	{
		creationParams = instanceParams;

		InitDynamicDispatcherFirstStep();

		if constexpr (VulkanConstants::enableValidationLayers)
		{
			ASSERT(EnableValidationLayersSupport(), "Error enabling Vulkan validation layers. ",
			       Core::ELogChannel::CLOG_RENDER);
		}

		ASSERT(Create(), "Error creating Vulkan instance. ", Core::ELogChannel::CLOG_RENDER);


		ASSERT(CheckRequiredExtensionAvailability(creationParams.requiredExtensions),
		       "Error checking extension availability. ", Core::ELogChannel::CLOG_RENDER);

		InitDynamicDispatcherSecondStep();

		return true;
	}

	void VulkanInstance::InitDynamicDispatcherFirstStep()
	{
		vk::DynamicLoader dl;
		PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr =
			dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
		VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
	}

	void VulkanInstance::InitDynamicDispatcherSecondStep()
	{
		VULKAN_HPP_DEFAULT_DISPATCHER.init(instance.get());
	}

	void VulkanInstance::InitDynamicDispatcherThirdStep(vk::Device device)
	{
		VULKAN_HPP_DEFAULT_DISPATCHER.init(device);
	}

	bool VulkanInstance::EnableValidationLayersSupport()
	{
		layerProperties = vk::enumerateInstanceLayerProperties().value;

		bool allLayersSupported = CheckValidationLayersSupport();

		if (allLayersSupported)
			creationParams.requiredExtensions.AddExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		return allLayersSupported;
	}

	bool VulkanInstance::CheckValidationLayersSupport()
	{
		for (const auto& vl : VulkanConstants::validationLayers)
		{
			bool layerFound = false;

			for (const auto& prop : layerProperties)
			{
				if (strcmp(prop.layerName, vl) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				std::string str = "Missing validation layer: ";
				str += vl;
				LOG(LOG_ERROR, str, Core::ELogChannel::CLOG_RENDER);
				return false;
			}
		}

		return true;
	}

	bool VulkanInstance::Create()
	{
		vk::ApplicationInfo appInfo(
			creationParams.appName.data(),
			VK_MAKE_VERSION(1, 0, 0),
			creationParams.engineName.data(),
			VK_MAKE_VERSION(1, 0, 0),
			VK_API_VERSION_1_2
		);

		vk::InstanceCreateInfo createInfo(
			vk::InstanceCreateFlags(),
			&appInfo,
			0,
			nullptr,
			static_cast<uint32_t>(creationParams.requiredExtensions.Count()),
			creationParams.requiredExtensions.List()
		);

		vk::DebugUtilsMessengerCreateInfoEXT instanceCreationDebugMessengerInfo;
#ifdef _DEBUG
		if constexpr (VulkanConstants::enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(VulkanConstants::validationLayers.size());
			createInfo.ppEnabledLayerNames = VulkanConstants::validationLayers.data();

			instanceCreationDebugMessengerInfo = VulkanDebugMessenger::GenerateCreateInfo();
			createInfo.pNext = &instanceCreationDebugMessengerInfo;
		}
#endif

		instance = createInstanceUnique(createInfo, nullptr).value;

		ASSERT(instance.get(), "Instance could not be created. ", Core::ELogChannel::CLOG_RENDER);

		return true;
	}

	bool VulkanInstance::CheckRequiredExtensionAvailability(const ExtensionList& requiredExtensions)
	{
		RetrieveExtensionProperties();

		ExtensionList tmpExtList = requiredExtensions;

		for (const auto& prop : extensionProperties)
		{
			LOG(LOG_INFO, Core::CLog::FormatString("\t%s", prop.extensionName.data()).c_str(), Core::ELogChannel::CLOG_RENDER);

			if (tmpExtList.Count() != 0)
			{
				for (auto i = 0u; i < tmpExtList.Count(); i++)
				{
					if (strcmp(prop.extensionName, tmpExtList.List()[i]) == 0)
					{
						tmpExtList.RemoveExtensionAt(i);
						break;
					}
				}
			}
			else
				break;
		}

		const bool allRequirementsMet = tmpExtList.Count() == 0;
		if (allRequirementsMet)
		{
			LOG(LOG_INFO, "All required instance extensions were found.", Core::ELogChannel::CLOG_RENDER);
		}
		else
		{
			LOG(LOG_ERROR, "Required Vulkan extensions are missing.", Core::ELogChannel::CLOG_RENDER);
		}

		return allRequirementsMet;
	}

	void VulkanInstance::RetrieveExtensionProperties()
	{
		extensionProperties = vk::enumerateInstanceExtensionProperties().value;

		const std::string str("Found " + std::to_string(extensionProperties.size()) + " extensions");
		LOG(LOG_INFO, str, Core::ELogChannel::CLOG_RENDER);
	}
}
