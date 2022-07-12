#pragma once
#include <cstdint>
#include <set>
#include <string>
#include <vector>


#include "UI.h"
#include "core/Delegate.h"

namespace std
{
	class mutex;
}

namespace vk
{
	class CommandBuffer;
	class Instance;
	class DescriptorPool;
	class ImageView;
	class Sampler;
}

enum VkImageLayout;

namespace Render
{
	class VulkanCommandPool;
	class VulkanSwapchain;
	class VulkanDescriptorPool;
	class VulkanPipeline;
	class VulkanDevice;
	class VulkanInstance;
	class EngineWindow;
}

DELEGATE_One_Param(TextureToLoad, std::string, filepath);

DELEGATE(ShaderRefresh);

DELEGATE_One_Param(PhysXDebug, bool, value);

class ImGuiImpl
{
public:
	static bool Header(const char* caption);
	static bool CheckBox(const char* caption, bool* value);
	static bool CheckBox(const char* caption, int32_t* value);
	static bool InputFloat(const char* caption, float* value, float step, const char* precision);
	static bool SliderFloat(const char* caption, float* value, float min, float max);
	static bool SliderInt(const char* caption, int32_t* value, int32_t min, int32_t max);
	static bool ComboBox(const char* caption, int32_t* itemIndex, const std::vector<std::string>& items);
	static bool Button(const char* caption);
	static void Text(const char* formatStr, ...);

	static void OpenWindow(const char* name);
	static void NewFrame();

	static void InitImGui(const Render::EngineWindow& window, vk::Instance instance, const Render::VulkanDevice& device,
	                      const Render::VulkanPipeline& pipeline, const vk::DescriptorPool& descriptorPool,
	                      const Render::VulkanSwapchain& swapchain, Render::VulkanCommandPool& commandPool);

	static void ShutdownImGui();

	static void RenderDrawData(vk::CommandBuffer commandBuffer);

	static void Render();

	[[nodiscard]] static void* CreateImGuiTexture(const vk::Sampler& sampler, const vk::ImageView& view,
	                                              VkImageLayout layout);
	static void LoadImGuiTexture(const std::string& filePath);


	static UI ui;

private:
	static bool IsTexturePendingLoad(const std::string& filePath);

	static std::set<std::string> pendingTextures;

	static std::mutex mutex;
};
