#pragma once
#include <filesystem>
#include <vector>

#include "core/reflection/StructureField.h"

#define INPUT_STEP 0.1f
#define FIELD_NAME_SIZE 115

class UITheme;

namespace Model
{
	enum class MaterialTextureLocation;
}

namespace Render
{
	struct Material;
	class PointLight;
	class SpotLight;
	class DirectionalLight;
}

namespace Sound
{
	class Sound2D;
}

namespace std
{
	namespace filesystem
	{
		class path;
	}
}

namespace Physics
{
	class PhysicsVehicleActor;
	class PhysicsRigidStatic;
	class PhysicsRigidDynamic;
}

namespace Core
{
	class ComponentHandle;
	struct Field;
	class SceneNode;
	struct Type;
}

struct File;

class EditorUI
{
public:
	EditorUI();
	~EditorUI();

	void Init();

	void Draw(int idx);

	/* Style */
	void UpdateStyle();

private:
	static void PackageProject();
	void LoadProject();
	void SaveProject();

	void DrawMainMenuBar();
	void DrawToolsWindow();

	/* Debug window */
	void DrawDebugWindow() const;
	void FPSCounter() const;

	/* Inspector */
	static std::string GetID(Core::SceneNode* node);

	void DrawInspectorWindow();
	void DrawComponents() const;
	void DrawFields(Core::Structure* structure, void* instance) const;
	static void DrawFieldName(const std::string& fieldName);

	void CreateComponentSection();
	void ComponentCreation(const Core::Structure* selectedComponent) const;
	void ComponentDestroyButton(Core::ComponentHandle componentHandle) const;

	void InitializeParams(const Core::Structure* initParamsType);
	void* initParamsBuffer = nullptr;

	void DragDropFile(File* instance) const;

	void DragDropTexture(Render::Material* mat,
	                     Model::MaterialTextureLocation materialTextureLocation, int id) const;

	std::vector<const Core::Structure*> components;

	/* Scene Graph */
	void DrawSceneGraphWindow();
	void DrawSceneNode(Core::SceneNode* sceneNode);

	Core::SceneNode* currentSelectedNode = nullptr;

	/* File explorer */
	void ExploreFiles();
	void DrawExploredDirectories();
	void FreeCurrentSound();

	std::vector<std::filesystem::path> paths;

	float imageWidth = 90.f;
	float imageHeight = 90.f;
	float imagePadding = 25.f;
	int maxColumns = 8;
	size_t maxCharacters = 13;

	void* folderIcon = nullptr;
	void* fileIcon = nullptr;
	void* soundFileIcon = nullptr;
	void* soundPlayIcon = nullptr;
	void* soundPauseIcon = nullptr;
	void* levelIcon = nullptr;

	bool physXDebug = false;

	Sound::Sound2D* currentSound = nullptr;

	UITheme* uiTheme;

	bool needStyleUpdate = true;

	std::string currentLevelFile = "";
};
