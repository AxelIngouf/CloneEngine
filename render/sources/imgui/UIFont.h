#pragma once
#include <string>

struct ImFont;

class UIFont
{
public:
    UIFont() = delete;
    UIFont(const std::string& fontPath, float fontSize);

    void    LoadFont() const;
    static void    UnloadFont();

    void    SetFontSize(float fontSize) const;
    [[nodiscard]] float   GetFontSize() const;

private:
    ImFont* font = nullptr;
};

