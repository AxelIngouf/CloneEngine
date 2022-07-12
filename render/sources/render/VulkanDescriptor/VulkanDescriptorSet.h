#pragma once

#include <mutex>

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

	class VulkanDescriptorSet
	{
	public:

		static std::vector<vk::UniqueDescriptorSet> CreatePerFrameSets(
			const VulkanDevice& device,
			const vk::DescriptorSetLayout& layout,
			const vk::DescriptorPool& descriptorPool,
			uint32_t count,
			const std::vector<VulkanUniformBuffer*>& uniformBuffers,
			const VulkanTextureSampler& textureSampler,
			vk::ImageView cubemapView);
		static std::vector<vk::UniqueDescriptorSet> CreateLightingSets(
			const VulkanDevice& device,
			const vk::DescriptorSetLayout& layout,
			const vk::DescriptorPool& descriptorPool,
			uint32_t count,
			const VulkanLightBuffers& lightBuffers,
			const std::vector<VulkanUniformBuffer*>& directionalUbo,
			const VulkanTextureSampler& textureSampler,
			const std::vector<VulkanFramebuffer*>& depthBuffers);


		static std::vector<vk::UniqueDescriptorSet> CreatePerObjectSets(
			const VulkanDevice& device, const vk::DescriptorSetLayout& layout,
			const vk::DescriptorPool& descriptorPool, uint32_t count,
			Material& material, const VulkanTextureSampler& textureSampler);

		static std::vector<vk::UniqueDescriptorSet> CreateDepthSets(
			const VulkanDevice& device,
			const vk::DescriptorSetLayout& layout,
			const vk::DescriptorPool& descriptorPool,
			uint32_t count,
			const std::vector<VulkanUniformBuffer*>& uniformBuffers);

		static void UpdatePerFrameSet(const VulkanDevice& device,
		                              const VulkanUniformBuffer& uniformBuffer,
		                              const vk::DescriptorSet& set);

		static void UpdateLightingSet(const VulkanDevice& device, const VulkanLightBuffers& lightBuffers,
		                              const VulkanUniformBuffer& directionalUbo,
		                              const vk::DescriptorSet& set,
		                              const VulkanTextureSampler& textureSampler,
		                              vk::ImageView shadowMap);

		static void UpdatePerObjectSet(const VulkanDevice& device, Material& material,
		                               const VulkanTextureSampler& textureSampler, const vk::DescriptorSet& set);

		static void UpdateDepthSet(const VulkanDevice& device,
		                           const VulkanUniformBuffer& uniformBuffer,
		                           const vk::DescriptorSet& set);

		static std::mutex descriptorSetAllocationMutex;
	};
}
