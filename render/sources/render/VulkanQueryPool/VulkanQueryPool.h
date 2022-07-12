#pragma once

#include "../VulkanMacros.h"

namespace Render
{
	class VulkanDevice;

	class VulkanQueryPool
	{
	public:
		void SetupQueryPool(const VulkanDevice& device);
		void GetQueryResults(const VulkanDevice& device);

		[[nodiscard]] uint32_t GetQueryCount() const { return static_cast<uint32_t>(pipelineStats.size()); }
		[[nodiscard]] vk::QueryPool GetQueryPool() const { return queryPool.get(); }

	private:

		vk::UniqueQueryPool queryPool;
		std::vector<std::string> pipelineStatNames;
		std::vector<uint64_t> pipelineStats;
	};
}
