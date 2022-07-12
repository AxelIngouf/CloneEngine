#pragma once

#include "../VulkanMacros.h"


namespace Render
{
	class VulkanDevice;

	class VulkanTextureSampler
	{
	public:
		void CreateSampler(VulkanDevice& device);

		[[nodiscard]] const vk::Sampler& GetSampler() const
		{
			return sampler.get();
		}

	private:
		vk::UniqueSampler sampler;
	};
}
