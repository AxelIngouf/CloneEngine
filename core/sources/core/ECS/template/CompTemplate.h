#pragma once

#include <iostream>

#include "../Component.h"

namespace Core
{
	class CompTemplate : public Component<CompTemplate>
	{
	public:
		CompTemplate() = default;
		~CompTemplate() = default;
		
		void Initialize(const void* params) { value = counter++; std::cout << "init comp " << value << " attribut with default value" << std::endl; }
		void Constructor() { std::cout << "use comp " << value << " attribut to finalize initialisation" << value << std::endl; }
		void BeginPlay() { std::cout << "finish comp " << value << " initialisation (guarantee after entity's all comp have call Constructor() but before first update)" << value << std::endl; }
		void Finalize() { std::cout << "clean up comp " << value << std::endl; }
		void Update(float elapsedTime) { std::cout << "update comp " << value << " [elapsedTime = " << elapsedTime << "]" << std::endl; }

		int value;

	private:
		inline static int counter = 0;
	};
}