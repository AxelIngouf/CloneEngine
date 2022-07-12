#pragma once

class Camera;

namespace LibMath
{
	struct Vector3;
}

namespace Render
{
	class FreeCam;
	class VulkanGlfwApplication;

	class RenderApplication
	{
	public:
		RenderApplication(size_t windowWidth, size_t windowHeight, const char* windowTitle);
		void DrawFrame(float deltaTime) const;

		static FreeCam& GetCamera();

	private:
		VulkanGlfwApplication* app = nullptr;
	};
}
