#include "VulkanRenderer.h"


#include "../../../../physic/sources/physic/PhysicsManager.h"
#include "../../imgui/GameViewport.h"
#include "../../imgui/ImguiImpl.h"


#include "core/CLog.h"
#include "core/PoolAllocator.h"
#include "core/ResourceManager.h"
#include "core/DebugWindow/DebugWindow.h"
#include "core/ECS/Entity.h"
#include "core/InputManager/DefaultInputManager.h"
#include "core/scenegraph/SceneNode.h"
#include "render/VulkanConstants.h"
#include "render/EngineWindow/EngineWindow.h"
#include "render/TextureImage/VulkanTextureImage.h"
#include "render/VulkanCommandPool/VulkanCommandPool.h"
#include "render/VulkanDescriptor/VulkanDescriptorPool.h"
#include "render/VulkanFramebuffer/VulkanFramebuffer.h"
#include "render/VulkanPipeline/VulkanPipeline.h"
#include "render/VulkanQueryPool/VulkanQueryPool.h"
#include "render/VulkanSwapchain/VulkanSwapchain.h"
#include "render/VulkanTextureSampler/VulkanTextureSampler.h"
#include "render/VulkanDevice/VulkanDevice.h"
#include "render/VulkanUniformBuffer/VulkanUniformBuffer.h"
#include "model/Model.h"
#include "render/Camera/CameraComponent.h"
#include "render/Camera/FreeCam.h"
#include "render/Image/Image.h"
#include "render/Light/LightComponent.h"
#include "render/RenderComponent/ModelComponent.h"
#include "render/VulkanBuffer/VulkanVertexBuffer.h"
#include "render/VulkanPushConstant/VulkanPushConstant.h"
#include "render/VulkanUniformBuffer/VulkanLightBuffers.h"

