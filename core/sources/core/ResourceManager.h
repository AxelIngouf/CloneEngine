#pragma once

#include <mutex>
#include <string>
#include <unordered_map>

namespace Core
{
	class ResourceManager
	{
	public:
		ResourceManager() = default;
		~ResourceManager();

		ResourceManager(const ResourceManager&) = delete;
		ResourceManager(ResourceManager&&) = delete;
		ResourceManager& operator=(const ResourceManager&) = delete;
		ResourceManager& operator=(ResourceManager&&) = delete;

		template<typename T>
		static T*	GetResource(const std::string& key);

		template<typename T>
		static void AddResource(const std::string& key, T* resource);

        static void	DeleteResource(const std::string& key);

        static ResourceManager& GetResourceManager();

	private:
		std::unordered_map<std::string, void*> resources;

		std::mutex resourceManagerMutex;
	};
}

using Core::ResourceManager;

#include "ResourceManager.inl"