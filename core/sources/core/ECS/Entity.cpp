#include "Entity.h"

#include "../Array.h"
#include "core/scenegraph/SceneNode.h"

namespace Core
{
	std::vector<Entity> Entity::all;
	int Entity::lastFreeIndex = Entity::Setup();
	std::vector<EntityDetail> Entity::entitiesDetails;

	const Entity* Entity::CreateEntity(SceneNode* anchor)
	{
		int newEntityIndex;
		if (lastFreeIndex == -1)
		{
			newEntityIndex = static_cast<int>(all.size());
			all.emplace_back();
		}
		else
		{
			newEntityIndex = lastFreeIndex;
			lastFreeIndex = all[lastFreeIndex].handle.GetValue();
		}

		all[newEntityIndex].handle = EntityHandle(newEntityIndex);
		all[newEntityIndex].anchor = anchor;
		all[newEntityIndex].isInUse = true;

		return &all[newEntityIndex];
	}

	bool Entity::Destroy()
	{
		if (handle.IsNotValid()) // todo: check only in debug 
		{
			LOG(LOG_WARNING, "Cannot destroy entity not created with Entity::CreateEntity()", ELogChannel::CLOG_ECS);
			return false;
		}

		if (isInUse == false) // already destroy
		{
			return false;
		}

		// destroy all entity's components
		int entitiesDetailsSize = (int)entitiesDetails.size();
		for (int i = 0; i < entitiesDetailsSize; ++i)
		{
			if (entitiesDetails[i].entityHandle == handle)
			{
				Destroy(entitiesDetails[i].componentHandle);

				entitiesDetails[i--] = entitiesDetails[entitiesDetails.size() - 1];

				entitiesDetails.pop_back();
				entitiesDetailsSize--;
			}
		}

		// destroy entity
		isInUse = false; // flag this as already destroy

		int temp = lastFreeIndex;
		lastFreeIndex = handle.GetValue();
		handle = EntityHandle(temp);

		anchor->Destroy(); // can call [Entity::Destroy()]

		return true;
	}

	const Entity* Entity::GetEntity(EntityHandle handle)
	{
		int entityIndex = handle.GetValue();
		if (entityIndex < 0
			|| entityIndex >= all.size()
			|| all[entityIndex].isInUse == false)
		{
			return nullptr;
		}

		return &all[entityIndex];
	}

	bool Entity::Destroy(EntityHandle handle)
	{
		int entityIndex = handle.GetValue();
		if (entityIndex < 0
			|| entityIndex >= all.size()
			|| all[entityIndex].isInUse == false)
		{
			return false;
		}

		return all[entityIndex].Destroy();
	}

	const std::string& Entity::GetName() const
	{
		return anchor->GetName();
	}

	void Entity::SetName(const std::string& name) const
	{
		return anchor->SetName(name);
	}

	Array<EntityDetail*> Entity::GetAllComponents() const
	{
		std::vector<EntityDetail*> subset;

		for (EntityDetail& detail : entitiesDetails)
		{
			if (detail.entityHandle == handle)
			{
				subset.push_back(&detail);
			}
		}

		return subset;
	}

	void* Entity::AddComponent(const Core::Structure* compMeta, const void* params) const
	{
		//static_assert(compMeta->Inherit("Comp"), "T must inherit from Component");

		if (handle.IsNotValid()) // todo: check only in debug 
		{
			LOG(LOG_WARNING, "Cannot add component to entity not created with Entity::CreateEntity()", ELogChannel::CLOG_ECS);
			return nullptr;
		}

		const Function* createFunction = compMeta->FindFunction("CreateComponentFor");
		createFunction->SetParam("handle", &handle);
		createFunction->SetParam("params", &params);
		void* component = *(void**)createFunction->Invoke(nullptr).Data;

		const Function* getHandleFunction = compMeta->FindFunction("GetHandle");
		ComponentHandle componentHandle = *(ComponentHandle*)getHandleFunction->Invoke(component).Data;

		GetDetail().emplace_back(handle, componentHandle);

		return component;
	}

	bool Entity::DestroyComponent(ComponentHandle component) const
	{
		if (RemoveDetail(handle, component))
		{
			return Destroy(component);
		}
		
		return false;
	}

	int Entity::Setup()
	{
		return -1;
	}

	std::vector<EntityDetail>& Entity::GetDetail()
	{
		return entitiesDetails;
	}

	void Entity::AddDetail(EntityHandle entity, ComponentHandle component)
	{
		entitiesDetails.emplace_back(entity, component);
	}

	bool Entity::RemoveDetail(EntityHandle entity, ComponentHandle component)
	{
		for (int i = 0; i < entitiesDetails.size(); i++)
		{
			if (entitiesDetails[i].componentHandle == component)
			{
				if (entitiesDetails[i].entityHandle == entity)
				{
					entitiesDetails[i] = entitiesDetails[entitiesDetails.size() - 1];
					entitiesDetails.pop_back();

					return true;
				}
				else
				{
					LOG(LOG_DEBUG, "RemoveComponent fail : the component belong to an other entity", ELogChannel::CLOG_ECS);
					return false;
				}
			}
		}

		LOG(LOG_DEBUG, "RemoveComponent fail : component not found", ELogChannel::CLOG_ECS);
		return false;
	}

	bool Entity::Destroy(ComponentHandle component)
	{
		const Structure* compType = (Structure*)Type::Find(component.GetType());
		const Function* destroyComponentFunction = compType->FindFunction("DestroyComponent");

		destroyComponentFunction->SetParam("handle", &component);

		return *(bool*)destroyComponentFunction->Invoke(nullptr).Data;
	}

	bool Entity::Iterator::Next()
	{
		do
		{
			index++;
		} while (index < all.size()
			&& all[index].isInUse == false);

		return index < all.size();
	}

	bool Entity::Iterator::Prev()
	{
		do
		{
			index--;
		} while (index >= 0
			&& all[index].isInUse == false);

		return index >= 0;
	}
}
