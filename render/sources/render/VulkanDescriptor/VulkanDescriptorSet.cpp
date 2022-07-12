#include "VulkanDescriptorSet.h"


#include "model/Model.h"
#include "render/Material/Material.h"
#include "render/TextureImage/VulkanTextureImage.h"
#include "render/VulkanDevice/VulkanDevice.h"
#include "render/VulkanFramebuffer/VulkanFramebuffer.h"
#include "render/VulkanTextureSampler/VulkanTextureSampler.h"
#include "render/VulkanUniformBuffer/VulkanLightBuffers.h"
#include "render/VulkanUniformBuffer/VulkanUniformBuffer.h"

namespace Render
{
	std::mutex VulkanDescriptorSet::descriptorSetAllocationMutex;

	std::vector<vk::UniqueDescriptorSet> VulkanDescriptorSet::CreatePerFrameSets(const VulkanDevice& device,
		const vk::DescriptorSetLayout& layout, const vk::DescriptorPool& descriptorPool, const uint32_t count,
		const std::vector<VulkanUniformBuffer*>& uniformBuffers, const VulkanTextureSampler& textureSampler,
		const vk::ImageView cubemapView)
	{
		std::vector<vk::DescriptorSetLayout> layouts(count, layout);

		vk::DescriptorSetAllocateInfo allocInfo = {};
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = count;
		allocInfo.pSetLayouts = layouts.data();

		std::vector<vk::UniqueDescriptorSet> sets = device->allocateDescriptorSetsUnique(allocInfo).value;

		for (size_t i = 0; i < sets.size(); i++)
		{
			vk::DescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffers[i]->GetBuffer();
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			vk::DescriptorImageInfo imageInfo{};
			imageInfo.sampler = textureSampler.GetSampler();
			imageInfo.imageView = cubemapView;
			imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;


			std::array<vk::WriteDescriptorSet, 2> descriptorWrites;

			descriptorWrites[0].dstSet = sets[i].get();
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = vk::DescriptorType::eUniformBuffer;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			descriptorWrites[1].dstSet = sets[i].get();
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &imageInfo;

			device->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()),
			                             descriptorWrites.data(), 0, nullptr);
		}

		return sets;
	}

	std::vector<vk::UniqueDescriptorSet> VulkanDescriptorSet::CreateLightingSets(const VulkanDevice& device,
		const vk::DescriptorSetLayout& layout, const vk::DescriptorPool& descriptorPool, const uint32_t count,
		const VulkanLightBuffers& lightBuffers, const std::vector<VulkanUniformBuffer*>& directionalUbo,
		const VulkanTextureSampler& textureSampler,
		const std::vector<VulkanFramebuffer*>& depthBuffers)
	{
		std::vector<vk::DescriptorSetLayout> layouts(count, layout);

		vk::DescriptorSetAllocateInfo allocInfo = {};
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = count;
		allocInfo.pSetLayouts = layouts.data();

		std::vector<vk::UniqueDescriptorSet> sets = device->allocateDescriptorSetsUnique(allocInfo).value;

		for (uint32_t i = 0; i < count; i++)
		{
			vk::DescriptorBufferInfo directionalUBOInfo{};
			directionalUBOInfo.buffer = directionalUbo[i]->GetBuffer();
			directionalUBOInfo.offset = 0;
			directionalUBOInfo.range = sizeof(UniformBufferObject);

			vk::DescriptorBufferInfo directionalBufferInfo{};
			directionalBufferInfo.buffer = lightBuffers.GetBuffer();
			directionalBufferInfo.offset = 0;
			directionalBufferInfo.range = sizeof(VulkanLightBuffers::DirectionalLightStorage);

			vk::DescriptorImageInfo imageInfo{};
			imageInfo.sampler = textureSampler.GetSampler();
			imageInfo.imageView = depthBuffers[i]->GetImageView();
			imageInfo.imageLayout = vk::ImageLayout::eDepthStencilReadOnlyOptimal;

			//vk::DescriptorBufferInfo pointBufferInfo{};
			//pointBufferInfo.buffer = pointLightBuffer[i]->GetBuffer();
			//pointBufferInfo.offset = 0;
			//pointBufferInfo.range = sizeof(UniformBufferObject);

			//vk::DescriptorBufferInfo spotBufferInfo{};
			//spotBufferInfo.buffer = spotLightBuffer[i]->GetBuffer();
			//spotBufferInfo.offset = 0;
			//spotBufferInfo.range = sizeof(UniformBufferObject);

			std::array<vk::WriteDescriptorSet, 3> descriptorWrites;

			descriptorWrites[0].dstSet = sets[i].get();
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = vk::DescriptorType::eUniformBuffer;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &directionalUBOInfo;

			descriptorWrites[1].dstSet = sets[i].get();
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = vk::DescriptorType::eUniformBuffer;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pBufferInfo = &directionalBufferInfo;

			descriptorWrites[2].dstSet = sets[i].get();
			descriptorWrites[2].dstBinding = 2;
			descriptorWrites[2].dstArrayElement = 0;
			descriptorWrites[2].descriptorType = vk::DescriptorType::eCombinedImageSampler;
			descriptorWrites[2].descriptorCount = 1;
			descriptorWrites[2].pImageInfo = &imageInfo;

			//descriptorWrites[1].dstSet = sets[i].get();
			//descriptorWrites[1].dstBinding = 1;
			//descriptorWrites[1].dstArrayElement = 0;
			//descriptorWrites[1].descriptorType = vk::DescriptorType::eUniformBuffer;
			//descriptorWrites[1].descriptorCount = 1;
			//descriptorWrites[1].pBufferInfo = &pointBufferInfo;

			//descriptorWrites[2].dstSet = sets[i].get();
			//descriptorWrites[2].dstBinding = 2;
			//descriptorWrites[2].dstArrayElement = 0;
			//descriptorWrites[2].descriptorType = vk::DescriptorType::eUniformBuffer;
			//descriptorWrites[2].descriptorCount = 1;
			//descriptorWrites[2].pBufferInfo = &spotBufferInfo;

			device->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()),
			                             descriptorWrites.data(), 0, nullptr);
		}

		return sets;
	}

	std::vector<vk::UniqueDescriptorSet> VulkanDescriptorSet::CreatePerObjectSets(const VulkanDevice& device,
		const vk::DescriptorSetLayout& layout, const vk::DescriptorPool& descriptorPool, const uint32_t count,
		Material& material, const VulkanTextureSampler& textureSampler)
	{
		std::vector<vk::DescriptorSetLayout> layouts(count, layout);

		vk::DescriptorSetAllocateInfo allocInfo = {};
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = count;
		allocInfo.pSetLayouts = layouts.data();

		std::vector<vk::UniqueDescriptorSet> sets;
		{
			std::lock_guard<std::mutex> lock(descriptorSetAllocationMutex);
			sets = device->allocateDescriptorSetsUnique(allocInfo).value;
		}

		for (auto& s : sets)
		{
			vk::DescriptorImageInfo ambientImageInfo{};
			ambientImageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			ambientImageInfo.imageView = material.GetImageView(Model::MaterialTextureLocation::AMBIENT);
			ambientImageInfo.sampler = textureSampler.GetSampler();

			vk::DescriptorImageInfo diffuseImageInfo{};
			diffuseImageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			diffuseImageInfo.imageView = material.GetImageView(Model::MaterialTextureLocation::DIFFUSE);
			diffuseImageInfo.sampler = textureSampler.GetSampler();

			vk::DescriptorImageInfo specularImageInfo{};
			specularImageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			specularImageInfo.imageView = material.GetImageView(Model::MaterialTextureLocation::SPECULAR);
			specularImageInfo.sampler = textureSampler.GetSampler();

			vk::DescriptorImageInfo alphaImageInfo{};
			alphaImageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			alphaImageInfo.imageView = material.GetImageView(Model::MaterialTextureLocation::ALPHA);
			alphaImageInfo.sampler = textureSampler.GetSampler();

			vk::DescriptorBufferInfo materialBufferInfo{};
			materialBufferInfo.buffer = material.GetBuffer();
			materialBufferInfo.offset = 0;
			materialBufferInfo.range = sizeof(MaterialDescriptorValues);

			std::array<vk::WriteDescriptorSet, 5> descriptorWrites;

			descriptorWrites[0].dstSet = s.get();
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = vk::DescriptorType::eCombinedImageSampler;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pImageInfo = &ambientImageInfo;

			descriptorWrites[1].dstSet = s.get();
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pImageInfo = &diffuseImageInfo;

			descriptorWrites[2].dstSet = s.get();
			descriptorWrites[2].dstBinding = 2;
			descriptorWrites[2].dstArrayElement = 0;
			descriptorWrites[2].descriptorType = vk::DescriptorType::eCombinedImageSampler;
			descriptorWrites[2].descriptorCount = 1;
			descriptorWrites[2].pImageInfo = &specularImageInfo;

			descriptorWrites[3].dstSet = s.get();
			descriptorWrites[3].dstBinding = 3;
			descriptorWrites[3].dstArrayElement = 0;
			descriptorWrites[3].descriptorType = vk::DescriptorType::eCombinedImageSampler;
			descriptorWrites[3].descriptorCount = 1;
			descriptorWrites[3].pImageInfo = &alphaImageInfo;

			descriptorWrites[4].dstSet = s.get();
			descriptorWrites[4].dstBinding = 4;
			descriptorWrites[4].dstArrayElement = 0;
			descriptorWrites[4].descriptorType = vk::DescriptorType::eUniformBuffer;
			descriptorWrites[4].descriptorCount = 1;
			descriptorWrites[4].pBufferInfo = &materialBufferInfo;

			device->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()),
			                             descriptorWrites.data(), 0, nullptr);
		}

		return sets;
	}

	std::vector<vk::UniqueDescriptorSet> VulkanDescriptorSet::CreateDepthSets(const VulkanDevice& device,
	                                                                          const vk::DescriptorSetLayout& layout,
	                                                                          const vk::DescriptorPool& descriptorPool,
	                                                                          const uint32_t count,
	                                                                          const std::vector<VulkanUniformBuffer*>&
	                                                                          uniformBuffers)
	{
		std::vector<vk::DescriptorSetLayout> layouts(count, layout);

		vk::DescriptorSetAllocateInfo allocInfo = {};
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = count;
		allocInfo.pSetLayouts = layouts.data();

		std::vector<vk::UniqueDescriptorSet> sets = device->allocateDescriptorSetsUnique(allocInfo).value;

		for (size_t i = 0; i < sets.size(); i++)
		{
			vk::DescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffers[i]->GetBuffer();
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);


			std::array<vk::WriteDescriptorSet, 1> descriptorWrites;

			descriptorWrites[0].dstSet = sets[i].get();
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = vk::DescriptorType::eUniformBuffer;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;


			device->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()),
			                             descriptorWrites.data(), 0, nullptr);
		}

		return sets;
	}

	void VulkanDescriptorSet::UpdatePerFrameSet(const VulkanDevice& device,
	                                            const VulkanUniformBuffer& uniformBuffer,
	                                            const vk::DescriptorSet& set)
	{
		vk::DescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffer.GetBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		vk::WriteDescriptorSet descriptorWrite;
		descriptorWrite.dstSet = set;
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		device->updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
	}

	void VulkanDescriptorSet::UpdateLightingSet(const VulkanDevice& device, const VulkanLightBuffers& lightBuffers,
	                                            const VulkanUniformBuffer& directionalUbo,
	                                            const vk::DescriptorSet& set,
	                                            const VulkanTextureSampler& textureSampler,
	                                            const vk::ImageView shadowMap)
	{
		vk::DescriptorBufferInfo directionalUBOInfo{};
		directionalUBOInfo.buffer = directionalUbo.GetBuffer();
		directionalUBOInfo.offset = 0;
		directionalUBOInfo.range = sizeof(UniformBufferObject);

		vk::DescriptorBufferInfo directionalBufferInfo{};
		directionalBufferInfo.buffer = lightBuffers.GetBuffer();
		directionalBufferInfo.offset = 0;
		directionalBufferInfo.range = sizeof(VulkanLightBuffers::DirectionalLightStorage);

		vk::DescriptorImageInfo imageInfo{};
		imageInfo.sampler = textureSampler.GetSampler();
		imageInfo.imageView = shadowMap;
		imageInfo.imageLayout = vk::ImageLayout::eDepthStencilReadOnlyOptimal;

		std::array<vk::WriteDescriptorSet, 3> descriptorWrites;

		descriptorWrites[0].dstSet = set;
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = vk::DescriptorType::eUniformBuffer;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &directionalUBOInfo;

		descriptorWrites[1].dstSet = set;
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = vk::DescriptorType::eUniformBuffer;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pBufferInfo = &directionalBufferInfo;

		descriptorWrites[2].dstSet = set;
		descriptorWrites[2].dstBinding = 2;
		descriptorWrites[2].dstArrayElement = 0;
		descriptorWrites[2].descriptorType = vk::DescriptorType::eCombinedImageSampler;
		descriptorWrites[2].descriptorCount = 1;
		descriptorWrites[2].pImageInfo = &imageInfo;

		device->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()),
		                             descriptorWrites.data(), 0, nullptr);
	}

	void VulkanDescriptorSet::UpdatePerObjectSet(const VulkanDevice& device, Material& material,
	                                             const VulkanTextureSampler& textureSampler,
	                                             const vk::DescriptorSet& set)
	{
		vk::DescriptorImageInfo ambientImageInfo{};
		ambientImageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		ambientImageInfo.imageView = material.GetImageView(Model::MaterialTextureLocation::AMBIENT);
		ambientImageInfo.sampler = textureSampler.GetSampler();

		vk::DescriptorImageInfo diffuseImageInfo{};
		diffuseImageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		diffuseImageInfo.imageView = material.GetImageView(Model::MaterialTextureLocation::DIFFUSE);
		diffuseImageInfo.sampler = textureSampler.GetSampler();

		vk::DescriptorImageInfo specularImageInfo{};
		specularImageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		specularImageInfo.imageView = material.GetImageView(Model::MaterialTextureLocation::SPECULAR);
		specularImageInfo.sampler = textureSampler.GetSampler();

		vk::DescriptorImageInfo alphaImageInfo{};
		alphaImageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		alphaImageInfo.imageView = material.GetImageView(Model::MaterialTextureLocation::ALPHA);
		alphaImageInfo.sampler = textureSampler.GetSampler();

		vk::DescriptorBufferInfo materialBufferInfo{};
		materialBufferInfo.buffer = material.GetBuffer();
		materialBufferInfo.offset = 0;
		materialBufferInfo.range = sizeof(MaterialDescriptorValues);

		std::array<vk::WriteDescriptorSet, 5> descriptorWrites;

		descriptorWrites[0].dstSet = set;
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = vk::DescriptorType::eCombinedImageSampler;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pImageInfo = &ambientImageInfo;

		descriptorWrites[1].dstSet = set;
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &diffuseImageInfo;

		descriptorWrites[2].dstSet = set;
		descriptorWrites[2].dstBinding = 2;
		descriptorWrites[2].dstArrayElement = 0;
		descriptorWrites[2].descriptorType = vk::DescriptorType::eCombinedImageSampler;
		descriptorWrites[2].descriptorCount = 1;
		descriptorWrites[2].pImageInfo = &specularImageInfo;

		descriptorWrites[3].dstSet = set;
		descriptorWrites[3].dstBinding = 3;
		descriptorWrites[3].dstArrayElement = 0;
		descriptorWrites[3].descriptorType = vk::DescriptorType::eCombinedImageSampler;
		descriptorWrites[3].descriptorCount = 1;
		descriptorWrites[3].pImageInfo = &alphaImageInfo;

		descriptorWrites[4].dstSet = set;
		descriptorWrites[4].dstBinding = 4;
		descriptorWrites[4].dstArrayElement = 0;
		descriptorWrites[4].descriptorType = vk::DescriptorType::eUniformBuffer;
		descriptorWrites[4].descriptorCount = 1;
		descriptorWrites[4].pBufferInfo = &materialBufferInfo;

		device->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()),
		                             descriptorWrites.data(), 0, nullptr);
	}

	void VulkanDescriptorSet::UpdateDepthSet(const VulkanDevice& device, const VulkanUniformBuffer& uniformBuffer,
	                                         const vk::DescriptorSet& set)
	{
		vk::DescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffer.GetBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		vk::WriteDescriptorSet descriptorWrite;
		descriptorWrite.dstSet = set;
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		device->updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
	}
}
