#include "ShaderModule.h"


#include <filesystem>
#include <fstream>

#include "core/CLog.h"

#include "render/VulkanDevice/VulkanDevice.h"

namespace Render
{
	void ShaderModule::Initialize(const VulkanDevice& device, const std::string& filename,
	                              const vk::ShaderStageFlagBits shaderStage)
	{
		auto shaderCode = ReadShaderFile(filename);

		vk::ShaderModuleCreateInfo moduleCreateInfo{};
		moduleCreateInfo.codeSize = shaderCode.size();
		moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

		module = device->createShaderModuleUnique(moduleCreateInfo).value;
		ASSERT(module.get(), "Failed to create shader module. ", Core::ELogChannel::CLOG_RENDER);

		stageInfo.stage = shaderStage;
		stageInfo.module = module.get();
		stageInfo.pName = "main";
	}

	std::vector<char> ShaderModule::ReadShaderFile(const std::string& filename)
	{
		const std::string path = "render/sources/shaders/" + filename + ".spv";

		std::ifstream file(path, std::ios::ate | std::ios::binary);
		if (!file.is_open())
		{
			LOG(LOG_ERROR, std::filesystem::current_path().string(), Core::ELogChannel::CLOG_RENDER);
			LOG(LOG_ERROR, Core::CLog::FormatString("File \"%s\" could not be opened. ", path.c_str()).c_str(),
			    Core::ELogChannel::CLOG_RENDER);
			return std::vector<char>();
		}

		const size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}
}
