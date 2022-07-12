#pragma once
#include <string>

#include "model/Model.h"
#include "Vector/Vector3.h"
#include "core/ECS/Component.h"

namespace vk
{
	class ImageView;
	class Buffer;
	class DescriptorSet;
}

namespace Model
{
	enum class MaterialTextureLocation;
}

namespace Render
{
	class VulkanTextureSampler;
	class VulkanDescriptor;
	class VulkanTextureImage;
	class VulkanBuffer;
	class VulkanDevice;
	struct Image;
	
	struct MaterialDescriptorValues
	{
		LibMath::Vector4 ambient;
		LibMath::Vector4 diffuse;
		LibMath::Vector4 specular;

		float alpha = 1.f;
		float shininess = 0.f;

		uint32_t ambientPresent = false;
		uint32_t diffusePresent = false;
		uint32_t specularPresent = false;
		uint32_t alphaPresent = false;
	};

	STRUCT(Material,
		FUNCTION(void, SetName, std::string, value),
		FUNCTION(void, SetAmbientColor, LibMath::Vector3, value),
		FUNCTION(void, SetDiffuseColor, LibMath::Vector3, value),
		FUNCTION(void, SetSpecularColor, LibMath::Vector3, value),
		FUNCTION(void, SetSpecularExponent, float, value),
		FUNCTION(void, SetAlpha, float, value),
		FUNCTION(void, Update),
		FIELD(std::string, name, Core::FieldFlag::EDITOR_HIDDEN),
		FIELD(LibMath::Vector3, ambientColor),
		FIELD(LibMath::Vector3, diffuseColor),
		FIELD(LibMath::Vector3, specularColor),
		FIELD(float, specularExponent),
		FIELD(float, alpha),
		SUPPLEMENT(
			EMPTY(),
			Material();
			Material(VulkanDevice& device, VulkanTextureSampler& sampler);
			const vk::ImageView& GetImageView(const Model::MaterialTextureLocation& materialTextureLocation);
			bool HasTexture(const Model::MaterialTextureLocation& materialTextureLocation);
			const vk::DescriptorSet& GetDescriptorSet(size_t idx);
			void SetTextureImage(Image* value, const Model::MaterialTextureLocation& materialTextureLocation);
			const Image* GetTextureImage(const Model::MaterialTextureLocation& materialTextureLocation);
			const vk::Buffer& GetBuffer();
			Image* ambientTexture = nullptr;
			Image* diffuseTexture = nullptr;
			Image* specularTexture = nullptr;
			Image* alphaTexture = nullptr;
			VulkanDevice* device = nullptr;
			VulkanTextureSampler* sampler = nullptr;
			VulkanBuffer* buffer = nullptr;
			MaterialDescriptorValues materialDescriptorValues;
			VulkanDescriptor* descriptor = nullptr;,
			EMPTY()
		)
	);

	STRUCT(MaterialPointer,
		//FIELD(int, id = -1, Core::FieldFlag::EDITOR_HIDDEN | Core::FieldFlag::TRANSIENT),
		SUPPLEMENT(
		    EMPTY(),
		    std::vector<Material*> materials;,
		    EMPTY()
		)
	);
}
