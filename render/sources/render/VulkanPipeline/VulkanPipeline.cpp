#include "VulkanPipeline.h"

#include "render/Shaders/ShaderModule.h"
#include "render/Vertex/Vertex.h"
#include "render/VulkanConstants.h"
#include "render/Vertex/PhysicsDebugVertex.h"
#include "render/VulkanDescriptor/VulkanDescriptorSetLayout.h"
#include "render/VulkanDevice/VulkanDevice.h"
#include "render/VulkanPushConstant/VulkanPushConstant.h"

namespace Render
{
	void VulkanPipeline::CreatePipelineLayout(const VulkanDevice& device)
	{
		{
			// Standard shader
			vk::UniqueDescriptorSetLayout frameDescriptorLayout =
				VulkanDescriptorSetLayout::CreatePerFrameLayout(device);
			vk::UniqueDescriptorSetLayout objectDescriptorLayout =
				VulkanDescriptorSetLayout::CreatePerObjectLayout(device);
			vk::UniqueDescriptorSetLayout lightDescriptorLayout =
				VulkanDescriptorSetLayout::CreateLightingLayout(device);

			std::vector<vk::DescriptorSetLayout> layouts = {
				frameDescriptorLayout.get(), lightDescriptorLayout.get(), objectDescriptorLayout.get()
			};

			vk::PushConstantRange pushConstant;
			pushConstant.offset = 0;
			pushConstant.size = sizeof(VulkanPushConstant);
			pushConstant.stageFlags = vk::ShaderStageFlagBits::eVertex;

			vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
			pipelineLayoutInfo.pushConstantRangeCount = 1;
			pipelineLayoutInfo.pPushConstantRanges = &pushConstant;
			pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
			pipelineLayoutInfo.pSetLayouts = layouts.data();

			standardPipelineLayout = device->createPipelineLayoutUnique(pipelineLayoutInfo).value;
		}

		{
			// Depth shader
			vk::UniqueDescriptorSetLayout directionalLightDescriptor =
				VulkanDescriptorSetLayout::CreateDepthLayout(device);

			std::vector<vk::DescriptorSetLayout> layouts = {
				directionalLightDescriptor.get()
			};


			vk::PushConstantRange pushConstant;
			pushConstant.offset = 0;
			pushConstant.size = sizeof(VulkanPushConstant);
			pushConstant.stageFlags = vk::ShaderStageFlagBits::eVertex;

			vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
			pipelineLayoutInfo.pushConstantRangeCount = 1;
			pipelineLayoutInfo.pPushConstantRanges = &pushConstant;
			pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
			pipelineLayoutInfo.pSetLayouts = layouts.data();

			depthPipelineLayout = device->createPipelineLayoutUnique(pipelineLayoutInfo).value;
		}


		{
			// Skybox shader
			vk::UniqueDescriptorSetLayout frameDescriptorLayout =
				VulkanDescriptorSetLayout::CreatePerFrameLayout(device);

			std::vector<vk::DescriptorSetLayout> layouts = {frameDescriptorLayout.get()};


			vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
			pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
			pipelineLayoutInfo.pSetLayouts = layouts.data();

			skyboxPipelineLayout = device->createPipelineLayoutUnique(pipelineLayoutInfo).value;
		}

		{
			// Physics debug shader
			vk::UniqueDescriptorSetLayout frameDescriptorLayout =
				VulkanDescriptorSetLayout::CreatePerFrameLayout(device);

			std::vector<vk::DescriptorSetLayout> layouts = {frameDescriptorLayout.get()};


			vk::PipelineLayoutCreateInfo pipelineLayoutInfo = {};
			pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
			pipelineLayoutInfo.pSetLayouts = layouts.data();

			physicsDebugPipelineLayout = device->createPipelineLayoutUnique(pipelineLayoutInfo).value;
		}
	}

