#pragma once
#include <array>
#include <vector>

#include "imgui/imgui.h"

class UITheme
{
public:
	enum CurrentTheme
	{
		WATER,
		FIRE,
		EARTH,

		NUM_UI_THEMES
	};

	const char* themeNames[NUM_UI_THEMES] = {"Water", "Fire", "Earth"};

	enum CurrentBrightness
	{
		BRIGHTNESS_DARK,
		BRIGHTNESS_STANDARD,
		BRIGHTNESS_LIGHT,

		NUM_BRIGHTNESS_LEVELS
	};

	const char* brightnessNames[NUM_BRIGHTNESS_LEVELS] = {"Dark", "Standard", "Light"};

	void InitThemeValues();
	void UpdateUITheme();
	void UpdateUIColor();

	void ChangeUITheme();

	CurrentBrightness currentBrightness = BRIGHTNESS_STANDARD;
	CurrentTheme currentTheme = WATER;

	struct ThemeBaseColors
	{
		ImVec4 textColor;
		ImVec4 backgroundColor;
		ImVec4 highlightColor1;
		ImVec4 highlightColor2;


		[[nodiscard]] ThemeBaseColors MakeThemeCopy(const ThemeBaseColors& hsvModifiers) const;
		[[nodiscard]] static ImVec4 AddHsvModifier(const ImVec4& original, const ImVec4& mod);
		[[nodiscard]] static ImVec4 SubHsvModifier(const ImVec4& original, const ImVec4& mod);
		[[nodiscard]] static ImVec4 GenerateHsv(const ImVec4& original);
		[[nodiscard]] static ImVec4 GenerateRgb(const ImVec4& original);
	private:
		[[nodiscard]] ThemeBaseColors MakeThemeHsvCopy() const;
		[[nodiscard]] ThemeBaseColors MakeThemeRgbCopy() const;
	};

	ThemeBaseColors currentBaseColors;

private:
	enum class ColorWeightReference
	{
		TEXT,
		BACKGROUND,
		HIGHLIGHT1,
		HIGHLIGHT2,
	};

	[[nodiscard]] static ImVec4 GetHsvOffset(const ImVec4& original, const ImVec4& offsetReference);
	[[nodiscard]] static ImVec4 ApplyHsvOffset(const ImVec4& original, const ImVec4& hsvOffset);
	void GeneratePresets();

	std::vector<ColorWeightReference> colorReferences;
	std::vector<ImVec4> hsvOffsets;
	std::vector<ImVec4> currentColors;


	std::array<std::array<ThemeBaseColors, NUM_BRIGHTNESS_LEVELS>, NUM_UI_THEMES> presets;
};
