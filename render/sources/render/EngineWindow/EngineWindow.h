#pragma once

#include <string>
#include "core/InputManager/InputManager.h"

struct VkSurfaceKHR_T;
struct VkInstance_T;
enum VkResult;

namespace Render
{
	class EngineWindow
	{
	public:
		EngineWindow() = delete;
		~EngineWindow() = default;

		EngineWindow(const char* windowName, uint32_t width, uint32_t height, bool* bufferResizeFlag);

		EngineWindow(const EngineWindow&) = delete;
		EngineWindow(EngineWindow&&) = delete;
		EngineWindow& operator=(const EngineWindow& other) = delete;
		EngineWindow& operator=(EngineWindow&&) = delete;

		[[nodiscard]] bool ShouldClose() const;

		// Creates a Vulkan surface to pass to the renderer
		VkSurfaceKHR_T* CreateWindowSurface(VkInstance_T* instance) const;

		// Enters the size of the framebuffer in width and height
		void GetFramebufferSize(int* width, int* height) const;

		void DestroyWindow() const;

		static std::vector<const char*> GetRequiredWindowExtensions();

		void SetWindowTitle(const std::string& title) const;

		void SetWindowIcon(int width, int height, unsigned char* pixels) const;

		void GetWindowSize(int* width, int* height) const;

		[[nodiscard]] GLFWwindow* GetGLFWWindow() const { return window; }

		bool IsMinimized() const;

	private:
		static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
		static void InitWindow();

		GLFWwindow* window = nullptr;
		bool* framebufferResized = nullptr;
	};

	static EngineWindow* WindowInstance = nullptr;
}
