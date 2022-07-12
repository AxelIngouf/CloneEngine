#pragma once

#include <array>
#include <string>

namespace Model
{
	class Texture;
}

namespace Render
{
	class VulkanTextureImage;

	struct Image
	{
		Image() = default;
		Image(const Image& other) = default;
		Image(Image&& other) noexcept;
		~Image();

		Image& operator=(const Image& other) = default;

		static void InitializeImage(const std::string& path, bool flipVertically = true);

		std::string imagePath;

		Model::Texture* texture = nullptr;
		VulkanTextureImage* vulkanTextureImage = nullptr;
		void* imguiImage = nullptr;
	};

	struct CubemapImage
	{
		CubemapImage() = default;
		CubemapImage(const CubemapImage& other) = default;
		CubemapImage(CubemapImage&& other) noexcept;
		CubemapImage(const std::array<std::string, 6>& paths, bool flipVertically = true);

		CubemapImage& operator=(const CubemapImage& other) = default;

		static void InitializeImage(const std::array<std::string, 6>& paths, bool flipVertically = true);

		std::array<std::string, 6> imagePaths;

		std::array<Model::Texture*, 6> textures = {nullptr};
		VulkanTextureImage* cubemapTextureImage = nullptr;
		std::array<VulkanTextureImage*, 6> vulkanTextureImages = {nullptr};
		std::array<void*, 6> imguiImages = {nullptr};
	};
}
