#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>


#include "Texture.h"
#include "TextureImport.h"

#include "core/CLog.h"
#include "core/ResourceManager.h"
#include "Vertex.h"
#include "../../../render/sources/render/VulkanRenderer/VulkanRenderer.h"
#include "core/PoolAllocator.h"


namespace Model
{
	Model::Model(const char* path)
	{
		if (ResourceManager::GetResource<Model>(path) != nullptr)
			return;

		LoadModel(path);

		auto* model = Core::MemoryPool::Alloc<Model>();

		model->meshes.reserve(meshes.size());
		for (const auto& m : meshes)
		{
			std::vector<Vertex> vertexVector;
			vertexVector.reserve(m.vertices.size());
			for (const auto& v : m.vertices)
			{
				vertexVector.emplace_back(Vertex{v.position, v.normal, v.texCoords});
			}

			const std::vector<unsigned int>& indexVector = m.indices;

			model->meshes.emplace_back(Mesh{vertexVector, indexVector, m.materialName});
		}

		model->directory = directory;

		ResourceManager::AddResource<Model>(path, model);
	}

	void Model::LoadModel(const std::string& path)
	{
		Assimp::Importer import;

#ifdef _DEBUG
		const aiScene* scene = import.ReadFile(
			path, aiProcess_Triangulate | aiProcess_MakeLeftHanded/* | aiProcess_FlipWindingOrder*/);
#else
		const aiScene* scene = import.ReadFile(
			path, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_MakeLeftHanded);
#endif

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			LOG(LOG_ERROR, "Object " + path + " could not be imported: " + import.GetErrorString());
			return;
		}

		directory = path.substr(0, path.find_last_of('/'));

		LOG(LOG_INFO, "Assimp loaded model " + path);

		ProcessNode(scene->mRootNode, scene);
	}

	void Model::ProcessNode(aiNode* node, const aiScene* scene)
	{
		// process all the node's meshes (if any)
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(ProcessMesh(mesh, scene));
		}
		// then do the same for each of its children
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			LOG(LOG_INFO, "Processing child [" + std::to_string(i + 1) +
			    "/" + std::to_string(node->mNumChildren) + "]" +
			    node->mChildren[i]->mName.C_Str());

			ProcessNode(node->mChildren[i], scene);
		}
	}

	Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Vertex> vertices;
		vertices.reserve(mesh->mNumVertices);
		std::vector<unsigned int> indices;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex{};

			// process vertex positions, normals and texture coordinates
			vertex.position.x = mesh->mVertices[i].x;
			vertex.position.y = mesh->mVertices[i].y;
			vertex.position.z = mesh->mVertices[i].z;
			if (mesh->mNormals != nullptr)
			{
				vertex.normal.x = mesh->mNormals[i].x;
				vertex.normal.y = mesh->mNormals[i].y;
				vertex.normal.z = mesh->mNormals[i].z;
			}

			if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
				vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
			}
			else
				vertex.texCoords = LibMath::Vector2{0.0f};

			vertices.push_back(vertex);
		}

		indices.reserve(mesh->mNumFaces);

		// process indices
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		std::string matName;

		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			matName = LoadMaterialTextures(material);
		}


		return Mesh(vertices, indices, matName);
	}

	std::string Model::LoadMaterialTextures(aiMaterial* material) const
	{
		std::string path = directory + "/";

		std::string matName = material->GetName().C_Str();

		if (matName.empty() || matName == "DefaultMaterial")
			return defaultMaterialName;

		// Textures
		aiString str;
		if (material->GetTextureCount(aiTextureType_AMBIENT) >= 1)
		{
			material->GetTexture(aiTextureType_AMBIENT, 0, &str);
			path += str.C_Str();
			Render::VulkanRenderer::AddVulkanTextureToMaterial(matName, path, MaterialTextureLocation::AMBIENT);
		}

		path = directory + "/";

		if (material->GetTextureCount(aiTextureType_DIFFUSE) >= 1)
		{
			material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
			path += str.C_Str();
			Render::VulkanRenderer::AddVulkanTextureToMaterial(matName, path, MaterialTextureLocation::DIFFUSE);
		}

		path = directory + "/";

		if (material->GetTextureCount(aiTextureType_SPECULAR) >= 1)
		{
			material->GetTexture(aiTextureType_SPECULAR, 0, &str);
			path += str.C_Str();
			Render::VulkanRenderer::AddVulkanTextureToMaterial(matName, path, MaterialTextureLocation::SPECULAR);
		}

		path = directory + "/";

		if (material->GetTextureCount(aiTextureType_OPACITY) >= 1)
		{
			material->GetTexture(aiTextureType_OPACITY, 0, &str);
			path += str.C_Str();
			Render::VulkanRenderer::AddVulkanTextureToMaterial(matName, path, MaterialTextureLocation::ALPHA);
		}

		// Colors
		aiColor3D color;
		float fValue;
		LibMath::Vector3 value;

		material->Get(AI_MATKEY_COLOR_AMBIENT, color);
		value = LibMath::Vector3(color.r, color.g, color.b);
		Render::VulkanRenderer::AddValuesToMaterial(matName, value, MaterialValueLocation::AMBIENT_COLOR);

		material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		value = LibMath::Vector3(color.r, color.g, color.b);
		Render::VulkanRenderer::AddValuesToMaterial(matName, value, MaterialValueLocation::DIFFUSE_COLOR);

		material->Get(AI_MATKEY_COLOR_SPECULAR, color);
		value = LibMath::Vector3(color.r, color.g, color.b);
		Render::VulkanRenderer::AddValuesToMaterial(matName, value, MaterialValueLocation::SPECUALR_COLOR);

		material->Get(AI_MATKEY_SHININESS, fValue);
		value = LibMath::Vector3(fValue);
		Render::VulkanRenderer::AddValuesToMaterial(matName, value, MaterialValueLocation::SPECULAR_EXPONENT);

		material->Get(AI_MATKEY_OPACITY, fValue);
		value = LibMath::Vector3(fValue);
		Render::VulkanRenderer::AddValuesToMaterial(matName, value, MaterialValueLocation::ALPHA);

		return matName;
	}
}
