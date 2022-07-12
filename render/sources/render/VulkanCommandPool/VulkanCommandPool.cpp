#include "VulkanCommandPool.h"


#include "../../../../physic/sources/physic/PhysicsManager.h"
#include "core/PoolAllocator.h"
#include "core/DebugWindow/DebugWindow.h"
#include "core/scenegraph/SceneGraph.h"
#include "render/VulkanSwapchain/VulkanSwapchain.h"


#include "render/VulkanDevice/VulkanDevice.h"
#include "render/VulkanPipeline/VulkanPipeline.h"

#include "imgui/EditorUI.h"
#include "imgui/ImguiImpl.h"
#include "render/VulkanConstants.h"
#include "render/RenderComponent/ModelComponent.h"
#include "render/VulkanBuffer/VulkanBuffer.h"
#include "render/VulkanQueryPool/VulkanQueryPool.h"
#include "model/Vertex.h"
#include "render/VulkanBuffer/VulkanVertexBuffer.h"

namespace Render
{
	void VulkanCommandPool::ResetCommandPool()
	{
		(*device)->resetCommandPool(commandPool.get(), vk::CommandPoolResetFlags{});
	}

	void VulkanCommandPool::CreateCommandPool(VulkanDevice& graphicsDevice)
	{
		device = &graphicsDevice;

		vk::CommandPoolCreateInfo poolInfo = {};
		poolInfo.queueFamilyIndex = device->GetQueueFamilyIndicesArray()[0]; // Graphics queue
		poolInfo.flags = vk::CommandPoolCreateFlags{}; // Optional

		commandPool = (*device)->createCommandPoolUnique(poolInfo).value;

		ASSERT(commandPool.get(), "Failed to create command pool. ", Core::ELogChannel::CLOG_RENDER);


		std::vector<Model::Vertex> skyboxVertices =
		{

			{{-1.0f, 1.0f, -1.0f}, {}, {}},
			{{-1.0f, -1.0f, -1.0f}, {}, {}},
			{{1.0f, -1.0f, -1.0f}, {}, {}},
			{{1.0f, -1.0f, -1.0f}, {}, {}},
			{{1.0f, 1.0f, -1.0f}, {}, {}},
			{{-1.0f, 1.0f, -1.0f}, {}, {}},

			{{-1.0f, -1.0f, 1.0f}, {}, {}},
			{{-1.0f, -1.0f, -1.0f}, {}, {}},
			{{-1.0f, 1.0f, -1.0f}, {}, {}},
			{{-1.0f, 1.0f, -1.0f}, {}, {}},
			{{-1.0f, 1.0f, 1.0f}, {}, {}},
			{{-1.0f, -1.0f, 1.0f}, {}, {}},

			{{1.0f, -1.0f, -1.0f}, {}, {}},
			{{1.0f, -1.0f, 1.0f}, {}, {}},
			{{1.0f, 1.0f, 1.0f}, {}, {}},
			{{1.0f, 1.0f, 1.0f}, {}, {}},
			{{1.0f, 1.0f, -1.0f}, {}, {}},
			{{1.0f, -1.0f, -1.0f}, {}, {}},

			{{-1.0f, -1.0f, 1.0f}, {}, {}},
			{{-1.0f, 1.0f, 1.0f}, {}, {}},
			{{1.0f, 1.0f, 1.0f}, {}, {}},
			{{1.0f, 1.0f, 1.0f}, {}, {}},
			{{1.0f, -1.0f, 1.0f}, {}, {}},
			{{-1.0f, -1.0f, 1.0f}, {}, {}},

			{{-1.0f, 1.0f, -1.0f}, {}, {}},
			{{1.0f, 1.0f, -1.0f}, {}, {}},
			{{1.0f, 1.0f, 1.0f}, {}, {}},
			{{1.0f, 1.0f, 1.0f}, {}, {}},
			{{-1.0f, 1.0f, 1.0f}, {}, {}},
			{{-1.0f, 1.0f, -1.0f}, {}, {}},

			{{-1.0f, -1.0f, -1.0f}, {}, {}},
			{{-1.0f, -1.0f, 1.0f}, {}, {}},
			{{1.0f, -1.0f, -1.0f}, {}, {}},
			{{1.0f, -1.0f, -1.0f}, {}, {}},
			{{-1.0f, -1.0f, 1.0f}, {}, {}},
			{{1.0f, -1.0f, 1.0f}, {}, {}}
		};

		cubemapVertices = Core::MemoryPool::Alloc<VulkanVertexBuffer>();
		cubemapVertices->Initialize(*device, *this, skyboxVertices);
	}

