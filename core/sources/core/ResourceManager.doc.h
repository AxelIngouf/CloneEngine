#pragma once

#include <core_export.h>
#include <mutex>
#include <string>
#include <unordered_map>

namespace Core
{
    /**
	 * Resource Manager used to store different types of data pointers, so they can be quickly accessed later on when needed.
	 */
	class ResourceManager
	{
	public:
		ResourceManager() = default;
		~ResourceManager();

		ResourceManager(const ResourceManager&) = delete;
		ResourceManager(ResourceManager&&) = delete;
		ResourceManager& operator=(const ResourceManager&) = delete;
		ResourceManager& operator=(ResourceManager&&) = delete;

        /**
		 * Returns the T resource pointer assigned to the specified key.
		 * 
		 * @tparam T - Pointed resource type.
		 * @param key - Resource key.
		 * @return T resource pointer assigned to the specified key.
		 */
		template<typename T>
		static T* GetResource(const std::string& key);

        /**
		 * Add the resource to the resource manager with the specified key assigned to it.
		 * 
		 * @tparam T - Pointed resource type.
		 * @param key - Resource key.
		 * @param resource - Resource to store.
		 */
		template<typename T>
		static void AddResource(const std::string& key, T* resource);

        /**
		 * Delete the resource from the resource manager. Note, this does not free the pointer.
		 * 
		 * @param key - Resource key.
		 */
		static void	DeleteResource(const std::string& key);

		/**
		 * Returns ResourceManager singleton.
		 *
		 * @return ResourceManager singleton.
		 */
		CORE_EXPORT static ResourceManager& GetResourceManager();

	private:
		/**
		 * ResourceManager stored resources. Void pointers paired with a unique string key.
		 */
		std::unordered_map<std::string, void*> resources;

        /**
		 * Resource manager mutex, used to make sure it is thread-safe.
		 */
		std::mutex resourceManagerMutex;
	};
}

using Core::ResourceManager;

#include "ResourceManager.inl"