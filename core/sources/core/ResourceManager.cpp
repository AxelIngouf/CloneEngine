#include "ResourceManager.h"

ResourceManager::~ResourceManager()
{
    std::lock_guard<std::mutex> lock(resourceManagerMutex);

    //for (const auto& [fst, snd] : resources)
    //{
    //    if(snd != nullptr)
    //        free(snd);
    //}
    resources.clear();
}

ResourceManager& ResourceManager::GetResourceManager()
{
    static ResourceManager resourceManager;

    return resourceManager;
}

void ResourceManager::DeleteResource(const std::string& key)
{
    ResourceManager& resourceManager = GetResourceManager();

    std::lock_guard<std::mutex> lock(resourceManager.resourceManagerMutex);
    const auto it = resourceManager.resources.find(key);

    if (it == resourceManager.resources.end())
        return;

    resourceManager.resources.erase(it);
}
