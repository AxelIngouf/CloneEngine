#ifdef _DEBUG

#include "VulkanDebugMessenger.h"


#include "core/CLog.h"
#include "render/VulkanInstance/VulkanInstance.h"

namespace Render
{
	void VulkanDebugMessenger::Create(VulkanInstance& instance)
	{
		const auto createInfo = GenerateCreateInfo();
		const vk::Instance& inst = instance.Instance();

		messenger = inst.createDebugUtilsMessengerEXTUnique(createInfo, nullptr).value;
	}

	vk::DebugUtilsMessengerCreateInfoEXT VulkanDebugMessenger::GenerateCreateInfo()
	{
		vk::DebugUtilsMessengerCreateInfoEXT messengerInfo = {};

		const auto severity =
			//vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
			//vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;

		messengerInfo.setMessageSeverity(severity);
		messengerInfo.setMessageType(
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
			vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
			vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
		);

		messengerInfo.setPfnUserCallback(&MessengerCallback);
		messengerInfo.setPUserData(nullptr); // Custom pointer to pass to debug messenger

		return messengerInfo;
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugMessenger::MessengerCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* /*pUserData*/)
	{
		Core::ELogLevel logSeverity;
		Core::ELogChannel logChannel;

		switch (messageSeverity)
		{
		case static_cast<int>(vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo):
		case static_cast<int>(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose):
			logSeverity = Core::ELogLevel::CLOG_INFO;
			break;
		case static_cast<int>(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning):
			logSeverity = Core::ELogLevel::CLOG_WARNING;
			break;
		case static_cast<int>(vk::DebugUtilsMessageSeverityFlagBitsEXT::eError):
			logSeverity = Core::ELogLevel::CLOG_ERROR;
			break;
		default:
			logSeverity = Core::ELogLevel::CLOG_DEBUG;
			break;
		}

		switch (messageType)
		{
		case static_cast<int>(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral):
			logChannel = Core::ELogChannel::CLOG_VULKAN_GENERAL;
			break;
		case static_cast<int>(vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance):
			logChannel = Core::ELogChannel::CLOG_VULKAN_PERFORMANCE;
			break;
		case static_cast<int>(vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation):
			logChannel = Core::ELogChannel::CLOG_VULKAN_VALIDATION;
			break;
		default:
			logChannel = Core::ELogChannel::CLOG_RENDER;
		}

		std::string message = "Vulkan validation layer: ";
		message += pCallbackData->pMessage;

		LOG(logSeverity, message, logChannel);

		return VK_FALSE;
	}
}

#endif
