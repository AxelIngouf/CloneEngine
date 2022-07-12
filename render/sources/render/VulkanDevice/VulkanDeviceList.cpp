#include "VulkanDeviceList.h"

#include "core/CLog.h"

namespace Render
{
	bool VulkanDeviceList::Initialize(const vk::Instance& instance)
	{
		PopulateDevices(instance);

		return true;
	}

	void VulkanDeviceList::PopulateDevices(const vk::Instance& instance)
	{
		auto physicalDevices = instance.enumeratePhysicalDevices().value;

		ASSERT(!physicalDevices.empty(), "Could not find any physical devices. ", Core::ELogChannel::CLOG_RENDER);

		devices.reserve(physicalDevices.size());

		for (auto& pd : physicalDevices)
		{
			devices.emplace_back(pd);
		}
	}

	VulkanDevice* VulkanDeviceList::PickGraphicsDevice(vk::SurfaceKHR presentSurface)
	{
		VulkanDevice* bestCandidate = nullptr;
		uint32_t bestScore = 0;

		for (auto& device : devices)
		{
			const uint32_t score = device.GetScoreForPresentSurface(presentSurface);
			if (score > bestScore)
			{
				bestScore = score;
				bestCandidate = &device;
			}
		}

		ASSERT(bestCandidate != nullptr, "No suitable graphics device found. ", Core::ELogChannel::CLOG_RENDER);

		graphicsDevice = bestCandidate;
		LOG(LOG_INFO, Core::CLog::FormatString("Using %s as the graphics device.",
			    graphicsDevice->GetProperties().deviceName .data()).c_str(), Core::ELogChannel::CLOG_RENDER);

		return graphicsDevice;
	}
}
