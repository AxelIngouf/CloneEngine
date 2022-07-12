#include "VulkanUniformBuffer.h"

#include "render/VulkanMacros.h"
#include "render/Camera/CameraComponent.h"
#include "render/Camera/FreeCam.h"
#include "render/VulkanBuffer/VulkanBuffer.h"
#include "render/VulkanDevice/VulkanDevice.h"
#include "render/VulkanRenderer/VulkanRenderer.h"

namespace Render
{
	VulkanUniformBuffer::VulkanUniformBuffer(VulkanDevice& device)
	{
		buffer.Initialize(device, sizeof(ubo), vk::BufferUsageFlagBits::eUniformBuffer,
		                  vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
	}

	void VulkanUniformBuffer::Update(const VulkanDevice& device)
	{
		ubo.viewPos = viewProjMatrixStorage.viewPos;
		ubo.viewProj = viewProjMatrixStorage.proj * viewProjMatrixStorage.view;

		void* data = device->mapMemory(buffer.GetBufferMemory(), 0, sizeof(ubo)).value;
		memcpy(data, &ubo, sizeof(ubo));
		device->unmapMemory(buffer.GetBufferMemory());
	}

	void VulkanUniformBuffer::UpdateViewMatrix()
	{
		viewProjMatrixStorage.view = VulkanRenderer::GetCameraLookAt();
		viewProjMatrixStorage.viewPos = LibMath::Vector4::Point(VulkanRenderer::GetCameraPosition());
	}

	void VulkanUniformBuffer::UpdateProjectionMatrix(const vk::Extent2D viewportSize)
	{
		const float aspectRatio = (float)viewportSize.width /
			(float)viewportSize.height;

		viewProjMatrixStorage.proj = LibMath::Matrix4::Perspective(
			LibMath::Degree(45),
			aspectRatio,
			0.1f,
			1000.f);

		viewProjMatrixStorage.proj[1][1] *= -1;
	}
}
