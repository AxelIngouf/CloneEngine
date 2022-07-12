#include "UI.h"


#include "core/PoolAllocator.h"
#include "core/ResourceManager.h"

void UI::Draw() const
{
    for (UIObject* object : objects)
    {
        if(!object->isHidden)
        {
            if(object->objectType == EObjectType::TEXT)
            {
                ((UIText*)object)->Draw();
            }
            else if (object->objectType == EObjectType::IMAGE)
            {
                ((UIImage*)object)->Draw();
            }
            else if (object->objectType == EObjectType::BUTTON)
            {
                ((UIButton*)object)->Draw();
            }
        }
    }
}

void UI::Init()
{
    for (const auto& object : objects)
    {
        object->Init();
    }

    isSimulationRunning = true;
}

UIImage* UI::AddImage(const std::string& path, float x, float y, float imageWidth, float imageHeight)
{

    auto* image = Core::MemoryPool::Alloc<UIImage>(path, objectsNumber, x, y, imageWidth, imageHeight);

    objects.push_back(image);
    objectsNumber++;

    if (isSimulationRunning) // security check to ensure Vulkan context was already loaded
        image->Init();

    return image;
}

UIText* UI::AddText(const std::string& text, float x, float y, float fontSize)
{
    auto* uiText = Core::MemoryPool::Alloc<UIText>(text, objectsNumber, x, y, fontSize);

    objects.push_back(uiText);
    objectsNumber++;

    return uiText;
}

UIButton* UI::AddButton(const std::string& label, float x, float y, float width, float height)
{
    auto* button = Core::MemoryPool::Alloc<UIButton>(label, objectsNumber, x, y, width, height);

    objects.push_back(button);
    objectsNumber++;

    return button;
}

void UI::ReleaseUIObject(UIObject* object)
{
    for(auto it = objects.begin(); it != objects.end(); ++it)
    {
        if((*it)->id == object->id)
        {
            Core::MemoryPool::Free(*it);
            objects.erase(it);
            return;
        }
    }
}

UIFont* UI::CreateFont(const std::string& path, const float fontSize)
{
    auto* font = Core::MemoryPool::Alloc<UIFont>(path, fontSize);

    ResourceManager::AddResource(path, font);

    return font;
}
