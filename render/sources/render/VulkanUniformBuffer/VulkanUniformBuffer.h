#pragma once

#include "Matrix/Matrix4.h"
#include "render/VulkanBuffer/VulkanBuffer.h"


namespace vk
{
	class Buffer;
	struct Extent2D;
}

namespace Render
{
	class FreeCam;
	class VulkanSwapchain;
	class VulkanDevice;

	struct ViewProjMatrixStorage
	{
		LibMath::Matrix4 view;
		LibMath::Matrix4 proj;
		LibMath::Vector4 viewPos;
	};

	struct UniformBufferObject
	{
		LibMath::Matrix4 viewProj;
		LibMath::Vector4 viewPos;
	};

	class VulkanUniformBuffer
	{
	public:
		VulkanUniformBuffer(VulkanDevice& device);

		void Update(const VulkanDevice& device);

		void UpdateViewMatrix();
		void UpdateProjectionMatrix(vk::Extent2D viewportSize);

		[[nodiscard]] const vk::Buffer& GetBuffer() const { return buffer.GetBuffer(); }
		[[nodiscard]] ViewProjMatrixStorage& GetViewProj() { return viewProjMatrixStorage; }

	private:
		VulkanBuffer buffer;

		UniformBufferObject ubo;
		ViewProjMatrixStorage viewProjMatrixStorage;
	};
}
