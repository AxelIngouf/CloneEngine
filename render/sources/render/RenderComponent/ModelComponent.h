#pragma once

#include "core/ECS/Component.h"
#include "core/Delegate.h"
#include "core/File.h"
#include "render/Material/Material.h"

namespace vk
{
	class CommandBuffer;
	class Sampler;
	class PipelineLayout;
}

namespace Model
{
	struct Vertex;
}


namespace Render
{
	class VulkanTextureSampler;
	struct VulkanPushConstant;
	class VulkanDescriptor;
	class VulkanDevice;
	class VulkanCommandPool;
	class VulkanVertexBuffer;
	class VulkanIndexBuffer;

	struct MeshSubComponent;

	STRUCT(ModelComponentInitParams,
		FIELD(File, file),
		SUPPLEMENT(
			EMPTY(),
			ModelComponentInitParams();,
			EMPTY()
		)
	);

	COMPONENT(ModelComponent,
		INIT_PARAM(ModelComponentInitParams),
		FUNCTION(void, Initialize, const void*, params),
		FUNCTION(void, Constructor),
		FUNCTION(void, Finalize),
		FUNCTION(void, AddMaterialToModel, std::string, materialName),
		FIELD(std::string, path, Core::FieldFlag::READONLY),
		FIELD(MaterialPointer, material),
		SUPPLEMENT(
			EMPTY(),
			ModelComponent() = default;
	        ModelComponent(ModelComponent&& other) noexcept;
			void Draw(const vk::CommandBuffer& commandBuffer, const vk::PipelineLayout pipelineLayout, int idx, std::string& previousMaterialName);
			void DrawUntextured(const vk::CommandBuffer& commandBuffer, const vk::PipelineLayout pipelineLayout);
			void UpdateMaterials(const std::string newMaterial);
			std::vector<MeshSubComponent> meshes;
			VulkanPushConstant* modelMatrix;,
		    EMPTY()
		)
	);

	struct MeshSubComponent
	{
		MeshSubComponent(VulkanDevice& device, VulkanCommandPool& commandPool, 
						 std::vector<Model::Vertex>& vertices, std::vector<unsigned>& indices, const std::string& materialName);

		MeshSubComponent(MeshSubComponent&& other) noexcept;
		MeshSubComponent(const MeshSubComponent& other);

		~MeshSubComponent();

		VulkanVertexBuffer* vertexBuffer = nullptr;
		VulkanIndexBuffer* indexBuffer = nullptr;

		Material* material = nullptr;
	};
}
