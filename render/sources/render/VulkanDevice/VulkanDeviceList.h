#pragma once
#include <vector>

#include "VulkanDevice.h"
#include "render/VulkanMacros.h"

namespace Render
{
	class VulkanDeviceList
	{
	public:

		VulkanDeviceList() = default;
		bool Initialize(const vk::Instance& instance);
		void PopulateDevices(const vk::Instance& instance);
		VulkanDevice* PickGraphicsDevice(vk::SurfaceKHR presentSurface);

	private:

		std::vector<VulkanDevice> devices;
		VulkanDevice* graphicsDevice = nullptr;
	};
}