namespace Render
{
	bool VulkanRenderer::InitializeRHI(CreationParams& instanceParams, EngineWindow& window)
	{
		if (s_renderer)
			return false;

		s_renderer = this;

		engineWindow = &window;
		bool ok = rhi.Initialize(instanceParams);
		ASSERT(ok, "RHI failed to initialize", Core::ELogChannel::CLOG_RENDER);

		presentSurface = window.CreateWindowSurface(rhi.GetInstance());
		graphicsDevice = rhi.InitializeGraphicsDevice(presentSurface);
		ASSERT(graphicsDevice != nullptr, "Device failed to initialize", Core::ELogChannel::CLOG_RENDER);

		swapchain = Core::MemoryPool::Alloc<VulkanSwapchain>();
		queryPool = Core::MemoryPool::Alloc<VulkanQueryPool>();
		pipeline = Core::MemoryPool::Alloc<VulkanPipeline>();
		sampler = Core::MemoryPool::Alloc<VulkanTextureSampler>();
		sampler->CreateSampler(*graphicsDevice);
		camera = Core::MemoryPool::Alloc<FreeCam>();
		lightBuffers = Core::MemoryPool::Alloc<VulkanLightBuffers>(*graphicsDevice);
		descriptors = Core::MemoryPool::Alloc<VulkanDescriptor>();
		depthDescriptors = Core::MemoryPool::Alloc<VulkanDescriptor>();
		lightDescriptors = Core::MemoryPool::Alloc<VulkanDescriptor>();
		debugLines = Core::MemoryPool::Alloc<VulkanVertexBuffer>();


		ok = swapchain->Initialize(rhi.GetInstance(), *graphicsDevice, window, presentSurface);
		ASSERT(ok, "Failed to initialize swapchain. ", Core::ELogChannel::CLOG_RENDER);

		CreateSwapchainUniformBuffers();
		CreateDirectionalLightBuffers();

		pipeline->CreatePipelineLayout(*graphicsDevice);
		pipeline->CreatePipeline(*graphicsDevice, swapchain->GetSwapchainImageFormat(),
		                         swapchain->GetSwapchainImageExtent());


		CreateSwapchainFramebuffers();

		for (uint32_t i = 0; i < swapchain->GetSwapchainImageCount(); i++)
		{
			commandPool.push_back(Core::MemoryPool::Alloc<VulkanCommandPool>());
			commandPool[i]->CreateCommandPool(*graphicsDevice);
			commandPool[i]->CreateCommandBuffer();
		}

		singleUsePool = Core::MemoryPool::Alloc<VulkanCommandPool>();
		singleUsePool->CreateCommandPool(*graphicsDevice);

		depthFramebuffers.resize(swapchain->GetSwapchainImageCount());
		for (uint32_t i = 0; i < swapchain->GetSwapchainImageCount(); i++)
		{
			depthFramebuffers[i] = Core::MemoryPool::Alloc<VulkanFramebuffer>();
			depthFramebuffers[i]->Initialize(*graphicsDevice);
			depthFramebuffers[i]->InitializeDepthFramebuffer(
				VulkanConstants::shadowMapSize,
				VulkanConstants::shadowMapSize,
				pipeline->GetRenderPass(VulkanPipeline::RenderPassStage::DEPTH));
		}

		offscreenFramebuffers.resize(swapchain->GetSwapchainImageCount());
		for (uint32_t i = 0; i < swapchain->GetSwapchainImageCount(); i++)
		{
			offscreenFramebuffers[i] = Core::MemoryPool::Alloc<VulkanFramebuffer>();
			offscreenFramebuffers[i]->Initialize(*graphicsDevice);
			offscreenFramebuffers[i]->InitializeOffscreenFramebuffer(
				swapchain->GetSwapchainImageExtent().width,
				swapchain->GetSwapchainImageExtent().height,
				pipeline->GetRenderPass(VulkanPipeline::RenderPassStage::OFFSCREEN));
		}

		VulkanDescriptorPool::imGuiPool = VulkanDescriptorPool::CreateImguiPool(*graphicsDevice);
		ImGuiImpl::InitImGui(window, rhi.GetInstance(), *graphicsDevice, *pipeline,
		                     VulkanDescriptorPool::imGuiPool.get(),
		                     *swapchain, *singleUsePool);

		std::array<std::string, 6> cubemapPath = {
			"assets/cubemap/right.png",
			"assets/cubemap/left.png",
			"assets/cubemap/bottom.png",
			"assets/cubemap/top.png",
			"assets/cubemap/front.png",
			"assets/cubemap/back.png"
		};

		CubemapImage::InitializeImage(cubemapPath);
		cubemapImage = ResourceManager::GetResource<CubemapImage>("CUBEMAP_" + cubemapPath[0]);

		descriptors->CreatePerFrameDescriptor(*graphicsDevice, uniformBuffers, *sampler,
		                                      cubemapImage->cubemapTextureImage->GetImageView());
		depthDescriptors->CreateDepthDescriptor(*graphicsDevice, directionalLightBuffers);
		lightDescriptors->CreateLightingDescriptor(*graphicsDevice, *lightBuffers, directionalLightBuffers, *sampler,
		                                           depthFramebuffers);

#ifndef _SHIPPING
		editorUI.Init();
#endif
		GameViewport::Init(swapchain->GetSwapchainImageCount());

		GameViewport::CreateViewportTextures(
			sampler->GetSampler(), offscreenFramebuffers,
			static_cast<VkImageLayout>(vk::ImageLayout::eShaderReadOnlyOptimal));

		GameViewport::CreateDebugViewportTextures(
			sampler->GetSampler(), depthFramebuffers,
			static_cast<VkImageLayout>(vk::ImageLayout::eDepthStencilReadOnlyOptimal),
			GameViewport::SHADOW_MAP);

		ImGuiImpl::ui.Init();

		Core::DebugWindow::AddDebugValue({"Viewport size X", &viewportSizeX}, "Viewport");
		Core::DebugWindow::AddDebugValue({"Viewport size Y", &viewportSizeY}, "Viewport");
		Core::DebugWindow::AddDebugValue({"Viewport aspect ratio", &viewportAspectRatio}, "Viewport");
		Core::DebugWindow::AddDebugValue({"Perspective matrix", &uniformBuffers[0]->GetViewProj().proj}, "Viewport");

		if constexpr (VulkanConstants::enableValidationLayers)
			queryPool->SetupQueryPool(*graphicsDevice);


		// Empty texture for materials with no texture
		auto* tex = Core::MemoryPool::Alloc<VulkanTextureImage>();
		ResourceManager::AddResource(VulkanConstants::emptyTextureName, tex);
		tex->Create(*graphicsDevice, *singleUsePool, 1, 1);

		auto* mat = Core::MemoryPool::Alloc<Material>(*graphicsDevice, *sampler);
		ResourceManager::AddResource(Model::defaultMaterialName, mat);

		auto* nextCheckpointMat = Core::MemoryPool::Alloc<Material>(*graphicsDevice, *sampler);
		ResourceManager::AddResource("nextCheckpointMat", nextCheckpointMat);
		nextCheckpointMat->SetAmbientColor({1.f, 1.f, 0.f});
		nextCheckpointMat->SetDiffuseColor({1.f, 1.f, 0.f});
		nextCheckpointMat->SetSpecularColor({1.f, 1.f, 0.f});
		nextCheckpointMat->SetName("nextCheckpointMat");


		auto* completedCheckpointMat = Core::MemoryPool::Alloc<Material>(*graphicsDevice, *sampler);
		ResourceManager::AddResource("completedCheckpointMat", completedCheckpointMat);
		completedCheckpointMat->SetAmbientColor({0.f, 1.f, 0.f});
		completedCheckpointMat->SetDiffuseColor({0.f, 1.f, 0.f});
		completedCheckpointMat->SetSpecularColor({0.f, 1.f, 0.f});
		completedCheckpointMat->SetName("completedCheckpointMat");


		return ok;
	}

