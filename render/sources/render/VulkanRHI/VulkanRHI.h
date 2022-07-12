#pragma once

namespace vk {
    class SurfaceKHR;
	class Instance;
}

namespace Render
{
    class VulkanInstance;
    struct CreationParams;

    class VulkanDevice;
	class VulkanDeviceList;

	class VulkanRHI
	{
	public:

		VulkanRHI() = default;
		~VulkanRHI();

		bool Initialize(CreationParams& instanceParams);

        [[nodiscard]] VulkanDevice* InitializeGraphicsDevice(vk::SurfaceKHR presentSurface) const;

        [[nodiscard]] vk::Instance GetInstance() const;

	private:

		VulkanInstance* instance;

		VulkanDeviceList* devices;
	};
}
