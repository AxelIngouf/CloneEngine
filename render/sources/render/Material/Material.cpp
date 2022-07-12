#include "Material.h"


#include "core/PoolAllocator.h"
#include "core/ResourceManager.h"
#include "render/VulkanConstants.h"
#include "model/Model.h"
#include "render/TextureImage/VulkanTextureImage.h"
#include "render/Image/Image.h"
#include "render/VulkanDescriptor/VulkanDescriptor.h"
#include "render/VulkanDevice/VulkanDevice.h"
#include "render/VulkanBuffer/VulkanBuffer.h"

namespace Render
{
	Material::Material() :
		specularExponent(23.f),
		alpha(1.f)
	{
	}

	Material::Material(VulkanDevice& vulkanDevice, VulkanTextureSampler& textureSampler)
	{
		ambientColor = LibMath::Vector3{0.2f, 0.2f, 0.2f};
		diffuseColor = LibMath::Vector3{0.55f, 0.55f, 0.55f};
		specularColor = LibMath::Vector3{0.1f, 0.1f, 0.1f};
		alpha = 1.f;
		specularExponent = 23.f;
		name = Model::defaultMaterialName;

		materialDescriptorValues.ambient = LibMath::Vector4::Point(ambientColor);
		materialDescriptorValues.diffuse = LibMath::Vector4::Point(diffuseColor);
		materialDescriptorValues.specular = LibMath::Vector4::Point(specularColor);
		materialDescriptorValues.shininess = specularExponent;
		materialDescriptorValues.alpha = alpha;
		materialDescriptorValues.ambientPresent = HasTexture(Model::MaterialTextureLocation::AMBIENT);
		materialDescriptorValues.diffusePresent = HasTexture(Model::MaterialTextureLocation::DIFFUSE);
		materialDescriptorValues.specularPresent = HasTexture(Model::MaterialTextureLocation::SPECULAR);
		materialDescriptorValues.alphaPresent = HasTexture(Model::MaterialTextureLocation::ALPHA);

		device = &vulkanDevice;
		sampler = &textureSampler;

		buffer = Core::MemoryPool::Alloc<VulkanBuffer>();
		buffer->Initialize(*device, sizeof(MaterialDescriptorValues), vk::BufferUsageFlagBits::eUniformBuffer,
		                   vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

		descriptor = Core::MemoryPool::Alloc<VulkanDescriptor>();
		descriptor->CreatePerObjectDescriptor(*device, *this, *sampler);

		Update();
	}

	const vk::ImageView& Material::GetImageView(const Model::MaterialTextureLocation& materialTextureLocation)
	{
		switch (materialTextureLocation)
		{
		case Model::MaterialTextureLocation::AMBIENT:
			if (ambientTexture)
				return ambientTexture->vulkanTextureImage->GetImageView();
			break;
		case Model::MaterialTextureLocation::DIFFUSE:
			if (diffuseTexture)
				return diffuseTexture->vulkanTextureImage->GetImageView();
			break;
		case Model::MaterialTextureLocation::SPECULAR:
			if (specularTexture)
				return specularTexture->vulkanTextureImage->GetImageView();
			break;
		case Model::MaterialTextureLocation::ALPHA:
			if (alphaTexture)
				return alphaTexture->vulkanTextureImage->GetImageView();
			break;
		}
		return ResourceManager::GetResource<VulkanTextureImage>(VulkanConstants::emptyTextureName)->GetImageView();
	}

	bool Material::HasTexture(const Model::MaterialTextureLocation& materialTextureLocation)
	{
		switch (materialTextureLocation)
		{
		case Model::MaterialTextureLocation::AMBIENT:
			if (ambientTexture)
				return true;
			break;
		case Model::MaterialTextureLocation::DIFFUSE:
			if (diffuseTexture)
				return true;
			break;
		case Model::MaterialTextureLocation::SPECULAR:
			if (specularTexture)
				return true;
			break;
		case Model::MaterialTextureLocation::ALPHA:
			if (alphaTexture)
				return true;
			break;
		}
		return false;
	}

	const vk::DescriptorSet& Material::GetDescriptorSet(const size_t idx)
	{
		return descriptor->GetDescriptorSet(idx);
	}

	void Material::SetTextureImage(Image* value, const Model::MaterialTextureLocation& materialTextureLocation)
	{
		switch (materialTextureLocation)
		{
		case Model::MaterialTextureLocation::AMBIENT:
			ambientTexture = value;
			materialDescriptorValues.ambientPresent = HasTexture(Model::MaterialTextureLocation::AMBIENT);
			break;
		case Model::MaterialTextureLocation::DIFFUSE:
			diffuseTexture = value;
			materialDescriptorValues.diffusePresent = HasTexture(Model::MaterialTextureLocation::DIFFUSE);
			break;
		case Model::MaterialTextureLocation::SPECULAR:
			specularTexture = value;
			materialDescriptorValues.specularPresent = HasTexture(Model::MaterialTextureLocation::SPECULAR);
			break;
		case Model::MaterialTextureLocation::ALPHA:
			alphaTexture = value;
			materialDescriptorValues.alphaPresent = HasTexture(Model::MaterialTextureLocation::ALPHA);
			break;
		}

		Update();
	}

	const Image* Material::GetTextureImage(const Model::MaterialTextureLocation& materialTextureLocation)
	{
		switch (materialTextureLocation)
		{
		case Model::MaterialTextureLocation::AMBIENT:
			return ambientTexture;
		case Model::MaterialTextureLocation::DIFFUSE:
			return diffuseTexture;
		case Model::MaterialTextureLocation::SPECULAR:
			return specularTexture;
		case Model::MaterialTextureLocation::ALPHA:
			return alphaTexture;
		}
		return nullptr;
	}

	void Material::Update()
	{
		descriptor->UpdatePerObjectDescriptors(*device, *this, *sampler);
		void* data = (*device)->mapMemory(buffer->GetBufferMemory(), 0, sizeof(MaterialDescriptorValues)).value;
		memcpy(data, &materialDescriptorValues, sizeof(MaterialDescriptorValues));
		(*device)->unmapMemory(buffer->GetBufferMemory());
	}

	void Material::SetName(std::string value)
	{
		if (!value.empty())
			name = value;
	}

	void Material::SetAmbientColor(LibMath::Vector3 value)
	{
		ambientColor = value;
		materialDescriptorValues.ambient = LibMath::Vector4::Point(ambientColor);
		Update();
	}

	void Material::SetDiffuseColor(LibMath::Vector3 value)
	{
		diffuseColor = value;
		materialDescriptorValues.diffuse = LibMath::Vector4::Point(diffuseColor);
		Update();
	}

	void Material::SetSpecularColor(LibMath::Vector3 value)
	{
		specularColor = value;
		materialDescriptorValues.specular = LibMath::Vector4::Point(specularColor);
		Update();
	}

	void Material::SetSpecularExponent(float value)
	{
		specularExponent = value;
		materialDescriptorValues.shininess = specularExponent;
		Update();
	}

	void Material::SetAlpha(float value)
	{
		alpha = value;
		materialDescriptorValues.alpha = alpha;
		Update();
	}

	const vk::Buffer& Material::GetBuffer()
	{
		return buffer->GetBuffer();
	}
}
