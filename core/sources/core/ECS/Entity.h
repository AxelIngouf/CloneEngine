#pragma once

#include <vector>

#include "EntityDetail.h"
#include "Component.h"
#include "../CLog.h"

namespace Core
{
	class SceneNode;
	class World;

	class Entity
	{
	public:
		class Iterator // todo: complete function https://www.cplusplus.com/reference/iterator/
		{
		public:
			Iterator() = delete;
			Iterator(std::vector<Entity>& allComponent) : all(allComponent) {}
			~Iterator() = default;

			bool Next();
			bool Prev();

			Iterator& operator++() { Next(); return *this; }
			Iterator operator++(int) { Iterator old = *this; operator++(); return old; }
			Iterator& operator--() { Prev(); return *this; }
			Iterator operator--(int) { Iterator old = *this; operator--(); return old; }

			Entity& operator*() { return all[index]; }
			Entity* operator->() { return &all[index]; }

		private:
			int index = -1;
			std::vector<Entity>& all;
		};


		Entity() = default;
		Entity(const Entity& other) = default;
		Entity(Entity&& other) noexcept = default;
		~Entity() = default;

		Entity& operator=(const Entity& other) = default;
		Entity& operator=(Entity&& other) noexcept = default;

		bool operator==(const Entity& other) const { return handle == other.handle; }

		static const Entity* CreateEntity(SceneNode* anchor);
		static const Entity* GetEntity(EntityHandle handle);
		static bool Destroy(EntityHandle handle);

		bool Destroy();
		[[nodiscard]] EntityHandle GetHandle() const { return handle; }
		[[nodiscard]] SceneNode* GetAnchor() const { return anchor; }
		[[nodiscard]] const std::string& GetName() const;
		void SetName(const std::string& name) const;
		[[nodiscard]] Array<EntityDetail*> GetAllComponents() const;

		[[nodiscard]] bool	IsInUse() const { return isInUse; }

		template<typename T>
		T* AddComponent(const void* params = nullptr) const;
		void* AddComponent(const Core::Structure* compMeta, const void* params = nullptr) const;

		template<typename T>
		bool DestroyComponent(T* component) const;
		bool DestroyComponent(ComponentHandle component) const;

		template<class T>
		Array<T*> GetComponents() const { return T::GetComponentFromEntity(GetHandle()); }

		static Iterator GetAllEntity() { return Iterator(all); }

	private:

		static int Setup();

		friend World; // because [World::LoadComponents(...)] need [Entity::AddDetail(...)]
		static void AddDetail(EntityHandle entity, ComponentHandle component);
		static bool RemoveDetail(EntityHandle entity, ComponentHandle component);
		static bool Destroy(ComponentHandle component);

		static std::vector<EntityDetail>& GetDetail();

		EntityHandle handle;
		SceneNode* anchor = nullptr;
		bool isInUse = false;

		static std::vector<Entity> all;
		static int lastFreeIndex;
		static std::vector<EntityDetail> entitiesDetails;
	};

	template<typename T>
	inline T* Entity::AddComponent(const void* params) const
	{
		static_assert(std::is_base_of<Comp, T>::value, "T must inherit from Component");

		if (handle.IsNotValid()) // todo: check only in debug 
		{
			LOG(LOG_WARNING, "Cannot add component to entity not created with Entity::CreateEntity()", ELogChannel::CLOG_ECS);
			return nullptr;
		}

		T* component = T::CreateComponentFor(GetHandle(), params);

		GetDetail().emplace_back(handle, component->GetHandle());

		return component;
	}

	template<typename T>
	inline bool Entity::DestroyComponent(T* component) const
	{
		if (RemoveDetail(handle, component->GetHandle()))
		{
			return T::DestroyComponent(component->GetHandle());
		}

		return false;
	}
}