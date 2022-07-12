#include "VulkanDescriptorPool.h"


#include "imgui/imgui.h"
#include "render/VulkanDevice/VulkanDevice.h"
#include "render/VulkanConstants.h"

namespace Render
{
	vk::UniqueDescriptorPool VulkanDescriptorPool::imGuiPool;

	vk::UniqueDescriptorPool VulkanDescriptorPool::CreatePerFramePool(const VulkanDevice& device)
	{
		vk::DescriptorPoolSize poolSize{};
		poolSize.type = vk::DescriptorType::eUniformBuffer;
		poolSize.descriptorCount = static_cast<uint32_t>(VulkanConstants::maxFramesInFlight);

		vk::DescriptorPoolCreateInfo poolInfo = {};
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = static_cast<uint32_t>(VulkanConstants::maxFramesInFlight);
		poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

		return device->createDescriptorPoolUnique(poolInfo).value;
	}

	vk::UniqueDescriptorPool VulkanDescriptorPool::CreateLightingPool(const VulkanDevice& device)
	{
		vk::DescriptorPoolSize poolSize{};
		poolSize.type = vk::DescriptorType::eUniformBuffer;
		poolSize.descriptorCount = static_cast<uint32_t>(VulkanConstants::maxFramesInFlight);

		vk::DescriptorPoolCreateInfo poolInfo = {};
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = static_cast<uint32_t>(VulkanConstants::maxFramesInFlight);
		poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

		return device->createDescriptorPoolUnique(poolInfo).value;
	}

	vk::UniqueDescriptorPool VulkanDescriptorPool::CreateImguiPool(const VulkanDevice& device)
	{
		vk::DescriptorPoolSize poolSizes[] =
		{
			{vk::DescriptorType::eSampler, 1000},
			{vk::DescriptorType::eCombinedImageSampler, 1000},
			{vk::DescriptorType::eSampledImage, 1000},
			{vk::DescriptorType::eStorageImage, 1000},
			{vk::DescriptorType::eUniformTexelBuffer, 1000},
			{vk::DescriptorType::eStorageTexelBuffer, 1000},
			{vk::DescriptorType::eUniformBuffer, 1000},
			{vk::DescriptorType::eStorageBuffer, 1000},
			{vk::DescriptorType::eUniformBuffer, 1000},
			{vk::DescriptorType::eStorageBufferDynamic, 1000},
			{vk::DescriptorType::eInputAttachment, 1000}
		};
		vk::DescriptorPoolCreateInfo poolInfo = {};
		poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
		poolInfo.maxSets = 1000 * IM_ARRAYSIZE(poolSizes);
		poolInfo.poolSizeCount = (uint32_t)IM_ARRAYSIZE(poolSizes);
		poolInfo.pPoolSizes = poolSizes;


		return device->createDescriptorPoolUnique(poolInfo).value;
	}

	vk::UniqueDescriptorPool VulkanDescriptorPool::CreatePerObjectPool(const VulkanDevice& device)
	{
		std::array<vk::DescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(VulkanConstants::maxFramesInFlight);
		poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(VulkanConstants::maxFramesInFlight);

		vk::DescriptorPoolCreateInfo poolInfo = {};
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(VulkanConstants::maxFramesInFlight);
		poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

		return device->createDescriptorPoolUnique(poolInfo).value;
	}

	vk::UniqueDescriptorPool VulkanDescriptorPool::CreateDepthPool(const VulkanDevice& device)
	{
		std::array<vk::DescriptorPoolSize, 1> poolSizes{};
		poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(VulkanConstants::maxFramesInFlight);

		vk::DescriptorPoolCreateInfo poolInfo = {};
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(VulkanConstants::maxFramesInFlight);
		poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

		return device->createDescriptorPoolUnique(poolInfo).value;
	}
}
