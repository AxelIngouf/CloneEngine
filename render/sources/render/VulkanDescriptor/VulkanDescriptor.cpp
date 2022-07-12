#include "VulkanDescriptor.h"

#include "render/VulkanConstants.h"

#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSet.h"
#include "VulkanDescriptorSetLayout.h"
#include "render/VulkanFramebuffer/VulkanFramebuffer.h"
#include "render/VulkanUniformBuffer/VulkanLightBuffers.h"
#include "render/VulkanUniformBuffer/VulkanUniformBuffer.h"

namespace Render
{
	void VulkanDescriptor::CreatePerFrameDescriptor(const VulkanDevice& device,
	                                                const std::vector<VulkanUniformBuffer*>& uniformBuffers,
	                                                const VulkanTextureSampler& textureSampler,
	                                                const vk::ImageView cubemapView)
	{
		descriptorSetLayout = VulkanDescriptorSetLayout::CreatePerFrameLayout(device);
		descriptorPool = VulkanDescriptorPool::CreatePerFramePool(device);
		descriptorSets = VulkanDescriptorSet::CreatePerFrameSets(device,
		                                                         descriptorSetLayout.get(),
		                                                         descriptorPool.get(),
		                                                         VulkanConstants::maxFramesInFlight,
		                                                         uniformBuffers, textureSampler, cubemapView);
	}

	void VulkanDescriptor::CreateLightingDescriptor(const VulkanDevice& device, const VulkanLightBuffers& lightBuffers,
	                                                const std::vector<VulkanUniformBuffer*>& directionalUbo,
	                                                const VulkanTextureSampler& textureSampler,
	                                                const std::vector<VulkanFramebuffer*>& depthBuffers)
	{
		descriptorSetLayout = VulkanDescriptorSetLayout::CreateLightingLayout(device);
		descriptorPool = VulkanDescriptorPool::CreateLightingPool(device);
		descriptorSets = VulkanDescriptorSet::CreateLightingSets(device,
		                                                         descriptorSetLayout.get(),
		                                                         descriptorPool.get(),
		                                                         VulkanConstants::maxFramesInFlight,
		                                                         lightBuffers, directionalUbo, textureSampler,
		                                                         depthBuffers);
	}

	void VulkanDescriptor::CreatePerObjectDescriptor(const VulkanDevice& device, Material& material,
	                                                 const VulkanTextureSampler& textureSampler)
	{
		descriptorSetLayout = VulkanDescriptorSetLayout::CreatePerObjectLayout(device);
		descriptorPool = VulkanDescriptorPool::CreatePerObjectPool(device);
		descriptorSets = VulkanDescriptorSet::CreatePerObjectSets(device,
		                                                          descriptorSetLayout.get(),
		                                                          descriptorPool.get(),
		                                                          VulkanConstants::maxFramesInFlight,
		                                                          material,
		                                                          textureSampler);
	}

	void VulkanDescriptor::CreateDepthDescriptor(const VulkanDevice& device,
	                                             const std::vector<VulkanUniformBuffer*>& uniformBuffers)
	{
		descriptorSetLayout = VulkanDescriptorSetLayout::CreateDepthLayout(device);
		descriptorPool = VulkanDescriptorPool::CreateDepthPool(device);
		descriptorSets = VulkanDescriptorSet::CreateDepthSets(device,
		                                                      descriptorSetLayout.get(),
		                                                      descriptorPool.get(),
		                                                      VulkanConstants::maxFramesInFlight,
		                                                      uniformBuffers);
	}

	void VulkanDescriptor::UpdatePerFrameDescriptors(const VulkanDevice& device,
	                                                 const std::vector<VulkanUniformBuffer*>& uniformBuffers)
	{
		for (size_t i = 0; i < descriptorSets.size(); i++)
		{
			VulkanDescriptorSet::UpdatePerFrameSet(device, *uniformBuffers[i], descriptorSets[i].get());
		}
	}

	void VulkanDescriptor::UpdateLightingDescriptors(const VulkanDevice& device, const VulkanLightBuffers& lightBuffers,
	                                                 const std::vector<VulkanUniformBuffer*>& directionalUbo,
	                                                 const VulkanTextureSampler& textureSampler,
	                                                 const std::vector<VulkanFramebuffer*>& depthBuffers)
	{
		for (size_t i = 0; i < descriptorSets.size(); i++)
		{
			VulkanDescriptorSet::UpdateLightingSet(device, lightBuffers, *directionalUbo[i], descriptorSets[i].get(),
			                                       textureSampler,
			                                       depthBuffers[i]->GetImageView());
		}
	}

	void VulkanDescriptor::UpdatePerObjectDescriptors(const VulkanDevice& device, Material& material,
	                                                  const VulkanTextureSampler& textureSampler)
	{
		for (auto& descriptorSet : descriptorSets)
		{
			VulkanDescriptorSet::UpdatePerObjectSet(device, material, textureSampler, descriptorSet.get());
		}
	}

	void VulkanDescriptor::UpdateDepthDescriptors(const VulkanDevice& device,
	                                              const std::vector<VulkanUniformBuffer*>& uniformBuffers)
	{
		for (size_t i = 0; i < descriptorSets.size(); i++)
		{
			VulkanDescriptorSet::UpdateDepthSet(device, *uniformBuffers[i], descriptorSets[i].get());
		}
	}

	const vk::DescriptorSetLayout& VulkanDescriptor::GetDescriptorSetLayout() const
	{
		return descriptorSetLayout.get();
	}

	const vk::DescriptorPool& VulkanDescriptor::GetDescriptorPool() const
	{
		return descriptorPool.get();
	}

	const vk::DescriptorSet& VulkanDescriptor::GetDescriptorSet(const size_t idx) const
	{
		return descriptorSets[idx].get();
	}
}
