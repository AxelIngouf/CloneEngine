#pragma once
#include <vector>
#include "VulkanMacros.h"

namespace Render::VulkanConstants
{
	static constexpr const char* applicationName("Clone Engine");
	static constexpr const char* engineName("Clone Engine");
	static constexpr const char* emptyTextureName("EMPTY_TEXTURE_vk");

	static constexpr size_t offscreenFramebufferWidth = 1920;
	static constexpr size_t offscreenFramebufferHeight = 1080;

	static constexpr size_t fpsGraphSize = 120;

	static constexpr size_t shadowMapSize = 8192;
	static constexpr float depthBiasConstant = 1.25f;
	static constexpr float depthBiasSlope = 1.75f;

	static constexpr float cameraHalfFov = 45.f;
	static constexpr float cameraNearPlane = 0.01f;
	static constexpr float cameraFarPlane = 200.f;


	static constexpr float dirLightOrthoSize = 75.f;
	static constexpr float dirLightOrthoNear = 0.1f;
	static constexpr float dirLightOrthoFar = 200.f;


	static constexpr int maxFramesInFlight = 2;

	static inline const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
	static inline const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef _DEBUG
	static constexpr bool enableValidationLayers = true;
#else
	static constexpr bool enableValidationLayers = false;
#endif
}