	void VulkanPipeline::CreatePipeline(const VulkanDevice& device, const vk::Format& colorFormat,
	                                    const vk::Extent2D& swapchainExtent)
	{
		renderPass.CreateRenderPass(device, colorFormat);
		offscreenRenderPass.CreateOffscreenRenderPass(device, colorFormat);
		depthRenderPass.CreateDepthRenderPass(device);

		mainViewport = CreateViewport(0.f, 0.f, swapchainExtent);
		offscreenViewport = CreateViewport(0.f, 0.f, swapchainExtent);
		mainScissor = CreateScissor(swapchainExtent);
		offscreenScissor = CreateScissor(swapchainExtent);

		auto inputAssemblyState = GenerateInputAssemblyCreateInfo(vk::PrimitiveTopology::eTriangleList,
		                                                          vk::PipelineInputAssemblyStateCreateFlags{}, false);
		auto rasterizationState = GenerateRasterizerCreateInfo(vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack,
		                                                       vk::FrontFace::eCounterClockwise);
		auto blendAttachmentState = GenerateColorBlendCreateInfo(
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB |
			vk::ColorComponentFlagBits::eA, false);
		auto colorBlendState = GenerateColorBlendStateCreateInfo(1, &blendAttachmentState);
		auto viewportState = GenerateViewportCreateInfo(1, 1);
		auto multisampleState = GenerateMultisampleCreateInfo(vk::SampleCountFlagBits::e1);
		std::vector<vk::DynamicState> dynamicStateEnables = {
			vk::DynamicState::eViewport, vk::DynamicState::eScissor
		};
		auto dynamicState = GenerateDynamicStateCreateInfo(dynamicStateEnables);

		auto bindingDescription = Vertex::GetBindingDescription();
		auto attributeDescription = Vertex::GetAttributeDescriptions();

		auto vertexInputInfo = GenerateVertexInputCreateInfo(bindingDescription, attributeDescription);

		std::array<vk::PipelineShaderStageCreateInfo, 3> shaderStages;
		skyboxVertexModule.Initialize(device, "skyboxVert", vk::ShaderStageFlagBits::eVertex);
		skyboxFragmentModule.Initialize(device, "skyboxFrag", vk::ShaderStageFlagBits::eFragment);
		standardVertexModule.Initialize(device, "vert", vk::ShaderStageFlagBits::eVertex);
		standardFragmentModule.Initialize(device, "frag", vk::ShaderStageFlagBits::eFragment);
		depthVertexModule.Initialize(device, "depthVert", vk::ShaderStageFlagBits::eVertex);
		physicsDebugVertexModule.Initialize(device, "physicsDebugVert", vk::ShaderStageFlagBits::eVertex);
		physicsDebugFragmentModule.Initialize(device, "physicsDebugFrag", vk::ShaderStageFlagBits::eFragment);

		{
			// Skybox pipeline
			auto depthStencilState = GenerateDepthCreateInfo(false, false, vk::CompareOp::eLessOrEqual);
			auto pipelineCI = GeneratePipelineCreateInfo(skyboxPipelineLayout.get(), renderPass.GetRenderPass());
			pipelineCI.pInputAssemblyState = &inputAssemblyState;
			pipelineCI.pRasterizationState = &rasterizationState;
			pipelineCI.pColorBlendState = &colorBlendState;
			pipelineCI.pMultisampleState = &multisampleState;
			pipelineCI.pViewportState = &viewportState;
			pipelineCI.pDepthStencilState = &depthStencilState;
			pipelineCI.pDynamicState = &dynamicState;
			pipelineCI.stageCount = 2;
			pipelineCI.pStages = shaderStages.data();
			pipelineCI.pVertexInputState = &vertexInputInfo;


			shaderStages[0] = skyboxVertexModule.stageInfo;
			shaderStages[1] = skyboxFragmentModule.stageInfo;
			rasterizationState.cullMode = vk::CullModeFlagBits::eFront;

			skyboxPipeline = device->createGraphicsPipelineUnique(pipelineCache, pipelineCI).value;
		}

		{
			// Standard pipeline (shadows)
			auto depthStencilState = GenerateDepthCreateInfo(true, true, vk::CompareOp::eLessOrEqual);
			auto pipelineCI = GeneratePipelineCreateInfo(standardPipelineLayout.get(), renderPass.GetRenderPass());
			pipelineCI.pInputAssemblyState = &inputAssemblyState;
			pipelineCI.pRasterizationState = &rasterizationState;
			pipelineCI.pColorBlendState = &colorBlendState;
			pipelineCI.pMultisampleState = &multisampleState;
			pipelineCI.pViewportState = &viewportState;
			pipelineCI.pDepthStencilState = &depthStencilState;
			pipelineCI.pDynamicState = &dynamicState;
			pipelineCI.stageCount = 2;
			pipelineCI.pStages = shaderStages.data();
			pipelineCI.pVertexInputState = &vertexInputInfo;


			shaderStages[0] = standardVertexModule.stageInfo;
			shaderStages[1] = standardFragmentModule.stageInfo;
			rasterizationState.cullMode = vk::CullModeFlagBits::eBack;

			standardPipeline = device->createGraphicsPipelineUnique(pipelineCache, pipelineCI).value;
		}


		{
			// Shadow pipeline (only vertex)
			auto depthStencilState = GenerateDepthCreateInfo(true, true, vk::CompareOp::eLessOrEqual);
			auto pipelineCI = GeneratePipelineCreateInfo(depthPipelineLayout.get(), depthRenderPass.GetRenderPass());
			pipelineCI.pInputAssemblyState = &inputAssemblyState;
			pipelineCI.pRasterizationState = &rasterizationState;
			pipelineCI.pColorBlendState = &colorBlendState;
			pipelineCI.pMultisampleState = &multisampleState;
			pipelineCI.pViewportState = &viewportState;
			pipelineCI.pDepthStencilState = &depthStencilState;
			pipelineCI.pDynamicState = &dynamicState;
			pipelineCI.stageCount = 1;
			shaderStages[0] = depthVertexModule.stageInfo;
			pipelineCI.pStages = shaderStages.data();
			pipelineCI.pVertexInputState = &vertexInputInfo;

			// No blend attachment states (no color attachments used)
			colorBlendState.attachmentCount = 0;
			// Cull front faces
			rasterizationState.cullMode = vk::CullModeFlagBits::eFront;
			// Enable depth bias
			rasterizationState.depthBiasEnable = VK_TRUE;

			depthPipeline = device->createGraphicsPipelineUnique(pipelineCache, pipelineCI).value;
		}

		{
			// Physics debug pipeline
			auto depthStencilState = GenerateDepthCreateInfo(true, true, vk::CompareOp::eLessOrEqual);
			auto pipelineCI = GeneratePipelineCreateInfo(physicsDebugPipelineLayout.get(), renderPass.GetRenderPass());
			inputAssemblyState = GenerateInputAssemblyCreateInfo(vk::PrimitiveTopology::eLineList,
			                                                     vk::PipelineInputAssemblyStateCreateFlags{}, false);


			bindingDescription = PhysicsDebugVertex::GetBindingDescription();
			attributeDescription = PhysicsDebugVertex::GetAttributeDescriptions();

			vertexInputInfo = GenerateVertexInputCreateInfo(bindingDescription, attributeDescription);


			pipelineCI.pInputAssemblyState = &inputAssemblyState;
			pipelineCI.pRasterizationState = &rasterizationState;
			pipelineCI.pColorBlendState = &colorBlendState;
			pipelineCI.pMultisampleState = &multisampleState;
			pipelineCI.pViewportState = &viewportState;
			pipelineCI.pDepthStencilState = &depthStencilState;
			pipelineCI.pDynamicState = &dynamicState;
			pipelineCI.stageCount = 2;
			pipelineCI.pStages = shaderStages.data();
			pipelineCI.pVertexInputState = &vertexInputInfo;

			colorBlendState.attachmentCount = 1;
			rasterizationState.cullMode = vk::CullModeFlagBits::eBack;
			rasterizationState.depthBiasEnable = VK_FALSE;

			shaderStages[0] = physicsDebugVertexModule.stageInfo;
			shaderStages[1] = physicsDebugFragmentModule.stageInfo;

			physicsDebugPipeline = device->createGraphicsPipelineUnique(pipelineCache, pipelineCI).value;
		}
	}

