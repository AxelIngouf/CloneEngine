#pragma once

#include <string>
#include <vector>

namespace Model
{
	class Texture
	{
	public:
		Texture() = default;
		Texture(const Texture& other) = default;
		Texture& operator=(const Texture& other) = default;

		void LoadImage(const std::string& path, bool flipVertically = true);

		std::vector<unsigned char> data;
		int width = 0;
		int height = 0;
		int channels = 0;
	};
}
