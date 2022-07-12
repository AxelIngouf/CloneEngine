#include "SceneNode.h"

#include <fstream>
#include <stack>

#include "../ECS/World.h"

namespace Core
{
	SceneNode::SceneNode()
	{
		parent = nullptr;

		local.position = LibMath::Vector3(0.f, 0.f, 0.f);
		local.rotation = LibMath::Quaternion(0.f, 0.f, 0.f, 1.f);
		local.scale = LibMath::Vector3(1.f, 1.f, 1.f);

		worldTransformIsDirty = true;
	}

	SceneNode::SceneNode(SceneNode&& other) noexcept
	{
		parent = other.parent;
		children = std::move(other.children);

		local = other.local;
		name = other.name;
		entityHandle = other.entityHandle;
		other.entityHandle = EntityHandle();

		//float boundingSphere; // todo:
		worldTransformIsDirty = true;
	}

	SceneNode& SceneNode::operator=(SceneNode&& other) noexcept
	{
		parent = other.parent;
		children = std::move(other.children);

		local = other.local;
		name = other.name;
		entityHandle = other.entityHandle;
		other.entityHandle = EntityHandle();

		//float boundingSphere; // todo:
		worldTransformIsDirty = true;

		return *this;
	}

	void SceneNode::ReParent(SceneNode* newParent)
	{
		if (parent)
		{
			if (parent == newParent)
			{
				return;
			}

			parent->RemoveChild(this);
		}

		parent = newParent;

		if (parent)
		{
			parent->children.push_back(this);
		}
	}

	SceneNode* SceneNode::CreateRoot(EntityCreationFunction function)
	{
		SceneNode* root = new SceneNode();

		root->parent = nullptr;
		root->entityHandle = function(root)->GetHandle();
		root->worldTransformIsDirty = true;
		root->name = "Root";

		return root;
	}

	SceneNode* SceneNode::CreateChild(EntityCreationFunction function)
	{
		SceneNode* child = new SceneNode();

		child->parent = this;
		child->entityHandle = function(child)->GetHandle();
		child->name = "Node";

		children.push_back(child);

		if (World::IsInPlay())
		{
			for (auto* component : Entity::GetEntity(child->entityHandle)->GetAllComponents())
			{
				component->BeginPlay();
			}
		}

		return child;
	}

	bool SceneNode::Destroy(bool andDescendants)
	{
		if (entityHandle.IsNotValid()) // already destroy
		{
			return false;
		}

		// clean up children
		for (SceneNode* child : children)
		{
			if (andDescendants == false
				&& parent)
			{
				child->parent = parent;
			}
			else
			{
				child->parent = nullptr;
				child->Destroy();
			}
		}

		// destroy node
		if (parent)
		{
			parent->RemoveChild(this);
		}

		EntityHandle temp = entityHandle;
		entityHandle = EntityHandle(); // flag this as already destroy

		Entity::Destroy(temp); // can call [SceneNode::Destroy(...)]
		
		delete this;

		return true;
	}

	void SceneNode::Translate(LibMath::Vector3 translation)
	{
		worldTransformIsDirty = true;
		local.position += translation;
	}

	void SceneNode::Rotate(LibMath::Quaternion rotation)
	{
		worldTransformIsDirty = true;
		local.rotation *= rotation;
	}

	void SceneNode::Scale(LibMath::Vector3 scale)
	{
		worldTransformIsDirty = true;
		local.scale *= scale;
	}

	void SceneNode::AddScale(LibMath::Vector3 scale)
	{
		worldTransformIsDirty = true;
		local.scale += scale;
	}

	void SceneNode::SetPosition(LibMath::Vector3 position)
	{
		worldTransformIsDirty = true;
		local.position = position;
	}

	void SceneNode::SetRotation(LibMath::Quaternion rotation)
	{
		worldTransformIsDirty = true;
		local.rotation = rotation;
	}

	void SceneNode::SetScale(LibMath::Vector3 scale)
	{
		worldTransformIsDirty = true;
		local.scale = scale;
	}

	void SceneNode::SetWorldPosition(LibMath::Vector3 position)
	{
		SetPosition(position - parent->GetWorldTransformCheck().position);
	}

	void SceneNode::SetWorldRotation(LibMath::Quaternion rotation)
	{
		SetRotation(parent->GetWorldTransformCheck().rotation.GetOffset(rotation));
	}

	void SceneNode::SetWorldScale(LibMath::Vector3 scale)
	{
		SetScale(scale / parent->GetWorldTransformCheck().scale);
	}

	const Transform& SceneNode::GetWorldTransformCheck()
	{
		std::stack<SceneNode*> ancestor;
		ancestor.push(this);

		SceneNode* oldestDirtyNode = worldTransformIsDirty ? this : nullptr;

		SceneNode* current = parent;
		while (current)
		{
			ancestor.push(current);
			if (current->worldTransformIsDirty)
			{
				oldestDirtyNode = current;
			}
			current = current->parent;
		}

		if (oldestDirtyNode)
		{
			while (ancestor.top() != oldestDirtyNode)
			{
				ancestor.pop();
			}

			while (!ancestor.empty())
			{
				oldestDirtyNode = ancestor.top();
				ancestor.pop();

				oldestDirtyNode->CleanWorldTransform();
			}
		}

		return world;
	}

	LibMath::Matrix4 SceneNode::GenerateWorldTransformMatrixCheck()
	{
		const Transform trans = GetWorldTransformCheck();

		LibMath::Matrix4 mat(1.f);

		mat = mat.Scale(trans.scale);
		mat = mat.Rotate(trans.rotation);
		mat = mat.Translate(trans.position);

		return mat;
	}

	LibMath::Matrix4 SceneNode::GenerateWorldTransformMatrixNoCheck() const
	{
		LibMath::Matrix4 mat(1.f);

		mat = mat.Scale(world.scale);
		mat = mat.Rotate(world.rotation);
		mat = mat.Translate(world.position);

		return mat;
	}

    bool SceneNode::IsDescendantOf(const SceneNode* other) const
    {
		const auto* tempSceneNode = this;

		while(tempSceneNode->GetParent() != nullptr)
		{
			tempSceneNode = tempSceneNode->GetParent();
			if(tempSceneNode == other)
			{
				return true;
			}
		}
		return false;
    }

    void SceneNode::CleanWorldTransform()
	{
		if (worldTransformIsDirty == false)
		{
			return;
		}

		Transform temp = parent ? parent->world + local : local;

		if (temp != world)
		{
			world = temp;

			for (SceneNode* child : children)
			{
				child->worldTransformIsDirty = true;
			}
		}

		worldTransformIsDirty = false;
		onCleaned.Broadcast();
	}

	void SceneNode::DeepCleanWorldTransform()
	{
		if (worldTransformIsDirty)
		{
			world = parent ? parent->world + local : local;
			worldTransformIsDirty = false;
			onCleaned.Broadcast();
		}

		for (SceneNode* child : children)
		{
			child->DeepCleanWorldTransformRecursive();
		}
	}

	void SceneNode::DeepCleanWorldTransformRecursive()
	{
		if (worldTransformIsDirty)
		{
			world = parent->world + local;
			worldTransformIsDirty = false;
			onCleaned.Broadcast();
		}

		for (SceneNode* child : children)
		{
			child->DeepCleanWorldTransformRecursive();
		}
	}

	bool SceneNode::RemoveChild(SceneNode* child)
	{
		for (int i = 0; i < children.size(); i++)
		{
			if (children[i] == child)
			{
				children[i] = children[children.size() - 1];
				children.pop_back();
				return true;
			}
		}

		return false;
	}
}
