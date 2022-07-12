#include "UIFont.h"

#include "imgui/imgui.h"

using namespace ImGui;

UIFont::UIFont(const std::string& fontPath, const float fontSize)
{
    ImGuiIO& io = GetIO();

    font = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), fontSize);
}

void UIFont::LoadFont() const
{
    PushFont(font);
}

void UIFont::UnloadFont()
{
    PopFont();
}

void UIFont::SetFontSize(const float fontSize) const
{
    font->Scale = fontSize;
}

float UIFont::GetFontSize() const
{
    return font->Scale;
}
