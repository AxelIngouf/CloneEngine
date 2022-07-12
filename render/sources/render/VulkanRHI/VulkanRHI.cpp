#include "VulkanRHI.h"

#include "core/CLog.h"
#include "core/PoolAllocator.h"
#include "render/VulkanDevice/VulkanDevice.h"
#include "render/VulkanInstance/VulkanInstance.h"
#include "../VulkanDevice/VulkanDeviceList.h"


VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace Render
{
    VulkanRHI::~VulkanRHI()
    {
		Core::MemoryPool::Free(instance);
		Core::MemoryPool::Free(devices);
    }

	bool VulkanRHI::Initialize(CreationParams& instanceParams)
	{
		instance = Core::MemoryPool::Alloc<VulkanInstance>();
		devices = Core::MemoryPool::Alloc<VulkanDeviceList>();

        const bool ok = instance->Initialize(instanceParams);

		ASSERT(ok, "Instance could not initialize. ", Core::ELogChannel::CLOG_RENDER);

		return ok;
	}

	VulkanDevice* VulkanRHI::InitializeGraphicsDevice(const vk::SurfaceKHR presentSurface) const
    {
		bool ok = devices->Initialize(instance->Instance());
		ASSERT(ok, "Error initializing device. ", Core::ELogChannel::CLOG_RENDER);

		VulkanDevice* graphicsDevice = devices->PickGraphicsDevice(presentSurface);
		ASSERT(graphicsDevice != nullptr, "Error picking graphics device. ", Core::ELogChannel::CLOG_RENDER);

		ok = graphicsDevice->CreateLogicalDevice();
		ASSERT(ok, "Error initializing logical device. ", Core::ELogChannel::CLOG_RENDER);

		instance->InitDynamicDispatcherThirdStep(static_cast<vk::Device>(*graphicsDevice));

		return graphicsDevice;
	}

    vk::Instance VulkanRHI::GetInstance() const
    {
        return instance->Instance();
    }
}
