#pragma once

#include "HasFunction.h"
#include "World.h"

namespace Core
{
	class Sys { private: inline static const Core::Structure* MetaData = Core::StructBuilder("Sys", 0).Finish(); };

	template<typename T>
	class System : public Sys
	{
	private:
		static char SetupSystem();
		inline static char c = SetupSystem();

	private:
		inline static const Core::Structure* MetaData = Core::StructBuilder("System", 0)
			.AddBase("Sys")
			.Finish(); // todo : complete MetaData for system
	};

	template<typename T>
	inline char System<T>::SetupSystem()
	{
		if constexpr (HasUpdate<T>::value)
		{
			World::RegisterComponentForUpdate(&T::Update);
		}

		return '\0';
	}
}