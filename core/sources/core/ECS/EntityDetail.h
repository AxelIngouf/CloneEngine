#pragma once

#include "Handle.h"
#include "../reflection/StructMeta.h"

namespace Core
{
	class Entity;
	class EntityDetail
	{
	public:
		EntityDetail(EntityHandle entity, ComponentHandle component) :
			entityHandle(entity),
			componentHandle(component)
		{}

		template<typename T>
		bool IsComponentA() { return &T::MetaData->name.hash == componentHandle.GetType(); }

        Structure* GetComponentMeta() const
        {
			return (Structure*)Type::Find(componentHandle.GetType());
		}

		void* GetComponent()
		{
			const Structure* compType = (Structure*)Type::Find(componentHandle.GetType());
			const Function* getComponentFunction = compType->FindFunction("GetComponent");

			getComponentFunction->SetParam("handle", &componentHandle);

			return *(void**)getComponentFunction->Invoke(nullptr).Data;
		}

		template<typename T>
		T* GetComponentAs() { return (T*)GetComponent(); }

		template<typename T>
		T* TryGetComponent() { return IsComponentA<T>() ? (T*)GetComponent() : nullptr; }

		template<typename T>
		bool IsForThisComponent(ComponentHandle componentHandle) { return IsComponentA<T>() && componentHandle == this->componentHandle; }

		void BeginPlay()
		{
			const Structure* compType = (Structure*)Type::Find(componentHandle.GetType());
			const Function* beginPlayFunction = compType->FindFunction("BeginPlay");

			if (beginPlayFunction != nullptr)
			{
				beginPlayFunction->Invoke(GetComponent());
			}
		}

		ComponentHandle GetComponentHandle() const { return componentHandle; }

	private:
		friend Entity;

		EntityHandle entityHandle;
		ComponentHandle componentHandle;
	};
}