	void VulkanPipeline::RecreatePipeline(const VulkanDevice& device, const vk::Format& colorFormat,
	                                      const vk::Extent2D& swapchainExtent, bool recompileShaders)
	{
		renderPass.CreateRenderPass(device, colorFormat);
		offscreenRenderPass.CreateOffscreenRenderPass(device, colorFormat);
		depthRenderPass.CreateDepthRenderPass(device);

		mainViewport = CreateViewport(0.f, 0.f, swapchainExtent);
		offscreenViewport = CreateViewport(0.f, 0.f, swapchainExtent);
		mainScissor = CreateScissor(swapchainExtent);
		offscreenScissor = CreateScissor(swapchainExtent);

		auto inputAssemblyState = GenerateInputAssemblyCreateInfo(vk::PrimitiveTopology::eTriangleList,
		                                                          vk::PipelineInputAssemblyStateCreateFlags{}, false);
		auto rasterizationState = GenerateRasterizerCreateInfo(vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack,
		                                                       vk::FrontFace::eCounterClockwise);
		auto blendAttachmentState = GenerateColorBlendCreateInfo(
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB |
			vk::ColorComponentFlagBits::eA, false);
		auto colorBlendState = GenerateColorBlendStateCreateInfo(1, &blendAttachmentState);
		auto viewportState = GenerateViewportCreateInfo(1, 1);
		auto multisampleState = GenerateMultisampleCreateInfo(vk::SampleCountFlagBits::e1);
		std::vector<vk::DynamicState> dynamicStateEnables = {
			vk::DynamicState::eViewport, vk::DynamicState::eScissor
		};
		auto dynamicState = GenerateDynamicStateCreateInfo(dynamicStateEnables);

		auto bindingDescription = Vertex::GetBindingDescription();
		auto attributeDescription = Vertex::GetAttributeDescriptions();

		auto vertexInputInfo = GenerateVertexInputCreateInfo(bindingDescription, attributeDescription);

		std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages;


		if (recompileShaders)
		{
			skyboxVertexModule.Initialize(device, "skyboxVert", vk::ShaderStageFlagBits::eVertex);
			skyboxFragmentModule.Initialize(device, "skyboxFrag", vk::ShaderStageFlagBits::eFragment);
			standardVertexModule.Initialize(device, "vert", vk::ShaderStageFlagBits::eVertex);
			standardFragmentModule.Initialize(device, "frag", vk::ShaderStageFlagBits::eFragment);
			physicsDebugVertexModule.Initialize(device, "physicsDebugVert", vk::ShaderStageFlagBits::eVertex);
			physicsDebugFragmentModule.Initialize(device, "physicsDebugFrag", vk::ShaderStageFlagBits::eFragment);
		}

		{
			// Skybox pipeline
			auto depthStencilState = GenerateDepthCreateInfo(false, false, vk::CompareOp::eLessOrEqual);
			auto pipelineCI = GeneratePipelineCreateInfo(skyboxPipelineLayout.get(), renderPass.GetRenderPass());
			pipelineCI.pInputAssemblyState = &inputAssemblyState;
			pipelineCI.pRasterizationState = &rasterizationState;
			pipelineCI.pColorBlendState = &colorBlendState;
			pipelineCI.pMultisampleState = &multisampleState;
			pipelineCI.pViewportState = &viewportState;
			pipelineCI.pDepthStencilState = &depthStencilState;
			pipelineCI.pDynamicState = &dynamicState;
			pipelineCI.stageCount = static_cast<uint32_t>(shaderStages.size());
			pipelineCI.pStages = shaderStages.data();
			pipelineCI.pVertexInputState = &vertexInputInfo;


			shaderStages[0] = skyboxVertexModule.stageInfo;
			shaderStages[1] = skyboxFragmentModule.stageInfo;
			rasterizationState.cullMode = vk::CullModeFlagBits::eFront;

			skyboxPipeline = device->createGraphicsPipelineUnique(pipelineCache, pipelineCI).value;
		}

		{
			// Standard pipeline
			auto depthStencilState = GenerateDepthCreateInfo(true, true, vk::CompareOp::eLessOrEqual);
			auto pipelineCI = GeneratePipelineCreateInfo(standardPipelineLayout.get(), renderPass.GetRenderPass());
			pipelineCI.pInputAssemblyState = &inputAssemblyState;
			pipelineCI.pRasterizationState = &rasterizationState;
			pipelineCI.pColorBlendState = &colorBlendState;
			pipelineCI.pMultisampleState = &multisampleState;
			pipelineCI.pViewportState = &viewportState;
			pipelineCI.pDepthStencilState = &depthStencilState;
			pipelineCI.pDynamicState = &dynamicState;
			pipelineCI.stageCount = static_cast<uint32_t>(shaderStages.size());
			pipelineCI.pStages = shaderStages.data();
			pipelineCI.pVertexInputState = &vertexInputInfo;

			shaderStages[0] = standardVertexModule.stageInfo;
			shaderStages[1] = standardFragmentModule.stageInfo;
			rasterizationState.cullMode = vk::CullModeFlagBits::eBack;

			standardPipeline = device->createGraphicsPipelineUnique(pipelineCache, pipelineCI).value;
		}

		{
			// Physics debug pipeline
			auto depthStencilState = GenerateDepthCreateInfo(true, true, vk::CompareOp::eLessOrEqual);
			auto pipelineCI = GeneratePipelineCreateInfo(physicsDebugPipelineLayout.get(), renderPass.GetRenderPass());
			inputAssemblyState = GenerateInputAssemblyCreateInfo(vk::PrimitiveTopology::eLineList,
			                                                     vk::PipelineInputAssemblyStateCreateFlags{}, false);


			bindingDescription = PhysicsDebugVertex::GetBindingDescription();
			attributeDescription = PhysicsDebugVertex::GetAttributeDescriptions();

			vertexInputInfo = GenerateVertexInputCreateInfo(bindingDescription, attributeDescription);


			pipelineCI.pInputAssemblyState = &inputAssemblyState;
			pipelineCI.pRasterizationState = &rasterizationState;
			pipelineCI.pColorBlendState = &colorBlendState;
			pipelineCI.pMultisampleState = &multisampleState;
			pipelineCI.pViewportState = &viewportState;
			pipelineCI.pDepthStencilState = &depthStencilState;
			pipelineCI.pDynamicState = &dynamicState;
			pipelineCI.stageCount = 2;
			pipelineCI.pStages = shaderStages.data();
			pipelineCI.pVertexInputState = &vertexInputInfo;

			colorBlendState.attachmentCount = 1;
			rasterizationState.cullMode = vk::CullModeFlagBits::eBack;
			rasterizationState.depthBiasEnable = VK_FALSE;

			shaderStages[0] = physicsDebugVertexModule.stageInfo;
			shaderStages[1] = physicsDebugFragmentModule.stageInfo;

			physicsDebugPipeline = device->createGraphicsPipelineUnique(pipelineCache, pipelineCI).value;
		}
	}

