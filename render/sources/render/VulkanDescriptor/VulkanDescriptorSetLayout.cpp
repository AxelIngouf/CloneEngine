#include "VulkanDescriptorSetLayout.h"

#include "render/VulkanDevice/VulkanDevice.h"

namespace Render
{
	vk::UniqueDescriptorSetLayout VulkanDescriptorSetLayout::CreatePerFrameLayout(const VulkanDevice& device)
	{
		std::vector<vk::DescriptorSetLayoutBinding> bindings(2);

		bindings[0].binding = 0;
		bindings[0].descriptorType = vk::DescriptorType::eUniformBuffer;
		bindings[0].descriptorCount = 1;
		bindings[0].stageFlags = vk::ShaderStageFlagBits::eVertex;

		bindings[1].binding = 1;
		bindings[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
		bindings[1].descriptorCount = 1;
		bindings[1].stageFlags = vk::ShaderStageFlagBits::eFragment;

		vk::DescriptorSetLayoutCreateInfo layoutInfo;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		return device->createDescriptorSetLayoutUnique(layoutInfo).value;
	}

	vk::UniqueDescriptorSetLayout VulkanDescriptorSetLayout::CreateLightingLayout(const VulkanDevice& device)
	{
		std::vector<vk::DescriptorSetLayoutBinding> bindings(3);

		bindings[0].binding = 0;
		bindings[0].descriptorType = vk::DescriptorType::eUniformBuffer;
		bindings[0].descriptorCount = 1;
		bindings[0].stageFlags = vk::ShaderStageFlagBits::eVertex;

		bindings[1].binding = 1;
		bindings[1].descriptorType = vk::DescriptorType::eUniformBuffer;
		bindings[1].descriptorCount = 1;
		bindings[1].stageFlags = vk::ShaderStageFlagBits::eFragment;

		bindings[2].binding = 2;
		bindings[2].descriptorType = vk::DescriptorType::eCombinedImageSampler;
		bindings[2].descriptorCount = 1;
		bindings[2].stageFlags = vk::ShaderStageFlagBits::eFragment;

		vk::DescriptorSetLayoutCreateInfo layoutInfo;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		return device->createDescriptorSetLayoutUnique(layoutInfo).value;
	}

	vk::UniqueDescriptorSetLayout VulkanDescriptorSetLayout::CreateImguiLayout(const VulkanDevice& device)
	{
		vk::DescriptorSetLayoutBinding samplerLayoutBinding = {};
		samplerLayoutBinding.binding = 0;
		samplerLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

		vk::DescriptorSetLayoutCreateInfo layoutInfo;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &samplerLayoutBinding;

		return device->createDescriptorSetLayoutUnique(layoutInfo).value;
	}

	vk::UniqueDescriptorSetLayout VulkanDescriptorSetLayout::CreatePerObjectLayout(const VulkanDevice& device)
	{
		vk::DescriptorSetLayoutBinding samplerLayoutBinding = {};
		samplerLayoutBinding.binding = 0;
		samplerLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;
		samplerLayoutBinding.pImmutableSamplers = nullptr;


		std::vector<vk::DescriptorSetLayoutBinding> bindings(4, samplerLayoutBinding);

		bindings[0].binding = 0;
		bindings[1].binding = 1;
		bindings[2].binding = 2;
		bindings[3].binding = 3;

		vk::DescriptorSetLayoutBinding materialLayoutBinding = {};
		materialLayoutBinding.binding = 4;
		materialLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
		materialLayoutBinding.descriptorCount = 1;
		materialLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eFragment;

		bindings.push_back(materialLayoutBinding);

		vk::DescriptorSetLayoutCreateInfo layoutInfo;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		return device->createDescriptorSetLayoutUnique(layoutInfo).value;
	}

	vk::UniqueDescriptorSetLayout VulkanDescriptorSetLayout::CreateDepthLayout(const VulkanDevice& device)
	{
		std::vector<vk::DescriptorSetLayoutBinding> bindings(1);

		bindings[0].binding = 0;
		bindings[0].descriptorType = vk::DescriptorType::eUniformBuffer;
		bindings[0].descriptorCount = 1;
		bindings[0].stageFlags = vk::ShaderStageFlagBits::eVertex;

		vk::DescriptorSetLayoutCreateInfo layoutInfo;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		return device->createDescriptorSetLayoutUnique(layoutInfo).value;
	}
}
