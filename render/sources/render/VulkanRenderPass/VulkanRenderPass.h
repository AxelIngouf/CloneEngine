#pragma once

#include "render/VulkanMacros.h"

namespace Render
{
	class VulkanDevice;

	class VulkanRenderPass
	{
	public:

		void CreateRenderPass(const VulkanDevice& device, const vk::Format& colorFormat);
		void CreateOffscreenRenderPass(const VulkanDevice& device, const vk::Format& colorFormat);
		void CreateDepthRenderPass(const VulkanDevice& device);


		[[nodiscard]] const vk::RenderPass& GetRenderPass() const { return renderPass.get(); }

	private:

		vk::UniqueRenderPass renderPass;
	};
}
