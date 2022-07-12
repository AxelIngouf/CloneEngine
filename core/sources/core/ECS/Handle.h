#pragma once

#include "core/reflection/StructMeta.h"

namespace Core
{
	namespace Reflection
	{
		Structure* ComponentHandleMeta();
		Structure* EntityHandleMeta();
	}

	class ComponentHandle
	{
	public:
		ComponentHandle() = default;
		ComponentHandle(int value, long long type) : value(value), type(type) {}
		ComponentHandle(const ComponentHandle& other) = default;
		ComponentHandle(ComponentHandle&& other) = default;
		~ComponentHandle() = default;

		ComponentHandle& operator=(const ComponentHandle& other) = default;
		ComponentHandle& operator=(ComponentHandle&& other) = default;

		bool operator==(const ComponentHandle& rhs) const { return value == rhs.value && type == rhs.type; }
		bool operator!=(const ComponentHandle& rhs) const { return !(*this == rhs); }

		bool IsValid() const { return value > -1; }
		bool IsNotValid() const { return value < 0; }
		int GetValue() const { return value; }
		long long GetType() const { return type; }

		static const int INVALID_VALUE = -1;

	private:
		int value = ComponentHandle::INVALID_VALUE;
		long long type = 0;

		friend Structure* Reflection::ComponentHandleMeta();
		inline static const Structure* const MetaData = Reflection::ComponentHandleMeta();
	};

	inline Structure* Reflection::ComponentHandleMeta()
	{
		return StructBuilder("ComponentHandle", sizeof(ComponentHandle))
			.AddField("int", "value", (int)(long long)&((ComponentHandle*)0)->value)
			.AddField("long long", "type", (int)(long long)&((ComponentHandle*)0)->type)
			.Finish(); // todo : complete MetaData for handle
	}

	class  EntityHandle
	{
	public:
		EntityHandle() = default;
		explicit EntityHandle(int value) : value(value) {}
		EntityHandle(const EntityHandle& other) = default;
		EntityHandle(EntityHandle&& other) = default;
		~EntityHandle() = default;

		EntityHandle& operator=(const EntityHandle& other) = default;
		EntityHandle& operator=(EntityHandle&& other) = default;

		bool operator==(const EntityHandle& rhs) const { return value == rhs.value; }
		bool operator!=(const EntityHandle& rhs) const { return value != rhs.value; }

		bool IsValid() const { return value > -1; }
		bool IsNotValid() const { return value < 0; }
		int GetValue() const { return value; }

		static const int INVALID_VALUE = -1;

	private:
		int value = EntityHandle::INVALID_VALUE;

		friend Structure* Reflection::EntityHandleMeta();
		inline static const Structure* const MetaData = Reflection::EntityHandleMeta();
	};

	inline Structure* Reflection::EntityHandleMeta()
	{
		return StructBuilder("EntityHandle", sizeof(EntityHandle))
			.AddField("int", "value", (int)(long long)&((EntityHandle*)0)->value)
			.Finish(); // todo : complete MetaData for handle
	}
}
