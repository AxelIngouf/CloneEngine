#pragma once

#include "UIFont.h"
#include "UIObject.h"

class UI
{
public:
    UI() = default;

    void    Draw() const;

    void    Init();

    UIImage*    AddImage(const std::string& path, float x, float y, float imageWidth, float imageHeight);
    UIText*     AddText(const std::string& text, float x, float y, float fontSize);
    UIButton*   AddButton(const std::string& label, float x, float y, float width, float height);

    void    ReleaseUIObject(UIObject* object);

    [[nodiscard]] static UIFont*    CreateFont(const std::string& path, float fontSize);

private:
    std::vector<UIObject*> objects;
    int objectsNumber = 0;

    bool    isSimulationRunning = false;
};