	VulkanRenderer::~VulkanRenderer()
	{
		ImGuiImpl::ShutdownImGui();

		Core::MemoryPool::Free(sampler);
		Core::MemoryPool::Free(swapchain);
		Core::MemoryPool::Free(queryPool);
		Core::MemoryPool::Free(pipeline);
		Core::MemoryPool::Free(camera);
		for (uint32_t i = 0; i < swapchain->GetSwapchainImageCount(); i++)
		{
			Core::MemoryPool::Free(offscreenFramebuffers[i]);
			Core::MemoryPool::Free(commandPool[i]);
			Core::MemoryPool::Free(singleUsePool);
			Core::MemoryPool::Free(swapchainFramebuffers[i]);
		}
	}

	vk::Extent2D VulkanRenderer::HandleViewportResize()
	{
		if (windowResized)
		{
			RecreateSwapchain();
			windowResized = false;
		}

		vk::Extent2D viewportSize = {GameViewport::GetWidth(), GameViewport::GetHeight()};

		if (viewportSize.width <= 0)
			viewportSize.width = 1;
		if (viewportSize.height <= 0)
			viewportSize.height = 1;

		viewportSizeX = static_cast<float>(viewportSize.width);
		viewportSizeY = static_cast<float>(viewportSize.height);

		if (GameViewport::IsResized())
		{
			for (auto& offscreenFramebuffer : offscreenFramebuffers)
			{
				offscreenFramebuffer->InitializeOffscreenFramebuffer(
					viewportSize.width,
					viewportSize.height,
					pipeline->GetRenderPass(VulkanPipeline::RenderPassStage::OFFSCREEN));
			}
			GameViewport::gameViewport.UpdateViewportTextures(*graphicsDevice, sampler->GetSampler(),
			                                                  offscreenFramebuffers,
			                                                  vk::ImageLayout::eShaderReadOnlyOptimal);

			RecreateSwapchainUniformBuffers(viewportSize);

			GameViewport::SetIsResized(false);
		}

		return viewportSize;
	}