	const vk::Pipeline& VulkanPipeline::GetPipeline(PipelineStage pipelineStage) const
	{
		switch (pipelineStage)
		{
		case PipelineStage::STANDARD: return standardPipeline.get();
		case PipelineStage::SKYBOX: return skyboxPipeline.get();
		case PipelineStage::DEPTH: return depthPipeline.get();
		case PipelineStage::PHYSICS_DEBUG: return physicsDebugPipeline.get();
		}

		ASSERT(false, "Unexpected pipeline stage", Core::ELogChannel::CLOG_RENDER);
		return standardPipeline.get();
	}

	const vk::PipelineLayout& VulkanPipeline::GetLayout(PipelineStage pipelineStage) const
	{
		switch (pipelineStage)
		{
		case PipelineStage::STANDARD: return standardPipelineLayout.get();
		case PipelineStage::SKYBOX: return skyboxPipelineLayout.get();
		case PipelineStage::DEPTH: return depthPipelineLayout.get();
		case PipelineStage::PHYSICS_DEBUG: return physicsDebugPipelineLayout.get();
		}

		ASSERT(false, "Unexpected pipeline stage", Core::ELogChannel::CLOG_RENDER);
		return standardPipelineLayout.get();
	}

	const vk::RenderPass& VulkanPipeline::GetRenderPass(RenderPassStage renderPassStage) const
	{
		switch (renderPassStage)
		{
		case RenderPassStage::STANDARD: return renderPass.GetRenderPass();
		case RenderPassStage::OFFSCREEN: return offscreenRenderPass.GetRenderPass();
		case RenderPassStage::DEPTH: return depthRenderPass.GetRenderPass();
		}

		ASSERT(false, "Unexpected render pass stage", Core::ELogChannel::CLOG_RENDER);
		return renderPass.GetRenderPass();
	}

