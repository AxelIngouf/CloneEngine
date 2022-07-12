#pragma once

#include "../VulkanDescriptor/VulkanDescriptor.h"
#include "render/Shaders/ShaderModule.h"
#include "render/VulkanRenderPass/VulkanRenderPass.h"

namespace Render
{
	class VulkanUniformBuffer;
	class VulkanDevice;

	class VulkanPipeline
	{
	public:

		void CreatePipelineLayout(const VulkanDevice& device);
		void CreatePipeline(const VulkanDevice& device, const vk::Format& colorFormat,
		                    const vk::Extent2D& swapchainExtent);

		void RecreatePipeline(const VulkanDevice& device, const vk::Format& colorFormat,
		                      const vk::Extent2D& swapchainExtent, bool recompileShaders = false);


		enum class PipelineStage
		{
			STANDARD,
			SKYBOX,
			DEPTH,
			PHYSICS_DEBUG
		};

		enum class RenderPassStage
		{
			STANDARD,
			OFFSCREEN,
			DEPTH
		};

		[[nodiscard]] const vk::Pipeline& GetPipeline(PipelineStage pipelineStage) const;
		[[nodiscard]] const vk::PipelineLayout& GetLayout(PipelineStage pipelineStage) const;
		[[nodiscard]] const vk::RenderPass& GetRenderPass(RenderPassStage renderPassStage) const;

		[[nodiscard]] vk::Viewport& GetMainViewport()
		{
			return mainViewport;
		}

		[[nodiscard]] vk::Viewport& GetOffscreenViewport()
		{
			return offscreenViewport;
		}

		[[nodiscard]] vk::Rect2D& GetMainScissor()
		{
			return mainScissor;
		}

		[[nodiscard]] vk::Rect2D& GetOffscreenScissor()
		{
			return offscreenScissor;
		}

		static void SwapAndUpdateViewportAndScissor(const vk::CommandBuffer& commandBuffer, vk::Viewport& viewport,
		                                            vk::Rect2D& scissor, float xOffset, float yOffset,
		                                            vk::Extent2D size);

	private:
		[[nodiscard]] static vk::PipelineInputAssemblyStateCreateInfo GenerateInputAssemblyCreateInfo(
			vk::PrimitiveTopology topology, vk::PipelineInputAssemblyStateCreateFlags flags,
			bool primitiveRestartEnable);
		[[nodiscard]] static vk::PipelineRasterizationStateCreateInfo GenerateRasterizerCreateInfo(
			vk::PolygonMode polygonMode, vk::CullModeFlags cullMode, vk::FrontFace frontFace,
			vk::PipelineRasterizationStateCreateFlags flags = vk::PipelineRasterizationStateCreateFlags{});
		[[nodiscard]] static vk::PipelineColorBlendAttachmentState GenerateColorBlendCreateInfo(
			vk::ColorComponentFlags colorWriteMask, bool blendEnable);
		[[nodiscard]] static vk::PipelineColorBlendStateCreateInfo GenerateColorBlendStateCreateInfo(
			uint32_t attachmentCount, const vk::PipelineColorBlendAttachmentState* pAttachments);
		[[nodiscard]] static vk::PipelineDepthStencilStateCreateInfo GenerateDepthCreateInfo(
			bool depthTestEnable, bool depthWriteEnable, vk::CompareOp depthCompareOp);
		[[nodiscard]] static vk::PipelineViewportStateCreateInfo GenerateViewportCreateInfo(
			uint32_t viewportCount, uint32_t scissorCount,
			vk::PipelineViewportStateCreateFlags flags = vk::PipelineViewportStateCreateFlags{});
		[[nodiscard]] static vk::PipelineMultisampleStateCreateInfo GenerateMultisampleCreateInfo(
			vk::SampleCountFlagBits rasterizationSamples,
			vk::PipelineMultisampleStateCreateFlags flags = vk::PipelineMultisampleStateCreateFlags{});
		[[nodiscard]] static vk::PipelineDynamicStateCreateInfo GenerateDynamicStateCreateInfo(
			const std::vector<vk::DynamicState>& pDynamicStates,
			vk::PipelineDynamicStateCreateFlags flags = vk::PipelineDynamicStateCreateFlags{});
		[[nodiscard]] static vk::PipelineVertexInputStateCreateInfo GenerateVertexInputCreateInfo(
			const vk::VertexInputBindingDescription& bindingDescription,
			const std::vector<vk::VertexInputAttributeDescription>& attributeDescriptions);
		[[nodiscard]] static vk::GraphicsPipelineCreateInfo GeneratePipelineCreateInfo(
			vk::PipelineLayout layout, vk::RenderPass renderPass,
			vk::PipelineCreateFlags flags = vk::PipelineCreateFlags{});
		[[nodiscard]] static vk::Viewport CreateViewport(float x, float y, vk::Extent2D extent);
		[[nodiscard]] static vk::Rect2D CreateScissor(vk::Extent2D extent);


		VulkanRenderPass renderPass;
		VulkanRenderPass offscreenRenderPass;
		VulkanRenderPass depthRenderPass;

		vk::UniquePipeline skyboxPipeline;
		vk::UniquePipeline standardPipeline;
		vk::UniquePipeline depthPipeline;
		vk::UniquePipeline physicsDebugPipeline;

		vk::UniquePipelineLayout standardPipelineLayout;
		vk::UniquePipelineLayout skyboxPipelineLayout;
		vk::UniquePipelineLayout depthPipelineLayout;
		vk::UniquePipelineLayout physicsDebugPipelineLayout;

		vk::PipelineCache pipelineCache;

		vk::Viewport mainViewport;
		vk::Rect2D mainScissor;
		vk::Viewport offscreenViewport;
		vk::Rect2D offscreenScissor;

		ShaderModule skyboxVertexModule;
		ShaderModule skyboxFragmentModule;
		ShaderModule standardVertexModule;
		ShaderModule standardFragmentModule;
		ShaderModule physicsDebugVertexModule;
		ShaderModule physicsDebugFragmentModule;
		ShaderModule depthVertexModule;
	};
}