	void VulkanRenderer::DrawFrame(const float deltaTime)
	{
		vk::Extent2D viewportSize = HandleViewportResize();

		swapchain->PrepareNewFrame();

		HandleEditorMousePosition(deltaTime);

		START_BENCHMARK("Update ubo");
		uniformBuffers[swapchain->GetFrameIndex()]->UpdateViewMatrix();
		uniformBuffers[swapchain->GetFrameIndex()]->Update(*graphicsDevice);

		UpdateDirectionalLightBuffers(viewportAspectRatio);

		UpdateLightBuffers();

		STOP_BENCHMARK("Update ubo");

		START_BENCHMARK("UI draw");

#ifndef _SHIPPING
		editorUI.UpdateStyle();
#endif

		ImGuiImpl::NewFrame();
#ifdef _SHIPPING
		int width, height;
		engineWindow->GetWindowSize(&width, &height);
		GameViewport::gameViewport.Draw(swapchain->GetFrameIndex(), width, height);
#else
		editorUI.Draw(swapchain->GetFrameIndex());
#endif
		ImGuiImpl::Render();
		STOP_BENCHMARK("UI draw");

		std::vector<Physics::DebugVertex> debugVertices;
		GetDebugLines(debugVertices);


		debugLines->ClearBuffer();

		{
			std::lock_guard<std::mutex> lock(singleUsePool->GetCommandPoolMutex());
			debugLines->Initialize(*graphicsDevice, *singleUsePool, debugVertices);

			commandPool[swapchain->GetFrameIndex()]->RecordCommandBuffer(
				swapchainFramebuffers[swapchain->GetImageInFlightIndex()]->GetFramebuffer(),
				offscreenFramebuffers[swapchain->GetFrameIndex()]->GetFramebuffer(),
				depthFramebuffers[swapchain->GetFrameIndex()]->GetFramebuffer(),
				*swapchain,
				*pipeline,
				descriptors->GetDescriptorSet(swapchain->GetFrameIndex()),
				depthDescriptors->GetDescriptorSet(swapchain->GetFrameIndex()),
				lightDescriptors->GetDescriptorSet(swapchain->GetFrameIndex()),
				viewportSize,
				*debugLines,
				*queryPool);

			if constexpr (VulkanConstants::enableValidationLayers)
				queryPool->GetQueryResults(*graphicsDevice);

			START_BENCHMARK("Submit");
			swapchain->SubmitCommandBuffers(commandPool[swapchain->GetFrameIndex()]->GetCommandBuffer(), 1);
		}
		STOP_BENCHMARK("Submit");

		START_BENCHMARK("Present");
		swapchain->PresentFrame();
		STOP_BENCHMARK("Present");

		commandPool[swapchain->GetFrameIndex()]->ResetCommandPool();
	}

	void VulkanRenderer::WaitIdle() const
	{
		ASSERT((*graphicsDevice)->waitIdle() == vk::Result::eSuccess, "Device failed to wait. ",
		       Core::ELogChannel::CLOG_RENDER);
	}

	void VulkanRenderer::CreateSwapchainFramebuffers()
	{
		swapchainFramebuffers.resize(swapchain->GetSwapchainImageCount());

		for (uint32_t i = 0; i < swapchain->GetSwapchainImageCount(); i++)
		{
			swapchainFramebuffers[i] = Core::MemoryPool::Alloc<VulkanFramebuffer>();
			swapchainFramebuffers[i]->Initialize(*graphicsDevice);
			swapchainFramebuffers[i]->InitializeSwapchainFramebuffer(
				i, *swapchain,
				pipeline->GetRenderPass(VulkanPipeline::RenderPassStage::STANDARD));
		}
	}

