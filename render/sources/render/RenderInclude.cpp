#include "RenderInclude.h"
#include "render/VulkanGlfwApplication/VulkanGlfwApplication.h"
#include "render/Camera/FreeCam.h"

namespace Render
{
	RenderApplication::RenderApplication(const size_t windowWidth, const size_t windowHeight, const char* windowTitle)
	{
		app = new VulkanGlfwApplication(windowWidth, windowHeight, windowTitle);
	}

	void RenderApplication::DrawFrame(const float deltaTime) const
	{
		app->DrawFrame(deltaTime);
	}

	FreeCam& RenderApplication::GetCamera()
	{
		return VulkanGlfwApplication::GetCamera();
	}
}
