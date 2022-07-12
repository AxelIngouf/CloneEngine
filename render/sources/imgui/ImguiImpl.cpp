#include "ImguiImpl.h"

#include "mutex"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_vulkan.h"
#include "core/CLog.h"
#include "core/ResourceManager.h"
#include "render/EngineWindow/EngineWindow.h"
#include "render/Image/Image.h"
#include "render/VulkanCommandPool/VulkanCommandPool.h"
#include "render/VulkanDevice/VulkanDevice.h"
#include "render/VulkanPipeline/VulkanPipeline.h"
#include "render/VulkanRenderer/VulkanRenderer.h"
#include "render/VulkanSwapchain/VulkanSwapchain.h"

using namespace ImGui;

std::set<std::string> ImGuiImpl::pendingTextures;
std::mutex ImGuiImpl::mutex;
UI ImGuiImpl::ui;

bool ImGuiImpl::Header(const char* caption)
{
	return CollapsingHeader(caption, ImGuiTreeNodeFlags_DefaultOpen);
}

bool ImGuiImpl::CheckBox(const char* caption, bool* value)
{
	const bool res = Checkbox(caption, value);
	return res;
}

bool ImGuiImpl::CheckBox(const char* caption, int32_t* value)
{
	bool val = (*value == 1);
	const bool res = Checkbox(caption, &val);
	*value = val;
	return res;
}

bool ImGuiImpl::InputFloat(const char* caption, float* value, const float step, const char* precision)
{
	const bool res = ImGui::InputFloat(caption, value, step, step * 10.0f, precision);
	return res;
}

bool ImGuiImpl::SliderFloat(const char* caption, float* value, const float min, const float max)
{
	const bool res = ImGui::SliderFloat(caption, value, min, max);
	return res;
}

bool ImGuiImpl::SliderInt(const char* caption, int32_t* value, const int32_t min, const int32_t max)
{
	const bool res = ImGui::SliderInt(caption, value, min, max);
	return res;
}

bool ImGuiImpl::ComboBox(const char* caption, int32_t* itemIndex, const std::vector<std::string>& items)
{
	if (items.empty())
	{
		return false;
	}
	std::vector<const char*> charItems;
	charItems.reserve(items.size());
	for (const auto& item : items)
	{
		charItems.push_back(item.c_str());
	}
	const auto itemCount = static_cast<uint32_t>(charItems.size());
	const bool res = Combo(caption, itemIndex, &charItems[0], itemCount, itemCount);
	return res;
}

bool ImGuiImpl::Button(const char* caption)
{
	const bool res = ImGui::Button(caption);
	return res;
}

void ImGuiImpl::Text(const char* formatStr, ...)
{
	va_list args;
	va_start(args, formatStr);
	TextV(formatStr, args);
	va_end(args);
}

void ImGuiImpl::OpenWindow(const char* name)
{
	const ImGuiWindowFlags windowFlags = 0;

	Begin(name, nullptr, windowFlags);

	SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
	SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

	End();
}

void ImGuiImpl::NewFrame()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void ImGuiImpl::InitImGui(const Render::EngineWindow& window, vk::Instance instance, const Render::VulkanDevice& device,
                          const Render::VulkanPipeline& pipeline, const vk::DescriptorPool& descriptorPool,
                          const Render::VulkanSwapchain& swapchain, Render::VulkanCommandPool& commandPool)
{
	IMGUI_CHECKVERSION();
	CreateContext();
	ImGuiIO& io = GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	StyleColorsDark();

	ImGui_ImplGlfw_InitForVulkan(window.GetGLFWWindow(), true);
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = instance;
	init_info.PhysicalDevice = device.GetPhysicalDevice();
	init_info.Device = *device;
	init_info.QueueFamily = device.GetQueueFamilyIndicesArray()[0]; // Graphics queue
	init_info.Queue = device.GraphicsQueue();
	init_info.PipelineCache = nullptr;
	init_info.DescriptorPool = descriptorPool;
	// todo use vulkan allocator
	init_info.Allocator = nullptr;
	init_info.MinImageCount = 2;
	init_info.ImageCount = swapchain.GetSwapchainImagesKHR();
	//init_info.CheckVkResultFn = check_vk_result;
	ImGui_ImplVulkan_Init(&init_info, pipeline.GetRenderPass(Render::VulkanPipeline::RenderPassStage::STANDARD));

	// Upload Fonts
	{
		vk::CommandBuffer command_buffer = commandPool.BeginSingleTimeCommands();

		ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

		commandPool.EndSingleTimeCommands(command_buffer);

		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}
}

void ImGuiImpl::ShutdownImGui()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	DestroyContext();
}

void ImGuiImpl::RenderDrawData(vk::CommandBuffer commandBuffer)
{
	ImDrawData* draw_data = GetDrawData();

	ImGui_ImplVulkan_RenderDrawData(draw_data, commandBuffer);
}

void ImGuiImpl::Render()
{
	ImGui::Render();

	EndFrame();
}

void* ImGuiImpl::CreateImGuiTexture(const vk::Sampler& sampler, const vk::ImageView& view,
                                    const enum VkImageLayout layout)
{
	return ImGui_ImplVulkan_AddTexture(sampler, view, layout);
}

void ImGuiImpl::LoadImGuiTexture(const std::string& filePath)
{
	{
		std::lock_guard<std::mutex> lock(mutex);
		if (IsTexturePendingLoad(filePath))
		{
			return;
		}

		pendingTextures.emplace(filePath);
	}

	Render::Image::InitializeImage(filePath);

	{
		std::lock_guard<std::mutex> lock(mutex);
		pendingTextures.erase(pendingTextures.find(filePath));
	}
}

bool ImGuiImpl::IsTexturePendingLoad(const std::string& filePath)
{
	return pendingTextures.find(filePath) != pendingTextures.end();
}
