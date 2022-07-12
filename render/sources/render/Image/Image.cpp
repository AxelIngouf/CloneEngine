#include "Image.h"

#include "core/PoolAllocator.h"
#include "core/ResourceManager.h"
#include "model/Texture.h"
#include "render/VulkanRenderer/VulkanRenderer.h"
#include "render/VulkanMacros.h"
#include "render/TextureImage/VulkanTextureImage.h"

Render::Image::Image(Image&& other) noexcept:
	imagePath(std::move(other.imagePath)), texture(other.texture), vulkanTextureImage(other.vulkanTextureImage),
	imguiImage(other.imguiImage)
{
	other.texture = nullptr;
	other.vulkanTextureImage = nullptr;
	other.imguiImage = nullptr;
}

Render::Image::~Image()
{
	if (texture)
		Core::MemoryPool::Free(texture);

	if (vulkanTextureImage)
		Core::MemoryPool::Free(vulkanTextureImage);

	// imgui texture is cleaned up in the shutdown for imgui
}

void Render::Image::InitializeImage(const std::string& path, const bool flipVertically)
{
	if (ResourceManager::GetResource<Image>(path))
		return;

	auto* tempImage = Core::MemoryPool::Alloc<Image>();

	// Raw texture
	tempImage->texture = Core::MemoryPool::Alloc<Model::Texture>();
	tempImage->texture->LoadImage(path, flipVertically);

	// Vk texture
	tempImage->vulkanTextureImage = VulkanRenderer::LoadVulkanTexture(*tempImage->texture);

	// ImGui texture
	tempImage->imguiImage = VulkanRenderer::LoadImGuiTexture(*tempImage->vulkanTextureImage);

	tempImage->imagePath = path;

	ResourceManager::AddResource(path, tempImage);
}

Render::CubemapImage::CubemapImage(CubemapImage&& other) noexcept :
	imagePaths(std::move(other.imagePaths)),
	textures(other.textures),
	cubemapTextureImage(other.cubemapTextureImage),
	vulkanTextureImages(other.vulkanTextureImages),
	imguiImages(other.imguiImages)
{
	for (int i = 0; i < 6; i++)
	{
		other.textures[i] = nullptr;
		other.vulkanTextureImages[i] = nullptr;
		other.imguiImages[i] = nullptr;
	}

	other.cubemapTextureImage = nullptr;
}

Render::CubemapImage::CubemapImage(const std::array<std::string, 6>& paths, const bool flipVertically)
{
	InitializeImage(paths, flipVertically);
	*this = *ResourceManager::GetResource<CubemapImage>(paths[0]);
}

void Render::CubemapImage::InitializeImage(const std::array<std::string, 6>& paths, bool flipVertically)
{
	if (ResourceManager::GetResource<CubemapImage>(paths[0]))
		return;

	auto* tempImage = Core::MemoryPool::Alloc<CubemapImage>();


	for (int i = 0; i < 6; i++)
	{
		tempImage->textures[i] = Core::MemoryPool::Alloc<Model::Texture>();
		tempImage->textures[i]->LoadImage(paths[i], flipVertically);

		if (tempImage->textures[i]->channels == 0 ||
			tempImage->textures[i]->width == 0 ||
			tempImage->textures[i]->height == 0 ||
			tempImage->textures[i]->data.empty())
			return;


		tempImage->vulkanTextureImages[i] = VulkanRenderer::LoadVulkanTexture(*tempImage->textures[i]);

		tempImage->imguiImages[i] = VulkanRenderer::LoadImGuiTexture(*tempImage->vulkanTextureImages[i]);

		tempImage->imagePaths[i] = paths[i];
	}

	tempImage->cubemapTextureImage = VulkanRenderer::LoadVulkanCubemapTexture(tempImage->textures);


	ResourceManager::AddResource("CUBEMAP_" + paths[0], tempImage);
}
