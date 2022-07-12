#pragma once

#include <vector>


#include "Mesh.h"

#include "core/Delegate.h"

struct aiMaterial;
struct aiMesh;
struct aiScene;
struct aiNode;

namespace Model
{
	static constexpr const char* defaultMaterialName("DEFAULT_FALLBACK_MATERIAL");

	enum class MaterialTextureLocation
	{
		AMBIENT,
		DIFFUSE,
		SPECULAR,
		ALPHA
	};

	enum class MaterialValueLocation
	{
		AMBIENT_COLOR,
		DIFFUSE_COLOR,
		SPECUALR_COLOR,
		SPECULAR_EXPONENT,
		ALPHA
	};

	DELEGATE_Three_Params(MaterialTextureImport, const std::string&, materialName, const std::string&, texturePath,
	                      const MaterialTextureLocation&, materialTextureLocation)

	DELEGATE_Three_Params(MaterialValueImport, const std::string&, materialName, const LibMath::Vector3&, value,
	                      const MaterialValueLocation&, materialValueLocation)

	class Model
	{
	public:
		Model() = default;
		explicit Model(const char* path);

		explicit Model(std::vector<Mesh> _meshes):
			meshes(std::move(_meshes))
		{
		}

		std::vector<Mesh> meshes;

	private:
		std::string directory;

		void LoadModel(const std::string& path);
		void ProcessNode(aiNode* node, const aiScene* scene);
		Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
		std::string LoadMaterialTextures(aiMaterial* material) const;
	};
}
