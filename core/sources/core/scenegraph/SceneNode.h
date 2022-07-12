#pragma once

#include <vector>

#include "../Delegate.h"
#include "../Array.h"
#include "../ECS/Entity.h"
#include "../reflection/FuncPtrMeta.h"
#include "Transform.h"
#include "Matrix/Matrix4.h"

DELEGATE(Cleaned);

class Mesh;

namespace Core
{
    FUNCPTR(const Entity*, EntityCreationFunction, SceneNode*);

	class SceneNode
	{
	public:

		SceneNode(const SceneNode& other) = delete;
		SceneNode(SceneNode&& other) noexcept;
		~SceneNode() = default;

		SceneNode& operator=(const SceneNode& other) = delete;
		SceneNode& operator=(SceneNode&& other) noexcept;

		bool operator==(const SceneNode& other) const { return entityHandle == other.entityHandle; }

        [[nodiscard]] SceneNode*    GetParent() const { return parent; }
        void    ReParent(SceneNode* newParent);

        static SceneNode* CreateRoot(EntityCreationFunction function = &Entity::CreateEntity);
        SceneNode* CreateChild(EntityCreationFunction function = &Entity::CreateEntity);
		bool Destroy(bool andDescendants = true);
        ArrayView<SceneNode*> GetChildren() const { return ArrayView(children); }
        [[nodiscard]] size_t  GetChildrenCount() const { return children.size(); }

        [[nodiscard]] const Entity* GetEntity() const { return Entity::GetEntity(entityHandle); }

		[[nodiscard]] const std::string& GetName() const { return name; }
		void SetName(const std::string& newName) { name = newName; }

		void Translate(LibMath::Vector3 translation);
		void Rotate(LibMath::Quaternion rotation);
		void Scale(LibMath::Vector3 scale);
		void AddScale(LibMath::Vector3 scale);

		void SetPosition(LibMath::Vector3 position);
		void SetRotation(LibMath::Quaternion rotation);
		void SetScale(LibMath::Vector3 scale);

		void SetWorldPosition(LibMath::Vector3 position);
		void SetWorldRotation(LibMath::Quaternion rotation);
		void SetWorldScale(LibMath::Vector3 scale);

		[[nodiscard]] const Transform& GetLocalTransform() const { return local; }
		[[nodiscard]] const Transform& GetWorldTransformCheck();
		[[nodiscard]] const Transform& GetWorldTransformNoCheck() const { return world; }
		[[nodiscard]] LibMath::Matrix4 GenerateWorldTransformMatrixCheck();
		[[nodiscard]] LibMath::Matrix4 GenerateWorldTransformMatrixNoCheck() const;

		[[nodiscard]] bool IsDescendantOf(const SceneNode* other) const;

		void DeepCleanWorldTransform(); // doc: nocheck

        OnCleaned onCleaned;

	private:

		SceneNode();

		void CleanWorldTransform(); // doc: nocheck

        bool RemoveChild(SceneNode* child);
        void DeepCleanWorldTransformRecursive();

		SceneNode* parent = nullptr;
		std::vector<SceneNode*> children; // todo: replace vector<ptr> with vector<SceneNode>

		EntityHandle entityHandle;

		std::string name;

		Transform local;
		Transform world;

		LibMath::Matrix4 worldMatrix;

        //float boundingSphere; // todo:
        bool worldTransformIsDirty;
    };
}
