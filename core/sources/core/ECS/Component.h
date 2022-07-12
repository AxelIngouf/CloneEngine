#pragma once

#include <vector>

#include "../Array.h"
#include "Handle.h"
#include "HasFunction.h"
#include "World.h"

namespace Core
{
	class Entity;
	class EntityDetail;

	class Comp { private: inline static const Structure* MetaData = StructBuilder("Comp", 0).Finish(); };

	namespace Reflection
	{
		Structure* ComponentMeta();
	}

	template<typename T>
	class Component : public Comp // todo : add begin play, end play, fixed update, early update, late update
	{
	public:
		class Iterator // todo: complete function https://www.cplusplus.com/reference/iterator/
		{
		public:
			Iterator() = delete;
			Iterator(std::vector<T>& allComponent) : all(allComponent) {} // todo:  test if need first to be valid
			~Iterator() = default;

			bool Next();
			bool Prev();

			Iterator& operator++() { Next(); return *this; }
			Iterator operator++(int) { Iterator old = *this; operator++(); return old; }
			Iterator& operator--() { Prev(); return *this; }
			Iterator operator--(int) { Iterator old = *this; operator--(); return old; }

			T& operator*() { return all[index]; }
			T* operator->() { return &all[index]; }

		private:
			int index = -1;
			std::vector<T>& all;
		};

		
		EntityHandle GetEntityHandle() const { return entityHandle; }
		ComponentHandle GetHandle() const { return ComponentHandle((int)((T*)this - &all[0]), GetMetaData()->name.hash); }

		[[nodiscard]] bool GetIsActive() const { return isActive; }
		void SetIsActive(bool value) { isActive = value; }
		
		static void UpdateAllComponent(float elapsedTime);
		static void BeginPlayAllComponent();

		static T* GetComponent(ComponentHandle handle);

		static Iterator GetAll() { return Iterator(all); }

	protected:

		Component() = default;
		~Component() = default;

	private:
		friend Entity; // for DestroyComponent(), ... ??
		static Array<T*> GetComponentFromEntity(EntityHandle handle);
		static T* CreateComponentFor(EntityHandle handle, const void* params); // doc: can invalidate all other previous T ptr
		static bool DestroyComponent(ComponentHandle handle);

		static void* GetRawComponentData() { return &all[0]; }
		static int GetNumberOfComponentData() { return (int)all.size(); }
		static const Structure* GetMetaData() { return T::MetaData; }

		static int SetupComponent();

		EntityHandle entityHandle;
		bool isInUse = false;
		bool isActive = false;

		inline static std::vector<T> all;
		inline static int lastFreeIndex = SetupComponent();

		
	//meta:
	protected:
		struct CreateComponentForFunctionObject : public Function
		{
			CreateComponentForFunctionObject() :
				Function("CreateComponentFor", "void*", { Parameter("EntityHandle", "handle"), Parameter("const void*", "params") }, true)
			{}

			void InvokeImpl(void*) const override
			{
				*(void**)buffer = CreateComponentFor(*(EntityHandle*)GetParamValue("handle"), *(void**)GetParamValue("params"));
			}
		};

		struct CreateComponentFunctionObject : public Function
		{
			CreateComponentFunctionObject() :
				Function("CreateComponent", "void*", true)
			{}

			void InvokeImpl(void*) const override
			{
				*(void**)buffer = CreateComponent();
			}
		};

		struct GetComponentFunctionObject : public Function
		{
			GetComponentFunctionObject() :
				Function("GetComponent", "void*", { Parameter("ComponentHandle", "handle") }, true)
			{}

			void InvokeImpl(void*) const override
			{
				*(void**)buffer = GetComponent(*(ComponentHandle*)GetParamValue("handle"));
			}
		};

		struct DestroyComponentFunctionObject : public Function
		{
			DestroyComponentFunctionObject() :
				Function("DestroyComponent", "bool", { Parameter("ComponentHandle", "handle") }, true)
			{}

			void InvokeImpl(void*) const override
			{
				*(bool*)buffer = DestroyComponent(*(ComponentHandle*)GetParamValue("handle"));
			}
		};

		struct GetHandleFunctionObject : public Function
		{
			GetHandleFunctionObject() :
				Function("GetHandle", "ComponentHandle")
			{}

			void InvokeImpl(void* ref) const override
			{
				*(ComponentHandle*)buffer = ((T*)ref)->GetHandle();
			}
		};

	private:
		static void* CreateComponent();

		friend Structure* Reflection::ComponentMeta();
		inline static const Structure* const MetaData = Reflection::ComponentMeta();
	};

	inline Structure* Reflection::ComponentMeta()
	{
		return StructBuilder("Component", 8)
			.AddBase("Comp", 0)
			.AddField("EntityHandle", "entityHandle", 0, FieldFlag::EDITOR_HIDDEN)
			.AddField("bool", "isInUse", sizeof(EntityHandle), FieldFlag::TRANSIENT | FieldFlag::EDITOR_HIDDEN)
			.AddField("bool", "isActive", sizeof(EntityHandle) + 1)
			.Finish(); // todo : complete MetaData for component
	};

