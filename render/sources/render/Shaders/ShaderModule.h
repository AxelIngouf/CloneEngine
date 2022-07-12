#pragma once

#include "render/VulkanMacros.h"

namespace Render
{
	class VulkanDevice;

	struct ShaderModule
	{
		void Initialize(const VulkanDevice& device, const std::string& filename, vk::ShaderStageFlagBits shaderStage);

		vk::PipelineShaderStageCreateInfo stageInfo{};
		vk::UniqueShaderModule module;

	private:
		// File name with no path or extension. 
		static std::vector<char> ReadShaderFile(const std::string& filename);
	};
}
