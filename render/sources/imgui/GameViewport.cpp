#include "GameViewport.h"


#include "ImguiImpl.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_vulkan.h"
#include "render/VulkanConstants.h"
#include "render/VulkanMacros.h"
#include "render/VulkanFramebuffer/VulkanFramebuffer.h"
#include "render/VulkanDevice/VulkanDevice.h"

using namespace ImGui;

GameViewport GameViewport::gameViewport;

void GameViewport::Init(uint32_t swapchainFrameNum)
{
	gameViewport.viewportTextures.resize(swapchainFrameNum);

	for (auto& i : gameViewport.debugViewportTextures)
		i.resize(swapchainFrameNum);

	gameViewport.frames.resize(Render::VulkanConstants::fpsGraphSize);

#ifdef _SHIPPING
	ImGuiStyle& style = GetStyle();
	style.WindowBorderSize = 0.0f;
	style.WindowPadding = { 0, 0 };
#endif
}

void GameViewport::CreateViewportTextures(const vk::Sampler& sampler,
                                          const std::vector<Render::VulkanFramebuffer*>& framebuffers,
                                          const enum VkImageLayout layout)
{
	for (size_t i = 0; i < gameViewport.viewportTextures.size(); i++)
	{
		gameViewport.viewportTextures[i] =
			ImGui_ImplVulkan_AddTexture(sampler, framebuffers[i]->GetImageView(), layout);

		// pointer to standard viewport, no need to update it
		gameViewport.debugViewportTextures[STANDARD_VIEWPORT][i] = gameViewport.viewportTextures[i];
	}
}

void GameViewport::CreateDebugViewportTextures(const vk::Sampler& sampler,
                                               const std::vector<Render::VulkanFramebuffer*>& framebuffers,
                                               const enum VkImageLayout layout,
                                               const DebugViewportTexture debugViewportTexture)
{
	for (size_t i = 0; i < gameViewport.debugViewportTextures[debugViewportTexture].size(); i++)
	{
		gameViewport.debugViewportTextures[debugViewportTexture][i] =
			ImGui_ImplVulkan_AddTexture(sampler, framebuffers[i]->GetImageView(), layout);
	}
}

void GameViewport::UpdateViewportTextures(const Render::VulkanDevice& device, const vk::Sampler& sampler,
                                          const std::vector<Render::VulkanFramebuffer*>& framebuffers,
                                          const vk::ImageLayout layout)
{
	for (size_t i = 0; i < viewportTextures.size(); i++)
	{
		vk::DescriptorImageInfo imageInfo = {};
		imageInfo.sampler = sampler;
		imageInfo.imageView = framebuffers[i]->GetImageView();
		imageInfo.imageLayout = layout;
		vk::WriteDescriptorSet writeInfo = {};
		writeInfo.dstSet = static_cast<VkDescriptorSet>(viewportTextures[i]);
		writeInfo.descriptorCount = 1;
		writeInfo.descriptorType = vk::DescriptorType::eCombinedImageSampler;
		writeInfo.pImageInfo = &imageInfo;

		device->updateDescriptorSets(1, &writeInfo, 0, nullptr);
	}
}

void GameViewport::Draw(const int idx, const int windowWidth, const int windowHeight)
{
	const ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoSavedSettings | ImGuiDockNodeFlags_NoResize;

	SetNextWindowPos({0, 0});

	SetNextWindowSize({static_cast<float>(windowWidth), static_cast<float>(windowHeight)});

	DrawViewport(idx, flags);
}

void GameViewport::DrawViewport(const int idx, const int flags)
{
	Begin("Viewport", nullptr, flags);

	const auto viewportSize = GetContentRegionAvail();

	ImVec2 vMin = GetWindowContentRegionMin();
	ImVec2 vMax = GetWindowContentRegionMax();

	vMin.x += GetWindowPos().x;
	vMin.y += GetWindowPos().y;
	vMax.x += GetWindowPos().x;
	vMax.y += GetWindowPos().y;

	mainViewportMinPos.x = vMin.x;
	mainViewportMinPos.y = vMin.y;
	mainViewportMaxPos.x = vMax.x;
	mainViewportMaxPos.y = vMax.y;

	if (viewportWidth != static_cast<uint32_t>(viewportSize.x) ||
		viewportHeight != static_cast<uint32_t>(viewportSize.y))
	{
		isResized = true;
		viewportWidth = static_cast<uint32_t>(viewportSize.x);
		viewportHeight = static_cast<uint32_t>(viewportSize.y);
	}

	Image(viewportTextures[idx], viewportSize);

	ImGuiImpl::ui.Draw();

	End();
}


void GameViewport::DrawDebugViewport(const int idx, const int flags)
{
	if (!renderDebugViewport)
		return;

	Begin("Debug Viewport", nullptr, flags);

	ImVec2 viewportSize = GetContentRegionAvail();

	const float width = viewportSize.y * (viewportWidth / (float)viewportHeight);
	const float height = viewportSize.x * (viewportHeight / (float)viewportWidth);

	if (height <= viewportSize.y)
		viewportSize.y = height;
	else
		viewportSize.x = width;

	if (debugViewportTextures[currentDebugViewport][idx] == nullptr)
	{
		Image(debugViewportTextures[STANDARD_VIEWPORT][idx], viewportSize);
		LOG(LOG_ERROR, "Target debug viewport texture not bound. ", Core::ELogChannel::CLOG_RENDER);
	}
	else
		Image(debugViewportTextures[currentDebugViewport][idx], viewportSize);

	ImGuiImpl::ui.Draw();

	End();
}

void GameViewport::DrawViewportSelectorMenu()
{
	Combo("Debug Viewport Target", (int*)(&currentDebugViewport), debugViewportTargetNames, NUM_DEBUG_VIEWPORTS);
	Checkbox("Render Debug Viewport", &renderDebugViewport);
}

bool GameViewport::PositionIsInViewport(const float xPos, const float yPos)
{
	return
		xPos >= gameViewport.mainViewportMinPos.x && xPos <= gameViewport.mainViewportMaxPos.x &&
		yPos >= gameViewport.mainViewportMinPos.y && yPos <= gameViewport.mainViewportMaxPos.y;
}
