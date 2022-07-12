#pragma once

#ifndef _SHIPPING
#include "../../imgui/EditorUI.h"
#endif
#include <mutex>
#include <vector>

#include "../VulkanRHI/VulkanRHI.h"
#include "Vector/Vector3.h"


namespace LibMath
{
	//struct Vector3;
	struct Matrix4;
}

namespace Model
{
	enum class MaterialTextureLocation;
    enum class MaterialValueLocation;
    class Texture;
}

namespace vk
{
	struct Extent2D;
}

struct VkSurfaceKHR_T;

namespace Render
{
	struct CubemapImage;
	class VulkanVertexBuffer;
	class VulkanTextureImage;
	class VulkanDescriptor;
	class VulkanLightBuffers;
	class VulkanTextureSampler;
	class FreeCam;
	struct ModelComponent;
	class VulkanCommandPool;
	class VulkanQueryPool;
	class VulkanPipeline;
	class VulkanSwapchain;
	class VulkanFramebuffer;
	class VulkanUniformBuffer;

	class VulkanRenderer
	{
	public:
		bool InitializeRHI(CreationParams& instanceParams, class EngineWindow& window);

		~VulkanRenderer();

		void DrawFrame(float deltaTime);

		void WaitIdle() const;

		static FreeCam& GetCamera() { return *camera; }
		[[nodiscard]] bool& GetWindowResized() { return windowResized; }

		// Once delegates, now statics
		[[nodiscard]] static VulkanTextureImage* LoadVulkanTexture(const Model::Texture& texture);
		[[nodiscard]] static VulkanTextureImage* LoadVulkanCubemapTexture(
			const std::array<Model::Texture*, 6>& textures);
		[[nodiscard]] static void* LoadImGuiTexture(const VulkanTextureImage& vulkanTextureImage);
		static void AddVulkanTextureToMaterial(const std::string& matName, const std::string& texturePath,
		                                       Model::MaterialTextureLocation materialTextureLocation);
		static void AddValuesToMaterial(const std::string& matName, const LibMath::Vector3& value,
		                                Model::MaterialValueLocation materialValueLocation);
		static void AddVulkanBuffersToModel(ModelComponent* model, const std::string& path);
		static void UpdateLightBuffers();
		static void RefreshShaders();
		static void TogglePhysXDebug(bool value);

		[[nodiscard]] static LibMath::Matrix4 GetCameraLookAt();
		[[nodiscard]] static LibMath::Vector3 GetCameraPosition();
		[[nodiscard]] static LibMath::Vector3 GetCameraFront();
		[[nodiscard]] static LibMath::Vector3 GetCameraRight();
		[[nodiscard]] static LibMath::Vector3 GetCameraUp();

	private:

		void CreateSwapchainFramebuffers();
		void FillProjectionMatrices(float aspectRatio);
		void CreateSwapchainUniformBuffers();
		void CreateDirectionalLightBuffers();
		void UpdateDirectionalLightBuffers(float aspectRatio);
		void RecreateSwapchain();
		void RecreateSwapchainUniformBuffers(const vk::Extent2D& viewportSize);

		void HandleEditorMousePosition(float deltaTime);
		vk::Extent2D HandleViewportResize();

		VulkanRHI rhi = {};
		class VulkanDevice* graphicsDevice = nullptr;
		VulkanSwapchain* swapchain = nullptr;
		VulkanPipeline* pipeline = nullptr;
		std::vector<VulkanCommandPool*> commandPool;
		VulkanCommandPool* singleUsePool = nullptr;

#ifndef _SHIPPING
		EditorUI editorUI;
#endif

		std::mutex renderMutex;

		std::vector<VulkanUniformBuffer*> uniformBuffers;
		std::vector<VulkanUniformBuffer*> directionalLightBuffers;
		VulkanLightBuffers* lightBuffers = nullptr;

		VulkanTextureSampler* sampler = nullptr;

		std::vector<VulkanFramebuffer*> offscreenFramebuffers;
		std::vector<VulkanFramebuffer*> depthFramebuffers;
		std::vector<VulkanFramebuffer*> swapchainFramebuffers;

		bool windowResized = false;

		EngineWindow* engineWindow = nullptr;

		VkSurfaceKHR_T* presentSurface = nullptr;

		inline static FreeCam* camera = nullptr;

		VulkanQueryPool* queryPool = nullptr;

		bool mouseCaptured = false;

		float viewportAspectRatio = 0.f;
		float viewportSizeX = 0.f;
		float viewportSizeY = 0.f;

		VulkanDescriptor* descriptors = nullptr;
		VulkanDescriptor* depthDescriptors = nullptr;
		VulkanDescriptor* lightDescriptors = nullptr;

		CubemapImage* cubemapImage = nullptr;

		VulkanVertexBuffer* debugLines = nullptr;
	};

	static VulkanRenderer* s_renderer;
}
