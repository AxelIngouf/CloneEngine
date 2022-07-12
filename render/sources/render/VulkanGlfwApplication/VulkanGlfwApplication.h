#pragma once

class Camera;

namespace Render
{
	class FreeCam;
	class VulkanRenderer;
	class EngineWindow;

	class VulkanGlfwApplication
	{
	public:
		VulkanGlfwApplication(size_t width, size_t height, const char* windowTitle);

		static FreeCam& GetCamera();

		void DrawFrame(float deltaTime) const;

	private:
		EngineWindow* window = nullptr;
		VulkanRenderer* renderer;
	};
}