	void VulkanRenderer::FillProjectionMatrices(const float aspectRatio)
	{
		for (auto& uniformBuffer : uniformBuffers)
		{
			uniformBuffer->GetViewProj().proj = LibMath::Matrix4::PerspectiveLh(
				LibMath::Degree(VulkanConstants::cameraHalfFov),
				aspectRatio,
				VulkanConstants::cameraNearPlane,
				VulkanConstants::cameraFarPlane);

			// flip y coordinate
			uniformBuffer->GetViewProj().proj[1][1] *= -1;

			uniformBuffer->UpdateViewMatrix();
		}
	}

	void VulkanRenderer::CreateSwapchainUniformBuffers()
	{
		uniformBuffers.clear();
		for (uint32_t i = 0; i < swapchain->GetImagesInFlightCount(); i++)
		{
			uniformBuffers.push_back(Core::MemoryPool::Alloc<VulkanUniformBuffer>(*graphicsDevice));
		}

		const float aspectRatio = static_cast<float>(swapchain->GetSwapchainImageExtent().width) /
			static_cast<float>(swapchain->GetSwapchainImageExtent().height);

		FillProjectionMatrices(aspectRatio);
	}

	void VulkanRenderer::CreateDirectionalLightBuffers()
	{
		directionalLightBuffers.clear();
		for (uint32_t i = 0; i < swapchain->GetImagesInFlightCount(); i++)
		{
			directionalLightBuffers.push_back(
				Core::MemoryPool::Alloc<VulkanUniformBuffer>(*graphicsDevice));
		}

		for (auto& d : directionalLightBuffers)
		{
			d->GetViewProj().proj = LibMath::Matrix4::OrthographicLh(-1.f, 1.f, -1.f, 1.f, 0.1f, 1.f);

			// flip y coordinate
			d->GetViewProj().proj[1][1] *= -1;
		}
	}

	void VulkanRenderer::UpdateDirectionalLightBuffers(float /*aspectRatio*/)
	{
		using namespace LibMath;

		auto it = DirectionalLightComponent::GetAll();
		if (it.Next())
		{
			const Vector3 pos = GetCameraPosition();
			Vector3 dir = Core::Entity::GetEntity(
				it->GetEntityHandle())->GetAnchor()->GetWorldTransformNoCheck().rotation * Vector3::Down;

			dir *= VulkanConstants::dirLightOrthoFar * 0.5f;

			directionalLightBuffers[swapchain->GetFrameIndex()]->GetViewProj().view =
				Matrix4::LookAtLh(pos - dir, pos, {0.f, 1.f, 0.f});
			directionalLightBuffers[swapchain->GetFrameIndex()]->GetViewProj().viewPos = Vector4::Point(pos - dir);

			directionalLightBuffers[swapchain->GetFrameIndex()]->GetViewProj().proj =
				Matrix4::OrthographicLh(-VulkanConstants::dirLightOrthoSize, VulkanConstants::dirLightOrthoSize,
				                        -VulkanConstants::dirLightOrthoSize, VulkanConstants::dirLightOrthoSize,
				                        VulkanConstants::dirLightOrthoNear, VulkanConstants::dirLightOrthoFar);


			// flip y coordinate
			directionalLightBuffers[swapchain->GetFrameIndex()]->GetViewProj().proj[1][1] *= -1;
		}

		directionalLightBuffers[swapchain->GetFrameIndex()]->Update(*graphicsDevice);
	}

	void VulkanRenderer::RecreateSwapchain()
	{
		WaitIdle();

		swapchain->RecreateSwapchain(*engineWindow);

		CreateSwapchainUniformBuffers();

		descriptors->UpdatePerFrameDescriptors(*graphicsDevice, uniformBuffers);
		depthDescriptors->UpdateDepthDescriptors(*graphicsDevice, directionalLightBuffers);
		lightDescriptors->UpdateLightingDescriptors(*graphicsDevice, *lightBuffers, directionalLightBuffers, *sampler,
		                                            depthFramebuffers);

		pipeline->RecreatePipeline(*graphicsDevice, swapchain->GetSwapchainImageFormat(),
		                           swapchain->GetSwapchainImageExtent());

		CreateSwapchainFramebuffers();
	}

