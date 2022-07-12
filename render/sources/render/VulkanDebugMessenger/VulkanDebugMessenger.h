#pragma once

#ifdef _DEBUG

#include "render/VulkanMacros.h"

namespace Render
{
	class VulkanDebugMessenger
	{
	public:
		void Create(class VulkanInstance& instance);


		static vk::DebugUtilsMessengerCreateInfoEXT GenerateCreateInfo();

		static VKAPI_ATTR VkBool32 VKAPI_CALL MessengerCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT*
			pCallbackData, void* pUserData);
	private:

		vk::UniqueDebugUtilsMessengerEXT messenger;
	};
}
#endif