	void VulkanCommandPool::CreateCommandBuffer()
	{
		vk::CommandBufferAllocateInfo allocInfo = {};
		allocInfo.commandPool = commandPool.get();
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandBufferCount = 1;

		commandBuffer = (*device)->allocateCommandBuffers(allocInfo).value[0];

		ASSERT(commandBuffer, "Failed to create command buffer. ", Core::ELogChannel::CLOG_RENDER);
	}

	vk::CommandBuffer VulkanCommandPool::BeginSingleTimeCommands()
	{
		vk::CommandBufferAllocateInfo allocInfo{};
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandPool = commandPool.get();
		allocInfo.commandBufferCount = 1;

		const vk::CommandBuffer buffer = (*device)->allocateCommandBuffers(allocInfo).value[0];
		ASSERT(buffer, "Failed to create command buffer. ", Core::ELogChannel::CLOG_RENDER);

		vk::CommandBufferBeginInfo beginInfo{};
		beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

		ASSERT(buffer.begin(&beginInfo) == vk::Result::eSuccess,
		       "Failed to begin recording command buffer. ", Core::ELogChannel::CLOG_RENDER);

		return buffer;
	}

	void VulkanCommandPool::EndSingleTimeCommands(vk::CommandBuffer& buffer)
	{
		ASSERT(buffer.end() == vk::Result::eSuccess,
		       "Failed to record command buffer. ", Core::ELogChannel::CLOG_RENDER);

		vk::SubmitInfo submitInfo{};
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &buffer;


		std::lock_guard<std::mutex> lock(*device->queueMutex);
		{
			ASSERT(device->GraphicsQueue().submit(1, &submitInfo, nullptr) == vk::Result::eSuccess,
			       "Failed to submit command buffer. ", Core::ELogChannel::CLOG_RENDER);

			ASSERT(device->GraphicsQueue().waitIdle() == vk::Result::eSuccess,
			       "Failed to wait on command buffer. ", Core::ELogChannel::CLOG_RENDER);
		}

		(*device)->freeCommandBuffers(commandPool.get(), 1, &buffer);
	}

	void VulkanCommandPool::RecordCommandBuffer(const vk::Framebuffer& framebuffer,
	                                            const vk::Framebuffer& offscreenFramebuffer,
	                                            const vk::Framebuffer& depthFramebuffer,
	                                            const VulkanSwapchain& swapchain,
	                                            VulkanPipeline& pipeline,
	                                            const vk::DescriptorSet frameDescriptor,
	                                            const vk::DescriptorSet depthDescriptor,
	                                            const vk::DescriptorSet lightDescriptor,
	                                            const vk::Extent2D& viewportSize,
	                                            VulkanVertexBuffer& debugLines,
	                                            const VulkanQueryPool& queryPool)
	{
		vk::CommandBufferBeginInfo beginInfo = {};
		beginInfo.flags = vk::CommandBufferUsageFlags{};
		beginInfo.pInheritanceInfo = nullptr;

		ASSERT(commandBuffer.begin(&beginInfo) == vk::Result::eSuccess,
		       "Failed to begin recording command buffer. ", Core::ELogChannel::CLOG_RENDER);

		if constexpr (VulkanConstants::enableValidationLayers)
			commandBuffer.resetQueryPool(queryPool.GetQueryPool(), 0, queryPool.GetQueryCount());

		/*
		 * First render pass (shadow map)
		 */
		std::array<vk::ClearValue, 2> clearValues{};
		clearValues[0].color = std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f};
		clearValues[1].depthStencil = vk::ClearDepthStencilValue{1.0f, 0};


