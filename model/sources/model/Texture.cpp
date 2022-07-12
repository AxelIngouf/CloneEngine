#include "Texture.h"

#pragma warning(push, 0)

#define STB_IMAGE_IMPLEMENTATION
#include <STB/stb_image.h>

#pragma warning(pop)

#include "core/CLog.h"
#include "core/ResourceManager.h"

namespace Model
{
	void Texture::LoadImage(const std::string& path, const bool flipVertically)
	{
		LOG(LOG_INFO,
		    Core::CLog::FormatString("Importing texture: %s", path.c_str()).c_str(),
		    Core::ELogChannel::CLOG_MODEL);

		stbi_set_flip_vertically_on_load(flipVertically);

		stbi_uc* imgData = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);

		const size_t imageSize = (size_t)width * (size_t)height * (size_t)channels;

		if (!imgData)
		{
			LOG(LOG_ERROR,
			    Core::CLog::FormatString("Failed to import texture: %s", path.c_str()).c_str(),
			    Core::ELogChannel::CLOG_MODEL);
			return;
		}

		data.resize(imageSize);
		data.assign(imgData, imgData + imageSize);

		stbi_image_free(imgData);
	}
}
