#pragma once

#include "render/VulkanMacros.h"

namespace Render
{
	class VulkanFramebuffer;
	class VulkanLightBuffers;
	class VulkanTextureSampler;
	struct Material;
	class VulkanModelMatrix;
	class VulkanUniformBuffer;
	class VulkanDevice;

	class VulkanDescriptor
	{
	public:

		// Create
		void CreatePerFrameDescriptor(const VulkanDevice& device,
		                              const std::vector<VulkanUniformBuffer*>& uniformBuffers,
		                              const VulkanTextureSampler& textureSampler,
		                              vk::ImageView cubemapView);

		void CreateLightingDescriptor(const VulkanDevice& device,
		                              const VulkanLightBuffers& lightBuffers,
		                              const std::vector<VulkanUniformBuffer*>& directionalUbo,
		                              const VulkanTextureSampler& textureSampler,
		                              const std::vector<VulkanFramebuffer*>& depthBuffers);

		void CreatePerObjectDescriptor(const VulkanDevice& device, Material& material,
		                               const VulkanTextureSampler& textureSampler);

		void CreateDepthDescriptor(const VulkanDevice& device,
		                           const std::vector<VulkanUniformBuffer*>& uniformBuffers);

		// Update
		void UpdatePerFrameDescriptors(const VulkanDevice& device,
		                               const std::vector<VulkanUniformBuffer*>& uniformBuffers);

		void UpdateLightingDescriptors(const VulkanDevice& device, const VulkanLightBuffers& lightBuffers,
		                               const std::vector<VulkanUniformBuffer*>& directionalUbo,
		                               const VulkanTextureSampler& textureSampler,
		                               const std::vector<VulkanFramebuffer*>& depthBuffers);

		void UpdatePerObjectDescriptors(const VulkanDevice& device, Material& material,
		                                const VulkanTextureSampler& textureSampler);

		void UpdateDepthDescriptors(const VulkanDevice& device,
		                            const std::vector<VulkanUniformBuffer*>& uniformBuffers);

		[[nodiscard]] const vk::DescriptorSetLayout& GetDescriptorSetLayout() const;

		[[nodiscard]] const vk::DescriptorPool& GetDescriptorPool() const;

		[[nodiscard]] const vk::DescriptorSet& GetDescriptorSet(size_t idx) const;


	private:
		vk::UniqueDescriptorSetLayout descriptorSetLayout;
		vk::UniqueDescriptorPool descriptorPool;
		std::vector<vk::UniqueDescriptorSet> descriptorSets;
	};
}
