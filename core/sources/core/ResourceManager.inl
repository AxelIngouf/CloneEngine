#pragma once
#include "ResourceManager.h"

namespace Core
{
    template <typename T>
    T* ResourceManager::GetResource(const std::string& key)
    {
        ResourceManager& resourceManager = GetResourceManager();

        std::lock_guard<std::mutex> lock(resourceManager.resourceManagerMutex);

        const auto it = resourceManager.resources.find(key);

        if (it == resourceManager.resources.end())
            return nullptr;

        return static_cast<T*>(it->second);
    }

    template <typename T>
    void ResourceManager::AddResource(const std::string& key, T* resource)
    {
        ResourceManager& resourceManager = GetResourceManager();

        std::lock_guard<std::mutex> lock(resourceManager.resourceManagerMutex);
        resourceManager.resources[key] = resource;
    }
}
