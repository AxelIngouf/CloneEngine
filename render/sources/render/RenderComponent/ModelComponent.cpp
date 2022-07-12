#include "ModelComponent.h"

#include "core/PoolAllocator.h"
#include "core/ResourceManager.h"
#include "core/ECS/Entity.h"
#include "core/scenegraph/SceneNode.h"
#include "model/Model.h"
#include "render/VulkanConstants.h"
#include "render/VulkanMacros.h"
#include "render/VulkanBuffer/VulkanVertexBuffer.h"
#include "render/VulkanBuffer/VulkanIndexBuffer.h"
#include "render/VulkanDescriptor/VulkanDescriptor.h"
#include "render/VulkanPushConstant/VulkanPushConstant.h"
#include "render/VulkanRenderer/VulkanRenderer.h"
#include "render/Material/Material.h"

namespace Render
{
	ModelComponentInitParams::ModelComponentInitParams() :
		file(File(EFileType::MODEL))
	{
	}

	MeshSubComponent::MeshSubComponent(VulkanDevice& device, VulkanCommandPool& commandPool,
	                                   std::vector<Model::Vertex>& vertices, std::vector<unsigned>& indices,
	                                   const std::string& materialName)
	{
		vertexBuffer = Core::MemoryPool::Alloc<VulkanVertexBuffer>();
		vertexBuffer->Initialize(device, commandPool, vertices);
		indexBuffer = Core::MemoryPool::Alloc<VulkanIndexBuffer>(device, commandPool, indices);
		material = ResourceManager::GetResource<Material>(materialName);
	}

	MeshSubComponent::MeshSubComponent(MeshSubComponent&& other) noexcept
		: vertexBuffer(other.vertexBuffer), indexBuffer(other.indexBuffer), material(other.material)
	{
		other.vertexBuffer = nullptr;
		other.indexBuffer = nullptr;
		other.material = nullptr;
	}

	MeshSubComponent::MeshSubComponent(const MeshSubComponent& other)
		: vertexBuffer(other.vertexBuffer), indexBuffer(other.indexBuffer), material(other.material)
	{
	}

	MeshSubComponent::~MeshSubComponent()
	{
	}

	ModelComponent::ModelComponent(ModelComponent&& other) noexcept :
		Component<ModelComponent>(other), path(std::move(other.path)), material(other.material),
		meshes(std::move(other.meshes)), modelMatrix(other.modelMatrix)
	{
		other.modelMatrix = nullptr;
	}

	void ModelComponent::Initialize(const void* params)
	{
		if (!params)
		{
			LOG(LOG_ERROR, "Model component initializer was not set in AddComponent().",
			    Core::ELogChannel::CLOG_RENDER);

			return;
		}

		path = ((const ModelComponentInitParams*)params)->file.path;
	}

	void ModelComponent::Constructor()
	{
		if (path.empty())
			return;

		VulkanRenderer::AddVulkanBuffersToModel(this, path);
	}

	void ModelComponent::Finalize()
	{
		for (auto& subComponent : meshes)
		{
			if (subComponent.vertexBuffer)
			{
				Core::MemoryPool::Free(subComponent.vertexBuffer);
				subComponent.vertexBuffer = nullptr;
			}

			if (subComponent.indexBuffer)
			{
				Core::MemoryPool::Free(subComponent.indexBuffer);
				subComponent.indexBuffer = nullptr;
			}
		}

		meshes.clear();

		if (modelMatrix)
		{
			Core::MemoryPool::Free(modelMatrix);
			modelMatrix = nullptr;
		}

		path.clear();

		material.materials.clear();
	}

	void ModelComponent::Draw(const vk::CommandBuffer& commandBuffer, const vk::PipelineLayout pipelineLayout, int idx,
	                          std::string& previousMaterialName)
	{
		vk::DeviceSize offsets[] = {0};

		if (meshes.empty())
			return;

		modelMatrix->model =
			Core::Entity::GetEntity(GetEntityHandle())->GetAnchor()->GenerateWorldTransformMatrixNoCheck();

		for (auto& mesh : meshes)
		{
			commandBuffer.bindVertexBuffers(0, 1, &mesh.vertexBuffer->GetBuffer(), offsets);

			commandBuffer.pushConstants(pipelineLayout,
			                            vk::ShaderStageFlagBits::eVertex, 0,
			                            sizeof(VulkanPushConstant),
			                            modelMatrix);

			if (!mesh.material)
			{
				if (previousMaterialName != Model::defaultMaterialName)
				{
					commandBuffer.bindDescriptorSets(
						vk::PipelineBindPoint::eGraphics,
						pipelineLayout, 2, 1,
						&(ResourceManager::GetResource<Material>(Model::defaultMaterialName)->
							GetDescriptorSet(idx)),
						0,
						nullptr);

					previousMaterialName = Model::defaultMaterialName;
				}
			}
			else
			{
				if (previousMaterialName != mesh.material->name)
				{
					commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
					                                 pipelineLayout, 2, 1,
					                                 &(mesh.material->GetDescriptorSet(idx)), 0,
					                                 nullptr);

					previousMaterialName = mesh.material->name;
				}
			}

			if (mesh.indexBuffer->GetIndexCount() == 0 ||
				mesh.indexBuffer->GetIndexCount() == mesh.vertexBuffer->GetVertexCount())
			{
				commandBuffer.draw(mesh.vertexBuffer->GetVertexCount(), 1, 0, 0);
			}
			else
			{
				commandBuffer.bindIndexBuffer(mesh.indexBuffer->GetBuffer(), 0, vk::IndexType::eUint32);
				commandBuffer.drawIndexed(mesh.indexBuffer->GetIndexCount(), 1, 0, 0, 0);
			}
		}
	}

	void ModelComponent::DrawUntextured(const vk::CommandBuffer& commandBuffer, const vk::PipelineLayout pipelineLayout)
	{
		vk::DeviceSize offsets[] = {0};

		if (modelMatrix == nullptr) return;
		modelMatrix->model =
			Core::Entity::GetEntity(GetEntityHandle())->GetAnchor()->GenerateWorldTransformMatrixNoCheck();

		for (auto& mesh : (meshes))
		{
			commandBuffer.bindVertexBuffers(0, 1, &mesh.vertexBuffer->GetBuffer(), offsets);

			commandBuffer.pushConstants(pipelineLayout,
			                            vk::ShaderStageFlagBits::eVertex, 0,
			                            sizeof(VulkanPushConstant),
			                            modelMatrix);


			if (mesh.indexBuffer->GetIndexCount() == 0 ||
				mesh.indexBuffer->GetIndexCount() == mesh.vertexBuffer->GetVertexCount())
			{
				commandBuffer.draw(mesh.vertexBuffer->GetVertexCount(), 1, 0, 0);
			}
			else
			{
				commandBuffer.bindIndexBuffer(mesh.indexBuffer->GetBuffer(), 0, vk::IndexType::eUint32);
				commandBuffer.drawIndexed(mesh.indexBuffer->GetIndexCount(), 1, 0, 0, 0);
			}
		}
	}

	void ModelComponent::AddMaterialToModel(std::string materialName)
	{
		for (const auto* mat : material.materials)
		{
			if (mat != nullptr && mat->name == materialName)
				return;
		}

		material.materials.push_back(ResourceManager::GetResource<Material>(materialName));
	}

	void ModelComponent::UpdateMaterials(const std::string newMaterial)
	{
		material.materials.clear();

		auto newMat = ResourceManager::GetResource<Material>(newMaterial);

		for (MeshSubComponent& mesh : meshes)
			mesh.material = newMat;

		material.materials.push_back(newMat);
	}
}
