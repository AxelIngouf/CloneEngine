#pragma once
#include <string>

namespace Core {
    class SceneNode;
}

namespace LibMath {
    struct Quaternion;
    struct Vector3;
}

void CleanScene();
void InitScene();

void CheckpointsDemoScene();

void PlatformsDemoScene();

void PhysicsDemoScene();

void DefaultDirectionLight();

void MainMenu();

void PauseMenu();

void BindEscInput();
void UnbindEscInput();

void LoadModelDynamicPhysics(Core::SceneNode* sceneNode, const std::string& name, const std::string& path, const LibMath::Vector3& location, const LibMath::Quaternion& rotation);
void LoadModelStaticPhysics(const std::string& name, const std::string& path, const LibMath::Vector3& location, const LibMath::Quaternion& rotation);
void LoadModelSphereDynamicPhysics(Core::SceneNode* sceneNode, const std::string& name, const std::string& path, const LibMath::Vector3& location, float sphereRadius);