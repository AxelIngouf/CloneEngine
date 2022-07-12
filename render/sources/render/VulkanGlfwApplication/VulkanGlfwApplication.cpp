#include "VulkanGlfwApplication.h"

#include <iostream>



#include "render/Vertex/Vertex.h"
#include "core/CLog.h"
#include "core/GameLoop.h"
#include "core/PoolAllocator.h"
#include "core/ResourceManager.h"
#include "core/DebugWindow/DebugWindow.h"
#include "core/ECS/World.h"
#include "core/scenegraph/SceneGraph.h"
#include "core/InputManager/DefaultInputManager.h"
#include "model/Texture.h"
#include "render/Camera/FreeCam.h"
#include "render/EngineWindow/EngineWindow.h"
#include "render/VulkanInstance/VulkanInstance.h"
#include "render/VulkanRenderer/VulkanRenderer.h"
#include "render/Camera/CameraComponent.h"
#include "render/Image/Image.h"

namespace Render
{
	VulkanGlfwApplication::VulkanGlfwApplication(const size_t width, const size_t height, const char* windowTitle)
	{
		renderer = Core::MemoryPool::Alloc<VulkanRenderer>();

		window = new EngineWindow(windowTitle, static_cast<uint32_t>(width), static_cast<uint32_t>(height),
		                          &renderer->GetWindowResized());

		const std::vector<const char*> extensions = EngineWindow::GetRequiredWindowExtensions();

		//todo: move to config file
		CreationParams instanceParams{
			ExtensionList(extensions),
			"RaceEngine Editor",
			"RaceEngine"
		};

		ASSERT(renderer->InitializeRHI(instanceParams, *window), "RHI could not initialize. ",
		       Core::ELogChannel::CLOG_RENDER);

		const std::string windowIconPath = "assets_editor/CloneEngineLogoSmall.png";
		Image::InitializeImage(windowIconPath, false);
		Model::Texture windowIcon = *ResourceManager::GetResource<Image>(windowIconPath)->texture;

		window->SetWindowIcon(windowIcon.width, windowIcon.height, &windowIcon.data[0]);

		// Camera keybinds
		renderer->GetCamera().SetPosition({0.f, 0.f, -5.f});

		Core::DebugWindow::AddDebugValue({"Camera position", &renderer->GetCamera().GetPosition()}, "Camera");
		Core::DebugWindow::AddDebugValue({"Camera rotation", &renderer->GetCamera().GetRotationEuler()}, "Camera");
		Core::DebugWindow::AddDebugValue({"Camera front", &renderer->GetCamera().GetFront()}, "Camera");
		Core::DebugWindow::AddDebugValue({"Camera right", &renderer->GetCamera().GetRight()}, "Camera");
		Core::DebugWindow::AddDebugValue({"Camera up", &renderer->GetCamera().GetUp()}, "Camera");
		Core::DebugWindow::AddDebugValue({"Camera yaw", &renderer->GetCamera().GetYaw()}, "Camera");
		Core::DebugWindow::AddDebugValue({"Camera pitch", &renderer->GetCamera().GetPitch()}, "Camera");
		Core::DebugWindow::AddDebugValue({"Camera look at matrix", &renderer->GetCamera().GetLookAt()}, "Camera");


		Core::DefaultInputManager::OnPressed(EActionEnum::FREE_CAM_FORWARD)->Add(
			std::bind(&FreeCam::StartMovingDir, &renderer->GetCamera(), FreeCam::Moving::FRONT));
		Core::DefaultInputManager::OnPressed(EActionEnum::FREE_CAM_BACKWARD)->Add(
			std::bind(&FreeCam::StartMovingDir, &renderer->GetCamera(), FreeCam::Moving::BACK));
		Core::DefaultInputManager::OnPressed(EActionEnum::FREE_CAM_LEFT)->Add(
			std::bind(&FreeCam::StartMovingDir, &renderer->GetCamera(), FreeCam::Moving::LEFT));
		Core::DefaultInputManager::OnPressed(EActionEnum::FREE_CAM_RIGHT)->Add(
			std::bind(&FreeCam::StartMovingDir, &renderer->GetCamera(), FreeCam::Moving::RIGHT));
		Core::DefaultInputManager::OnPressed(EActionEnum::FREE_CAM_UP)->Add(
			std::bind(&FreeCam::StartMovingDir, &renderer->GetCamera(), FreeCam::Moving::UP));
		Core::DefaultInputManager::OnPressed(EActionEnum::FREE_CAM_DOWN)->Add(
			std::bind(&FreeCam::StartMovingDir, &renderer->GetCamera(), FreeCam::Moving::DOWN));
		Core::DefaultInputManager::OnPressed(EActionEnum::FREE_CAM_SPRINT)->Add(
			std::bind(&FreeCam::StartSprint, &renderer->GetCamera()));

		Core::DefaultInputManager::OnReleased(EActionEnum::FREE_CAM_FORWARD)->Add(
			std::bind(&FreeCam::StopMovingDir, &renderer->GetCamera(), FreeCam::Moving::FRONT));
		Core::DefaultInputManager::OnReleased(EActionEnum::FREE_CAM_BACKWARD)->Add(
			std::bind(&FreeCam::StopMovingDir, &renderer->GetCamera(), FreeCam::Moving::BACK));
		Core::DefaultInputManager::OnReleased(EActionEnum::FREE_CAM_LEFT)->Add(
			std::bind(&FreeCam::StopMovingDir, &renderer->GetCamera(), FreeCam::Moving::LEFT));
		Core::DefaultInputManager::OnReleased(EActionEnum::FREE_CAM_RIGHT)->Add(
			std::bind(&FreeCam::StopMovingDir, &renderer->GetCamera(), FreeCam::Moving::RIGHT));
		Core::DefaultInputManager::OnReleased(EActionEnum::FREE_CAM_UP)->Add(
			std::bind(&FreeCam::StopMovingDir, &renderer->GetCamera(), FreeCam::Moving::UP));
		Core::DefaultInputManager::OnReleased(EActionEnum::FREE_CAM_DOWN)->Add(
			std::bind(&FreeCam::StopMovingDir, &renderer->GetCamera(), FreeCam::Moving::DOWN));
		Core::DefaultInputManager::OnReleased(EActionEnum::FREE_CAM_SPRINT)->Add(
			std::bind(&FreeCam::StopSprint, &renderer->GetCamera()));
	}

	FreeCam& VulkanGlfwApplication::GetCamera()
	{
		return VulkanRenderer::GetCamera();
	}


	void VulkanGlfwApplication::DrawFrame(const float deltaTime) const
	{
		if (!window->ShouldClose())
		{
			if(!window->IsMinimized())
			{
				renderer->DrawFrame(deltaTime);
			}
			return;
		}

		renderer->WaitIdle();
		Core::GameLoop::Stop();
	}
}