	template<typename T>
	inline void Component<T>::UpdateAllComponent(float elapsedTime)
	{
		for (T& component : all)
		{
			if (component.isActive && component.isInUse) // todo: use flags
			{
				component.Update(elapsedTime);
			}
		}
	}

	template<typename T>
	inline void Component<T>::BeginPlayAllComponent()
	{
		for (T& component : all)
		{
			if (component.isActive && component.isInUse) // todo: use flags
			{
				component.BeginPlay();
			}
		}
	}

	template<typename T>
	inline T* Component<T>::CreateComponentFor(EntityHandle handle, const void* params)
	{
		int newComponentIndex;
		if (lastFreeIndex == -1)
		{
			newComponentIndex = (int)all.size();
			all.emplace_back();
		}
		else
		{
			newComponentIndex = lastFreeIndex;
			lastFreeIndex = all[lastFreeIndex].entityHandle.GetValue();
		}

		all[newComponentIndex].entityHandle = handle;
		all[newComponentIndex].isInUse = true;
		all[newComponentIndex].isActive = true;
		if constexpr (HasInitialize<T>::value)
		{
			all[newComponentIndex].Initialize(params);
		}
		else
		{
			(void)params;
		}
		if constexpr (HasConstructor<T>::value)
		{
			all[newComponentIndex].Constructor();
		}

		return &all[newComponentIndex];
	}

	template<typename T>
	inline bool Component<T>::DestroyComponent(ComponentHandle handle)
	{
		int componentIndex = handle.GetValue();
		if (componentIndex < 0
			|| componentIndex >= all.size()
			|| all[componentIndex].isInUse == false)
		{
			return false;
		}

		if constexpr (HasFinalize<T>::value)
		{
			((T*)&all[componentIndex])->Finalize();
		}

		all[componentIndex].isInUse = false;

		all[componentIndex].entityHandle = EntityHandle(lastFreeIndex);
		lastFreeIndex = componentIndex;

		return true;
	}

	template<typename T>
	inline T* Component<T>::GetComponent(ComponentHandle handle)
	{
		int componentIndex = handle.GetValue();
		if (componentIndex < 0
			|| componentIndex >= all.size()
			|| all[componentIndex].isInUse == false
			|| handle.GetType() != T::MetaData->name.hash)
		{
			return nullptr;
		}

		return &all[handle.GetValue()];
	}

	template<typename T>
	inline Array<T*> Component<T>::GetComponentFromEntity(EntityHandle handle)
	{
		std::vector<T*> subset;

		for (T& component : all)
		{
			if (component.isInUse
				&& component.entityHandle == handle)
			{
				subset.push_back(&component);
			}
		}

		return subset;
	}

	template<typename T>
	inline int Component<T>::SetupComponent()
	{
		if constexpr (HasUpdate<T>::value)
		{
			World::RegisterComponentForUpdate(&T::UpdateAllComponent);
			// Todo: make it so this function is called only once per component instead of twice (dll + exe)
			// hint remove inline of [inline static int lastFreeIndex] at line ~95
			// (quick fix: use static lib instead of dll)
		}

		if constexpr (HasBeginPlay<T>::value)
		{
			// Todo: decide if we should rename BeginPlay
			World::RegisterComponentForBeginPlay(&T::BeginPlayAllComponent); // todo: use lambda
		}

		World::RegisterComponentForSerialization(&T::GetRawComponentData, &T::GetNumberOfComponentData, &T::GetMetaData);

		return -1;
	}

	template<typename T>
	inline void* Component<T>::CreateComponent()
	{
		int newComponentIndex;
		if (lastFreeIndex == -1)
		{
			newComponentIndex = (int)all.size();
			all.emplace_back();
		}
		else
		{
			newComponentIndex = lastFreeIndex;
			lastFreeIndex = all[lastFreeIndex].entityHandle.GetValue();
		}

		all[newComponentIndex].isInUse = true;

		return &all[newComponentIndex];
	}

	template<typename T>
	inline bool Component<T>::Iterator::Next()
	{
		do
		{
			index++;
		} while (index < all.size()
			&& all[index].isInUse == false);

		return index < all.size();
	}

	template<typename T>
	inline bool Component<T>::Iterator::Prev()
	{
		do
		{
			index--;
		} while (index >= 0
			&& all[index].isInUse == false);

		return index >= 0;
	}
}



#define COMPONENT(__struct_name__, ...)\
STRUCT(__struct_name__,\
	BASE2(Core::Component<__struct_name__>, Component),\
	__VA_ARGS__,\
	SUPPLEMENT(\
		EMPTY(),\
		EMPTY(),\
		.AddFunction(new Core::Component<__struct_name__>::CreateComponentForFunctionObject())\
		.AddFunction(new Core::Component<__struct_name__>::CreateComponentFunctionObject())\
		.AddFunction(new Core::Component<__struct_name__>::GetComponentFunctionObject())\
		.AddFunction(new Core::Component<__struct_name__>::DestroyComponentFunctionObject())\
		.AddFunction(new Core::Component<__struct_name__>::GetHandleFunctionObject())\
	)\
)

#define INIT_PARAM(__struct_name__) STATIC_FIELD(long long, initParamsType, Core::ConstexprCustomHash(STRINGYFY(__struct_name__)))