	void VulkanRenderer::RecreateSwapchainUniformBuffers(const vk::Extent2D& viewportSize)
	{
		viewportAspectRatio = static_cast<float>(viewportSize.width) / static_cast<float>(viewportSize.height);

		FillProjectionMatrices(viewportAspectRatio);
	}

	void VulkanRenderer::HandleEditorMousePosition(float deltaTime)
	{
		if (Core::DefaultInputManager::GetActionState(EActionEnum::MOUSE_RIGHT_CLICK) == Core::EActionState::PRESSED
			|| Core::DefaultInputManager::GetActionState(EActionEnum::MOUSE_RIGHT_CLICK) == Core::EActionState::HOLD)
		{
			double xPos, yPos;
			Core::DefaultInputManager::GetMousePosition(&xPos, &yPos);
			if (GameViewport::PositionIsInViewport((float)xPos, (float)yPos))
			{
				if (Core::DefaultInputManager::GetActionState(EActionEnum::MOUSE_RIGHT_CLICK) ==
					Core::EActionState::PRESSED)
				{
					Core::DefaultInputManager::ToggleMouseCapture();
					mouseCaptured = true;
				}
			}
		}

		if (mouseCaptured)
		{
			camera->Move(deltaTime);
			camera->Rotate(static_cast<float>(Core::DefaultInputManager::GetMouseHorizontalMove()),
			               static_cast<float>(Core::DefaultInputManager::GetMouseVerticalMove()),
			               deltaTime);

			if (Core::DefaultInputManager::GetActionState(EActionEnum::MOUSE_RIGHT_CLICK) ==
				Core::EActionState::RELEASED)
			{
				Core::DefaultInputManager::ToggleMouseCapture();
				mouseCaptured = false;
			}
		}
	}

	VulkanTextureImage* VulkanRenderer::LoadVulkanTexture(const Model::Texture& texture)
	{
		VulkanTextureImage* vulkanTextureImage = Core::MemoryPool::Alloc<VulkanTextureImage>();
		vulkanTextureImage->Create(
			*s_renderer->graphicsDevice,
			*s_renderer->singleUsePool,
			texture);

		return vulkanTextureImage;
	}

	VulkanTextureImage* VulkanRenderer::LoadVulkanCubemapTexture(const std::array<Model::Texture*, 6>& textures)
	{
		VulkanTextureImage* vulkanTextureImage = Core::MemoryPool::Alloc<VulkanTextureImage>();
		vulkanTextureImage->CreateCubemap(
			*s_renderer->graphicsDevice,
			*s_renderer->singleUsePool,
			textures);

		return vulkanTextureImage;
	}

	void* VulkanRenderer::LoadImGuiTexture(const VulkanTextureImage& vulkanTextureImage)
	{
		return ImGuiImpl::CreateImGuiTexture(s_renderer->sampler->GetSampler(), vulkanTextureImage.GetImageView(),
		                                     static_cast<VkImageLayout>(vk::ImageLayout::eShaderReadOnlyOptimal));
	}

	void VulkanRenderer::AddVulkanTextureToMaterial(const std::string& matName, const std::string& texturePath,
	                                                const Model::MaterialTextureLocation materialTextureLocation)
	{
		auto* mat = ResourceManager::GetResource<Material>(matName);

		if (!mat)
		{
			mat = Core::MemoryPool::Alloc<Material
			>(*s_renderer->graphicsDevice, *s_renderer->sampler);
			ResourceManager::AddResource(matName, mat);
			mat->SetName(matName);
		}

		auto* image = ResourceManager::GetResource<Image>(texturePath);

		if (!image)
		{
			Image::InitializeImage(texturePath);
			image = ResourceManager::GetResource<Image>(texturePath);
		}

		mat->SetTextureImage(image, materialTextureLocation);
	}

