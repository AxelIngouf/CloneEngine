#include "EngineWindow.h"
#include "core/CLog.h"

#include "render/VulkanMacros.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "core/InputManager/DefaultInputManager.h"

extern "C" {
extern GLFWAPI VkResult glfwCreateWindowSurface(VkInstance instance, GLFWwindow* window,
                                                const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface);
}

namespace Render
{
	EngineWindow::EngineWindow(const char* windowName, const uint32_t width, const uint32_t height,
	                           bool* bufferResizeFlag)
	{
		ASSERT(!WindowInstance, "Window instance already created", Core::ELogChannel::CLOG_RENDER);

		InitWindow();

		WindowInstance = this;
		framebufferResized = bufferResizeFlag;

		window = glfwCreateWindow(width, height, windowName, nullptr, nullptr);
		glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

		Core::DefaultInputManager::Init(window);
	}

	void EngineWindow::InitWindow()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	}

	std::vector<const char*> EngineWindow::GetRequiredWindowExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		return extensions;
	}

	bool EngineWindow::ShouldClose() const
	{
		return glfwWindowShouldClose(window);
	}

	VkSurfaceKHR_T* EngineWindow::CreateWindowSurface(VkInstance_T* instance) const
	{
		VkSurfaceKHR newSurface;

		const auto result = glfwCreateWindowSurface(instance /*Vulkan instance*/, window /*the current window*/,
		                                            nullptr /*allocator*/, &newSurface /*Vulkan surface*/);

		ASSERT(result == VkResult::VK_SUCCESS, "Failed to created glfw window!", Core::ELogChannel::CLOG_RENDER);

		return newSurface;
	}

	void EngineWindow::GetFramebufferSize(int* width, int* height) const
	{
		glfwGetFramebufferSize(window, width, height);
	}

	void EngineWindow::DestroyWindow() const
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void EngineWindow::SetWindowTitle(const std::string& title) const
	{
		glfwSetWindowTitle(window, title.c_str());
	}

	void EngineWindow::SetWindowIcon(const int width, const int height, unsigned char* pixels) const
	{
		GLFWimage windowIcon;
		windowIcon.width = width;
		windowIcon.height = height;
		windowIcon.pixels = pixels;


		glfwSetWindowIcon(window, 1, &windowIcon);
	}

	void EngineWindow::GetWindowSize(int* width, int* height) const
	{
		glfwGetWindowSize(window, width, height);
	}

    bool EngineWindow::IsMinimized() const
    {
		int width, height;
		GetWindowSize(&width, &height);

		return width < 1 || height < 1;
    }

	void EngineWindow::FramebufferSizeCallback(GLFWwindow* /*window*/, int /*width*/, int /*height*/)
	{
		*WindowInstance->framebufferResized = true;
	}
}
