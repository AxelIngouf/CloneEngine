#include "VulkanQueryPool.h"

#include "core/DebugWindow/DebugWindow.h"
#include "render/VulkanDevice/VulkanDevice.h"

namespace Render
{
	void VulkanQueryPool::SetupQueryPool(const VulkanDevice& device)
	{
		pipelineStatNames = {
			"Input assembly vertex count",
			"Input assembly primitives count",
			"Vertex shader invocations",
			"Clipping stage primitives processed",
			"Clipping stage primitives output",
			"Fragment shader invocations"
		};

		vk::QueryPoolCreateInfo queryPoolInfo = {};
		queryPoolInfo.queryType = vk::QueryType::ePipelineStatistics;
		queryPoolInfo.queryCount = static_cast<uint32_t>(pipelineStatNames.size());
		queryPoolInfo.pipelineStatistics =
			vk::QueryPipelineStatisticFlagBits::eInputAssemblyVertices |
			vk::QueryPipelineStatisticFlagBits::eInputAssemblyPrimitives |
			vk::QueryPipelineStatisticFlagBits::eVertexShaderInvocations |
			vk::QueryPipelineStatisticFlagBits::eClippingInvocations |
			vk::QueryPipelineStatisticFlagBits::eClippingPrimitives |
			vk::QueryPipelineStatisticFlagBits::eFragmentShaderInvocations;

		pipelineStats.resize(pipelineStatNames.size());

		queryPool = device->createQueryPoolUnique(queryPoolInfo).value;
		ASSERT(queryPool.get(), "Failed to create query pool. ", Core::ELogChannel::CLOG_RENDER);


		for (size_t i = 0; i < pipelineStats.size(); i++)
			Core::DebugWindow::AddDebugValue({pipelineStatNames[i], &pipelineStats[i]}, "Pipeline Statistics");
	}

	void VulkanQueryPool::GetQueryResults(const VulkanDevice& device)
	{
		const uint32_t count = static_cast<uint32_t>(pipelineStats.size());

		std::vector<unsigned long long> temp = device->getQueryPoolResults<uint64_t>(
			queryPool.get(),
			0,
			1,
			count * sizeof(uint64_t),
			sizeof(uint64_t),
			vk::QueryResultFlagBits::e64).value;

		for (size_t i = 0; i < pipelineStats.size(); i++)
			pipelineStats[i] = temp[i];
	}
}