		{
			vk::RenderPassBeginInfo renderPassBeginInfo = {};
			renderPassBeginInfo.renderPass = pipeline.GetRenderPass(VulkanPipeline::RenderPassStage::DEPTH);
			renderPassBeginInfo.framebuffer = depthFramebuffer;
			renderPassBeginInfo.renderArea.extent.width = VulkanConstants::shadowMapSize;
			renderPassBeginInfo.renderArea.extent.height = VulkanConstants::shadowMapSize;
			renderPassBeginInfo.clearValueCount = 1;
			renderPassBeginInfo.pClearValues = &clearValues[1];

			commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

			vk::Viewport viewport = {};
			viewport.width = VulkanConstants::shadowMapSize;
			viewport.height = VulkanConstants::shadowMapSize;
			viewport.minDepth = 0.f;
			viewport.maxDepth = 1.f;
			commandBuffer.setViewport(0, 1, &viewport);

			vk::Rect2D scissor = {};
			scissor.extent.width = VulkanConstants::shadowMapSize;
			scissor.extent.height = VulkanConstants::shadowMapSize;
			commandBuffer.setScissor(0, 1, &scissor);

			commandBuffer.setDepthBias(VulkanConstants::depthBiasConstant, 0.0f, VulkanConstants::depthBiasSlope);


			commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
			                           pipeline.GetPipeline(VulkanPipeline::PipelineStage::DEPTH));

			commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
			                                 pipeline.GetLayout(VulkanPipeline::PipelineStage::DEPTH), 0, 1,
			                                 &depthDescriptor, 0, nullptr);

			START_BENCHMARK("Draw Shadow Map");
			ModelComponent::Iterator it = ModelComponent::GetAll();
			while (it.Next())
			{
				it->DrawUntextured(commandBuffer, pipeline.GetLayout(VulkanPipeline::PipelineStage::DEPTH));
			}
			STOP_BENCHMARK("Draw Shadow Map");


			commandBuffer.endRenderPass();
		}


		/*
		 * Second render pass (scene)
		 */

		vk::RenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.renderPass = pipeline.GetRenderPass(VulkanPipeline::RenderPassStage::OFFSCREEN);
		renderPassInfo.framebuffer = offscreenFramebuffer;
		renderPassInfo.renderArea.offset = vk::Offset2D{0, 0};
		renderPassInfo.renderArea.extent = viewportSize;

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		VulkanPipeline::SwapAndUpdateViewportAndScissor(commandBuffer, pipeline.GetOffscreenViewport(),
		                                                pipeline.GetOffscreenScissor(), 0, 0, viewportSize);

		commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

		if constexpr (VulkanConstants::enableValidationLayers)
			commandBuffer.beginQuery(queryPool.GetQueryPool(), 0, vk::QueryControlFlags{});

		START_BENCHMARK("Bind pipeline/descriptors");


		// Skybox
		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
		                           pipeline.GetPipeline(VulkanPipeline::PipelineStage::SKYBOX));

		commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
		                                 pipeline.GetLayout(VulkanPipeline::PipelineStage::SKYBOX), 0, 1,
		                                 &frameDescriptor, 0, nullptr);

		vk::DeviceSize offset[] = {0};
		commandBuffer.bindVertexBuffers(0, 1, &cubemapVertices->GetBuffer(), offset);
		commandBuffer.draw(cubemapVertices->GetVertexCount(), 1, 0, 0);

		// Standard
		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
		                           pipeline.GetPipeline(VulkanPipeline::PipelineStage::STANDARD));

		commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
		                                 pipeline.GetLayout(VulkanPipeline::PipelineStage::STANDARD), 0, 1,
		                                 &frameDescriptor, 0, nullptr);

		commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
		                                 pipeline.GetLayout(VulkanPipeline::PipelineStage::STANDARD), 1, 1,
		                                 &lightDescriptor, 0, nullptr);
		STOP_BENCHMARK("Bind pipeline/descriptors");

		START_BENCHMARK("Draw");
		ModelComponent::Iterator it = ModelComponent::GetAll();
		std::string previousMaterialName = "";
		while (it.Next())
		{
			it->Draw(commandBuffer, pipeline.GetLayout(VulkanPipeline::PipelineStage::STANDARD),
			         swapchain.GetFrameIndex(), previousMaterialName);
		}
		STOP_BENCHMARK("Draw");

		START_BENCHMARK("Physics Debug");

		if (debugLines.GetVertexCount() > 0)
		{
			commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
			                           pipeline.GetPipeline(VulkanPipeline::PipelineStage::PHYSICS_DEBUG));

			commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
			                                 pipeline.GetLayout(VulkanPipeline::PipelineStage::PHYSICS_DEBUG), 0, 1,
			                                 &frameDescriptor, 0, nullptr);

			commandBuffer.bindVertexBuffers(0, 1, &debugLines.GetBuffer(), offset);

			commandBuffer.draw(debugLines.GetVertexCount(), 1, 0, 0);
		}
		STOP_BENCHMARK("Physics Debug");

		if constexpr (VulkanConstants::enableValidationLayers)
			commandBuffer.endQuery(queryPool.GetQueryPool(), 0);

		commandBuffer.endRenderPass();


		/*
		 * Third render pass (UI)
		 */

		renderPassInfo.framebuffer = framebuffer;
		renderPassInfo.renderArea.extent = swapchain.GetSwapchainImageExtent();
		renderPassInfo.renderPass = pipeline.GetRenderPass(VulkanPipeline::RenderPassStage::STANDARD);

		VulkanPipeline::SwapAndUpdateViewportAndScissor(commandBuffer, pipeline.GetMainViewport(),
		                                                pipeline.GetMainScissor(), 0, 0,
		                                                swapchain.GetSwapchainImageExtent());

		commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

		ImGuiImpl::RenderDrawData(commandBuffer);

		commandBuffer.endRenderPass();
		ASSERT(commandBuffer.end() == vk::Result::eSuccess,
		       "Failed to record command buffer. ", Core::ELogChannel::CLOG_RENDER);
	}

	void VulkanCommandPool::CopyBuffer(VulkanBuffer& src, VulkanBuffer& dst, vk::DeviceSize size,
	                                   const bool mutexLocked)
	{
		if (!mutexLocked)
			std::lock_guard<std::mutex> lock(commandPoolMutex);
		// Create temp buffer
		vk::CommandBufferAllocateInfo allocInfo{};
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandPool = commandPool.get();
		allocInfo.commandBufferCount = 1;

		vk::CommandBuffer cmdBuffer = (*device)->allocateCommandBuffers(allocInfo).value[0];

		// Record temp buffer
		vk::CommandBufferBeginInfo beginInfo{};
		beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

		ASSERT(cmdBuffer.begin(beginInfo) == vk::Result::eSuccess, "Failed to record copy command buffer. ",
		       Core::ELogChannel::CLOG_RENDER);

		vk::BufferCopy copyRegion{};
		copyRegion.size = size;

		cmdBuffer.copyBuffer(src.GetBuffer(), dst.GetBuffer(), 1, &copyRegion);

		ASSERT(cmdBuffer.end() == vk::Result::eSuccess, "Failed to record copy command buffer. ",
		       Core::ELogChannel::CLOG_RENDER);

		// Submit and wait till it finishes
		vk::SubmitInfo submitInfo{};
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdBuffer;

		std::lock_guard<std::mutex> queueLock(*device->queueMutex);
		{
			ASSERT(device->GraphicsQueue().submit(1, &submitInfo, nullptr) == vk::Result::eSuccess,
			       "Failed to submit copy command buffer. ", Core::ELogChannel::CLOG_RENDER);
			ASSERT(device->GraphicsQueue().waitIdle() == vk::Result::eSuccess,
			       "Failed to finish copy command buffer. ", Core::ELogChannel::CLOG_RENDER);
		}

		(*device)->freeCommandBuffers(commandPool.get(), 1, &cmdBuffer);
	}

	vk::CommandPool VulkanCommandPool::GetCommandPool() const
	{
		return commandPool.get();
	}
}