	void VulkanRenderer::AddValuesToMaterial(const std::string& matName, const LibMath::Vector3& value,
	                                         const Model::MaterialValueLocation materialValueLocation)
	{
		auto* mat = ResourceManager::GetResource<Material>(matName);

		if (!mat)
		{
			mat = Core::MemoryPool::Alloc<Material>
				(*s_renderer->graphicsDevice, *s_renderer->sampler);
			ResourceManager::AddResource(matName, mat);
			mat->SetName(matName);
		}

		switch (materialValueLocation)
		{
		case Model::MaterialValueLocation::AMBIENT_COLOR:
			mat->SetAmbientColor(value);
			break;
		case Model::MaterialValueLocation::DIFFUSE_COLOR:
			mat->SetDiffuseColor(value);
			break;
		case Model::MaterialValueLocation::SPECUALR_COLOR:
			mat->SetSpecularColor(value);
			break;
		case Model::MaterialValueLocation::SPECULAR_EXPONENT:
			mat->SetSpecularExponent(value.x);
			break;
		case Model::MaterialValueLocation::ALPHA:
			mat->SetAlpha(value.x);
			break;
		}
	}

	void VulkanRenderer::AddVulkanBuffersToModel(ModelComponent* model, const std::string& path)
	{
		Model::Model* data = ResourceManager::GetResource<Model::Model>(path);

		if (!data)
		{
			Model::Model mod = Model::Model(path.c_str());
			data = ResourceManager::GetResource<Model::Model>(path);
		}

		model->modelMatrix = new VulkanPushConstant();

		model->meshes.reserve(data->meshes.size());

		for (auto& m : data->meshes)
		{
			model->AddMaterialToModel(m.materialName);
			model->meshes.emplace_back(
				*s_renderer->graphicsDevice, *s_renderer->singleUsePool,
				m.vertices, m.indices, m.materialName);
		}
	}

	void VulkanRenderer::UpdateLightBuffers()
	{
		s_renderer->lightBuffers->Update(*s_renderer->graphicsDevice);
	}

	void VulkanRenderer::RefreshShaders()
	{
		s_renderer->pipeline->RecreatePipeline(*s_renderer->graphicsDevice,
		                                       s_renderer->swapchain->GetSwapchainImageFormat(),
		                                       s_renderer->swapchain->GetSwapchainImageExtent(), true);
	}

	void VulkanRenderer::TogglePhysXDebug(const bool value)
	{
		Physics::EnableDebugVisualization(value);
		if (!value)
		{
			s_renderer->debugLines->ClearBuffer();
		}
	}

	LibMath::Matrix4 VulkanRenderer::GetCameraLookAt()
	{
		if (Core::World::IsInPlay())
		{
			auto it = CameraComponent::GetAll();
			if (it.Next())
			{
				return it->ComputeLookAt();
			}
		}

		return s_renderer->camera->ComputeLookAt();
	}

	LibMath::Vector3 VulkanRenderer::GetCameraPosition()
	{
		if (Core::World::IsInPlay())
		{
			auto it = CameraComponent::GetAll();
			if (it.Next())
			{
				return it->GetPosition();
			}
		}

		return s_renderer->camera->GetPosition();
	}

	LibMath::Vector3 VulkanRenderer::GetCameraFront()
	{
		if (Core::World::IsInPlay())
		{
			auto it = CameraComponent::GetAll();
			if (it.Next())
			{
				return it->GetFront();
			}
		}

		return s_renderer->camera->GetFront();
	}

	LibMath::Vector3 VulkanRenderer::GetCameraRight()
	{
		if (Core::World::IsInPlay())
		{
			auto it = CameraComponent::GetAll();
			if (it.Next())
			{
				return it->GetRight();
			}
		}

		return s_renderer->camera->GetRight();
	}

	LibMath::Vector3 VulkanRenderer::GetCameraUp()
	{
		if (Core::World::IsInPlay())
		{
			auto it = CameraComponent::GetAll();
			if (it.Next())
			{
				return it->GetUp();
			}
		}

		return s_renderer->camera->GetUp();
	}
}
