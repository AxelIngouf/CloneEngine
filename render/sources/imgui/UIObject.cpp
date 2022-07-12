#include <utility>

#include "UIObject.h"

#include "ImguiImpl.h"
#include "core/ResourceManager.h"
#include "core/ThreadPool.h"
#include "imgui/imgui.h"

using namespace ImGui;

UIObject::UIObject() :
    isHidden(false),
    xScreenLocation(0.f),
    yScreenLocation(0.f),
    xLocalLocation(0.f),
    yLocalLocation(0.f),
    isDirtyLocation(true),
    id(-1),
    anchor(EAnchor::TOP_LEFT),
    objectType(EObjectType::TEXT)
{
}

void UIObject::Init()
{}

void UIObject::Draw()
{
    if(isDirtyLocation)
    {
        UpdateScreenLocation();
    }
    SetCursor();
}

void UIObject::SetLocationX(const float newX)
{
    xLocalLocation = newX;
    isDirtyLocation = true;
}

void UIObject::SetLocationY(const float newY)
{
    yLocalLocation = newY;
    isDirtyLocation = true;
}

void UIObject::SetLocation(const float newX, const float newY)
{
    xLocalLocation = newX;
    yLocalLocation = newY;
    isDirtyLocation = true;
}

void UIObject::SetAnchor(const EAnchor newAnchor)
{
    anchor = newAnchor;
    isDirtyLocation = true;
}

void UIObject::SetCursor()
{
    SetCursorPos({xScreenLocation, yScreenLocation});
}

LibMath::Vector2 UIObject::GetRelativeLocation()
{
    return LibMath::Vector2{ xLocalLocation, yLocalLocation };
}

LibMath::Vector2 UIObject::GetScreenLocation()
{
    return LibMath::Vector2{ xScreenLocation, yScreenLocation };
}

void UIObject::UpdateScreenLocation()
{
    const ImVec2& windowSize = GetWindowSize();

    if(anchor == EAnchor::TOP_LEFT)
    {
        SetScreenLocation(xLocalLocation, yLocalLocation);
    }
    else if (anchor == EAnchor::TOP_RIGHT)
    {
        SetScreenLocation(windowSize.x - xLocalLocation, yLocalLocation);
    }
    else if(anchor == EAnchor::BOTTOM_LEFT)
    {
        SetScreenLocation(xLocalLocation, windowSize.y - yLocalLocation);
    }
    else if(anchor == EAnchor::BOTTOM_RIGHT)
    {
        SetScreenLocation(windowSize.x - xLocalLocation, windowSize.y - yLocalLocation);
    }
    else if(anchor == EAnchor::TOP)
    {
        SetScreenLocation(windowSize.x / 2 + xLocalLocation, yLocalLocation);
    }
    else if(anchor == EAnchor::BOTTOM)
    {
        SetScreenLocation(windowSize.x / 2 + xLocalLocation, windowSize.y - yLocalLocation);
    }
    else if(anchor == EAnchor::LEFT)
    {
        SetScreenLocation(xLocalLocation, windowSize.y / 2 + yLocalLocation);
    }
    else if(anchor == EAnchor::RIGHT)
    {
        SetScreenLocation(windowSize.x - xLocalLocation, windowSize.y / 2 + yLocalLocation);
    }
    else if(anchor == EAnchor::CENTER)
    {
        SetScreenLocation(windowSize.x / 2 + xLocalLocation, windowSize.y / 2 + yLocalLocation);
    }

    isDirtyLocation = false;
}

void UIObject::SetScreenLocation(const float newX, const float newY)
{
    xScreenLocation = newX;
    yScreenLocation = newY;
}

UIText::UIText() :
    size(10.f),
    text("Text here")
{
}

UIText::UIText(std::string otherText, const int objectId, const float x, const float y, const float fontSize) : size(fontSize), text(std::move(otherText))
{
    xLocalLocation = x;
    yLocalLocation = y;

    id = objectId;

    objectType = EObjectType::TEXT;
}

void UIText::Draw()
{
    UIObject::Draw();

    // todo resize font

    if(text.capacity() < 100)
        Text(text.c_str());
}

UIImage::UIImage() :
    width(1.f),
    height(1.f),
    imagePointer(nullptr)
{
}

UIImage::UIImage(const std::string& imagePath, const int objectId, const float x, const float y, float imageWidth, float imageHeight)
{
    xLocalLocation = x;
    yLocalLocation = y;

    width = imageWidth;
    height = imageHeight;

    path = imagePath;

    id = objectId;
    objectType = EObjectType::IMAGE;
}

void UIImage::Draw()
{
    UIObject::Draw();

    if(imagePointer != nullptr)
        Image(imagePointer, {width, height});
}

void UIImage::Init()
{
    imagePointer = ResourceManager::GetResource<void>(path + "imgui");
    if(imagePointer == nullptr)
    {
        Core::ThreadPool::defaultThreadPool.AddTask([this]
        {
            ImGuiImpl::LoadImGuiTexture(path);
            this->imagePointer = ResourceManager::GetResource<void>(path + "imgui");
        });
    }
}

UIButton::UIButton() :
    width(1.f),
    height(1.f),
    label("Button")
{
}

UIButton::UIButton(const std::string& buttonLabel, const int objectId, float x, float y, float buttonWidth, float buttonHeight)
{
    xLocalLocation = x;
    yLocalLocation = y;

    width = buttonWidth;
    height = buttonHeight;

    label = buttonLabel;

    id = objectId;
    objectType = EObjectType::BUTTON;
}

void UIButton::Draw()
{
    UIObject::Draw();

    if(Button(label.c_str(), { width, height }))
    {
        onButtonPressed.Broadcast();
    }
}
