#include "VulkanLightBuffers.h"

#include "render/VulkanMacros.h"
#include "render/Light/LightComponent.h"
#include "render/VulkanBuffer/VulkanBuffer.h"
#include "render/VulkanDevice/VulkanDevice.h"
#include "core/ECS/Entity.h"
#include "core/scenegraph/SceneNode.h"

namespace Render
{
	VulkanLightBuffers::VulkanLightBuffers(VulkanDevice& device)
	{
		buffer.Initialize(device, sizeof(DirectionalLightStorage), vk::BufferUsageFlagBits::eUniformBuffer,
		                  vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
	}

	void VulkanLightBuffers::Update(const VulkanDevice& device)
	{
		auto it = DirectionalLightComponent::GetAll();
		if (it.Next())
		{
			dirLight.direction = LibMath::Vector4::Direction(
				Core::Entity::GetEntity(
					it->GetEntityHandle())->GetAnchor()->GetWorldTransformNoCheck().rotation.EulerAngles());

			dirLight.ambient = LibMath::Vector4(it->ambientComponent) * it->ambientComponent.w;
			dirLight.diffuse = LibMath::Vector4(it->diffuseComponent) * it->diffuseComponent.w;
			dirLight.specular = LibMath::Vector4(it->specularComponent) * it->specularComponent.w;

			dirLight.isActive = it->GetIsActive();
		}

		void* data = device->mapMemory(buffer.GetBufferMemory(), 0, sizeof(dirLight)).value;
		memcpy(data, &dirLight, sizeof(dirLight));
		device->unmapMemory(buffer.GetBufferMemory());
	}
}
