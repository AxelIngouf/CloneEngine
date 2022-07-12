#pragma once

#include <array>
#include <vector>

#include "Vector/TVector2.h"

namespace Render
{
	class VulkanDevice;
	class VulkanFramebuffer;
}

namespace vk
{
	class ImageView;
	enum class ImageLayout;
	class Sampler;
}

enum VkImageLayout;

class GameViewport
{
public:

	enum class TargetViewport
	{
		MAIN_VIEWPORT,
		DEBUG_VIEWPORT
	};

	enum DebugViewportTexture
	{
		STANDARD_VIEWPORT = 0,
		SHADOW_MAP = 1,

		NUM_DEBUG_VIEWPORTS
	};

	static void Init(uint32_t swapchainFrameNum);

	void Draw(int idx, int windowWidth, int windowHeight);

	void DrawViewport(int idx, int flags = 0);
	void DrawDebugViewport(int idx, int flags = 0);

	void DrawViewportSelectorMenu();

	static void CreateViewportTextures(const vk::Sampler& sampler,
	                                   const std::vector<Render::VulkanFramebuffer*>& framebuffers,
	                                   VkImageLayout layout);

	static void CreateDebugViewportTextures(const vk::Sampler& sampler,
	                                        const std::vector<Render::VulkanFramebuffer*>& framebuffers,
	                                        VkImageLayout layout, DebugViewportTexture debugViewportTexture);

	void UpdateViewportTextures(const Render::VulkanDevice& device, const vk::Sampler& sampler,
	                            const std::vector<Render::VulkanFramebuffer*>& framebuffers,
	                            vk::ImageLayout layout);

	[[nodiscard]] static bool PositionIsInViewport(float xPos, float yPos);

	[[nodiscard]] static uint32_t GetWidth() { return gameViewport.viewportWidth; }
	[[nodiscard]] static uint32_t GetHeight() { return gameViewport.viewportHeight; }
	[[nodiscard]] std::vector<float>& GetFrames() { return frames; }
	[[nodiscard]] static bool IsResized() { return gameViewport.isResized; }
	static void SetIsResized(const bool newIsResized) { gameViewport.isResized = newIsResized; }

	static GameViewport gameViewport;

private:
	GameViewport() = default;

	/* Main Viewport */
	bool isResized = false;
	uint32_t viewportWidth = 0;
	uint32_t viewportHeight = 0;

	std::vector<void*> viewportTextures;

	LibMath::Vector2 mainViewportMinPos;
	LibMath::Vector2 mainViewportMaxPos;

	std::vector<float> frames;

	/* Debug Viewport */
	DebugViewportTexture currentDebugViewport = STANDARD_VIEWPORT;
	const char* debugViewportTargetNames[NUM_DEBUG_VIEWPORTS] =
		{"Standard", "Directional Shadow Map"};

	bool renderDebugViewport = false;

	std::array<std::vector<void*>, NUM_DEBUG_VIEWPORTS> debugViewportTextures;
};