	void VulkanPipeline::SwapAndUpdateViewportAndScissor(const vk::CommandBuffer& commandBuffer, vk::Viewport& viewport,
	                                                     vk::Rect2D& scissor, const float xOffset, const float yOffset,
	                                                     const vk::Extent2D size)
	{
		viewport.width = (float)size.width;
		viewport.height = (float)size.height;
		viewport.x = xOffset;
		viewport.y = yOffset;

		scissor.extent = size;

		commandBuffer.setViewport(0, 1, &viewport);
		commandBuffer.setScissor(0, 1, &scissor);
	}

	vk::PipelineVertexInputStateCreateInfo VulkanPipeline::GenerateVertexInputCreateInfo(
		const vk::VertexInputBindingDescription& bindingDescription,
		const std::vector<vk::VertexInputAttributeDescription>& attributeDescriptions)
	{
		vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};

		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		return vertexInputInfo;
	}

	vk::PipelineInputAssemblyStateCreateInfo VulkanPipeline::GenerateInputAssemblyCreateInfo(
		vk::PrimitiveTopology topology,
		vk::PipelineInputAssemblyStateCreateFlags flags,
		bool primitiveRestartEnable)
	{
		vk::PipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.topology = topology;
		inputAssembly.flags = flags;
		inputAssembly.primitiveRestartEnable = primitiveRestartEnable;

		return inputAssembly;
	}

	vk::PipelineViewportStateCreateInfo VulkanPipeline::GenerateViewportCreateInfo(uint32_t viewportCount,
		uint32_t scissorCount, vk::PipelineViewportStateCreateFlags flags)
	{
		vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};
		pipelineViewportStateCreateInfo.viewportCount = viewportCount;
		pipelineViewportStateCreateInfo.scissorCount = scissorCount;
		pipelineViewportStateCreateInfo.flags = flags;
		return pipelineViewportStateCreateInfo;
	}

	vk::PipelineRasterizationStateCreateInfo VulkanPipeline::GenerateRasterizerCreateInfo(
		vk::PolygonMode polygonMode, vk::CullModeFlags cullMode, vk::FrontFace frontFace,
		vk::PipelineRasterizationStateCreateFlags flags)
	{
		vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};
		pipelineRasterizationStateCreateInfo.polygonMode = polygonMode;
		pipelineRasterizationStateCreateInfo.cullMode = cullMode;
		pipelineRasterizationStateCreateInfo.frontFace = frontFace;
		pipelineRasterizationStateCreateInfo.flags = flags;
		pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
		return pipelineRasterizationStateCreateInfo;
	}

	vk::PipelineMultisampleStateCreateInfo VulkanPipeline::GenerateMultisampleCreateInfo(
		vk::SampleCountFlagBits rasterizationSamples, vk::PipelineMultisampleStateCreateFlags flags)
	{
		vk::PipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{};
		pipelineMultisampleStateCreateInfo.rasterizationSamples = rasterizationSamples;
		pipelineMultisampleStateCreateInfo.flags = flags;
		return pipelineMultisampleStateCreateInfo;
	}

	vk::PipelineColorBlendAttachmentState VulkanPipeline::GenerateColorBlendCreateInfo(
		vk::ColorComponentFlags colorWriteMask, bool blendEnable)
	{
		vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{};
		pipelineColorBlendAttachmentState.colorWriteMask = colorWriteMask;
		pipelineColorBlendAttachmentState.blendEnable = blendEnable;
		return pipelineColorBlendAttachmentState;
	}

	vk::PipelineColorBlendStateCreateInfo VulkanPipeline::GenerateColorBlendStateCreateInfo(uint32_t attachmentCount,
		const vk::PipelineColorBlendAttachmentState* pAttachments)
	{
		vk::PipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};
		pipelineColorBlendStateCreateInfo.attachmentCount = attachmentCount;
		pipelineColorBlendStateCreateInfo.pAttachments = pAttachments;
		return pipelineColorBlendStateCreateInfo;
	}

	vk::PipelineDynamicStateCreateInfo VulkanPipeline::GenerateDynamicStateCreateInfo(
		const std::vector<vk::DynamicState>& pDynamicStates,
		vk::PipelineDynamicStateCreateFlags flags)
	{
		vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
		pipelineDynamicStateCreateInfo.pDynamicStates = pDynamicStates.data();
		pipelineDynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(pDynamicStates.size());
		pipelineDynamicStateCreateInfo.flags = flags;
		return pipelineDynamicStateCreateInfo;
	}


	vk::PipelineDepthStencilStateCreateInfo VulkanPipeline::GenerateDepthCreateInfo(
		bool depthTestEnable,
		bool depthWriteEnable,
		vk::CompareOp depthCompareOp)
	{
		vk::PipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.depthTestEnable = depthTestEnable;
		depthStencil.depthWriteEnable = depthWriteEnable;
		depthStencil.depthCompareOp = depthCompareOp;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;
		return depthStencil;
	}

	vk::GraphicsPipelineCreateInfo VulkanPipeline::GeneratePipelineCreateInfo(vk::PipelineLayout layout,
	                                                                          vk::RenderPass renderPass,
	                                                                          vk::PipelineCreateFlags flags)
	{
		vk::GraphicsPipelineCreateInfo pipelineCreateInfo{};
		pipelineCreateInfo.layout = layout;
		pipelineCreateInfo.renderPass = renderPass;
		pipelineCreateInfo.flags = flags;
		pipelineCreateInfo.basePipelineIndex = -1;
		pipelineCreateInfo.basePipelineHandle = nullptr;
		return pipelineCreateInfo;
	}

	vk::Viewport VulkanPipeline::CreateViewport(const float x, const float y, const vk::Extent2D extent)
	{
		vk::Viewport ret;

		ret.x = x;
		ret.y = y;
		ret.width = (float)extent.width;
		ret.height = (float)extent.height;
		ret.minDepth = 0.0f;
		ret.maxDepth = 1.0f;

		return ret;
	}

	vk::Rect2D VulkanPipeline::CreateScissor(const vk::Extent2D extent)
	{
		vk::Rect2D ret;

		ret.offset = vk::Offset2D{0, 0};
		ret.extent = extent;

		return ret;
	}
}
