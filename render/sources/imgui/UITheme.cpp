#include "UITheme.h"


void UITheme::InitThemeValues()
{
	const ImGuiStyle& style = ImGui::GetStyle();


	currentBaseColors.textColor = style.Colors[ImGuiCol_Text];
	currentBaseColors.backgroundColor = style.Colors[ImGuiCol_WindowBg];
	currentBaseColors.highlightColor1 = style.Colors[ImGuiCol_Button];
	currentBaseColors.highlightColor2 = style.Colors[ImGuiCol_DragDropTarget];

	colorReferences.resize(ImGuiCol_COUNT);
	colorReferences[ImGuiCol_Text] = ColorWeightReference::TEXT;
	colorReferences[ImGuiCol_TextDisabled] = ColorWeightReference::TEXT;
	colorReferences[ImGuiCol_PlotLines] = ColorWeightReference::TEXT;
	colorReferences[ImGuiCol_NavWindowingHighlight] = ColorWeightReference::TEXT;
	colorReferences[ImGuiCol_NavWindowingDimBg] = ColorWeightReference::TEXT;
	colorReferences[ImGuiCol_ModalWindowDimBg] = ColorWeightReference::TEXT;
	colorReferences[ImGuiCol_TableRowBgAlt] = ColorWeightReference::TEXT;
	colorReferences[ImGuiCol_WindowBg] = ColorWeightReference::BACKGROUND;
	colorReferences[ImGuiCol_PopupBg] = ColorWeightReference::BACKGROUND;
	colorReferences[ImGuiCol_TitleBg] = ColorWeightReference::BACKGROUND;
	colorReferences[ImGuiCol_MenuBarBg] = ColorWeightReference::BACKGROUND;
	colorReferences[ImGuiCol_ScrollbarBg] = ColorWeightReference::BACKGROUND;
	colorReferences[ImGuiCol_ScrollbarGrab] = ColorWeightReference::BACKGROUND;
	colorReferences[ImGuiCol_ScrollbarGrabHovered] = ColorWeightReference::BACKGROUND;
	colorReferences[ImGuiCol_ScrollbarGrabActive] = ColorWeightReference::BACKGROUND;
	colorReferences[ImGuiCol_DockingEmptyBg] = ColorWeightReference::BACKGROUND;
	colorReferences[ImGuiCol_ChildBg] = ColorWeightReference::BACKGROUND;
	colorReferences[ImGuiCol_BorderShadow] = ColorWeightReference::BACKGROUND;
	colorReferences[ImGuiCol_TitleBgCollapsed] = ColorWeightReference::BACKGROUND;
	colorReferences[ImGuiCol_TableRowBg] = ColorWeightReference::BACKGROUND;
	colorReferences[ImGuiCol_Border] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_Separator] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_TableHeaderBg] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_TableBorderStrong] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_TableBorderLight] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_FrameBg] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_FrameBgHovered] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_FrameBgActive] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_TitleBgActive] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_CheckMark] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_SliderGrab] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_SliderGrabActive] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_Button] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_ButtonHovered] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_ButtonActive] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_Header] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_HeaderHovered] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_HeaderActive] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_SeparatorHovered] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_SeparatorActive] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_ResizeGrip] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_ResizeGripHovered] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_ResizeGripActive] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_Tab] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_TabHovered] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_TabActive] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_TabUnfocused] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_TabUnfocusedActive] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_DockingPreview] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_TextSelectedBg] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_NavHighlight] = ColorWeightReference::HIGHLIGHT1;
	colorReferences[ImGuiCol_PlotLinesHovered] = ColorWeightReference::HIGHLIGHT2;
	colorReferences[ImGuiCol_PlotHistogram] = ColorWeightReference::HIGHLIGHT2;
	colorReferences[ImGuiCol_PlotHistogramHovered] = ColorWeightReference::HIGHLIGHT2;
	colorReferences[ImGuiCol_DragDropTarget] = ColorWeightReference::HIGHLIGHT2;


	hsvOffsets.resize(ImGuiCol_COUNT);
	currentColors.resize(ImGuiCol_COUNT);

	for (int i = 0; i < ImGuiCol_COUNT; i++)
	{
		switch (colorReferences[i])
		{
		case ColorWeightReference::TEXT:
			hsvOffsets[i] = GetHsvOffset(style.Colors[i], currentBaseColors.textColor);
			break;
		case ColorWeightReference::BACKGROUND:
			hsvOffsets[i] = GetHsvOffset(style.Colors[i], currentBaseColors.backgroundColor);
			break;
		case ColorWeightReference::HIGHLIGHT1:
			hsvOffsets[i] = GetHsvOffset(style.Colors[i], currentBaseColors.highlightColor1);
			break;
		case ColorWeightReference::HIGHLIGHT2:
			hsvOffsets[i] = GetHsvOffset(style.Colors[i], currentBaseColors.highlightColor2);
			break;
		}

		currentColors[i] = style.Colors[i];
	}

	GeneratePresets();
}

void UITheme::UpdateUITheme()
{
	ImGuiStyle& style = ImGui::GetStyle();

	for (int i = 0; i < ImGuiCol_COUNT; i++)
		style.Colors[i] = currentColors[i];
}

void UITheme::UpdateUIColor()
{
	for (int i = 0; i < ImGuiCol_COUNT; i++)
	{
		switch (colorReferences[i])
		{
		case ColorWeightReference::TEXT:
			currentColors[i] = ApplyHsvOffset(currentBaseColors.textColor, hsvOffsets[i]);
			break;
		case ColorWeightReference::BACKGROUND:
			currentColors[i] = ApplyHsvOffset(currentBaseColors.backgroundColor, hsvOffsets[i]);
			break;
		case ColorWeightReference::HIGHLIGHT1:
			currentColors[i] = ApplyHsvOffset(currentBaseColors.highlightColor1, hsvOffsets[i]);
			break;
		case ColorWeightReference::HIGHLIGHT2:
			currentColors[i] = ApplyHsvOffset(currentBaseColors.highlightColor2, hsvOffsets[i]);
			break;
		}
	}
}

void UITheme::ChangeUITheme()
{
	currentBaseColors.textColor = presets[currentTheme][currentBrightness].textColor;
	currentBaseColors.backgroundColor = presets[currentTheme][currentBrightness].backgroundColor;
	currentBaseColors.highlightColor1 = presets[currentTheme][currentBrightness].highlightColor1;
	currentBaseColors.highlightColor2 = presets[currentTheme][currentBrightness].highlightColor2;

	UpdateUIColor();
}

UITheme::ThemeBaseColors UITheme::ThemeBaseColors::MakeThemeCopy(const ThemeBaseColors& hsvModifiers) const
{
	ThemeBaseColors hsvOriginal = MakeThemeHsvCopy();

	hsvOriginal.textColor = AddHsvModifier(hsvOriginal.textColor, hsvModifiers.textColor);
	hsvOriginal.backgroundColor = AddHsvModifier(hsvOriginal.backgroundColor, hsvModifiers.backgroundColor);
	hsvOriginal.highlightColor1 = AddHsvModifier(hsvOriginal.highlightColor1, hsvModifiers.highlightColor1);
	hsvOriginal.highlightColor2 = AddHsvModifier(hsvOriginal.highlightColor2, hsvModifiers.highlightColor2);

	return hsvOriginal.MakeThemeRgbCopy();
}

UITheme::ThemeBaseColors UITheme::ThemeBaseColors::MakeThemeHsvCopy() const
{
	ThemeBaseColors ret;

	ret.textColor = GenerateHsv(textColor);
	ret.backgroundColor = GenerateHsv(backgroundColor);
	ret.highlightColor1 = GenerateHsv(highlightColor1);
	ret.highlightColor2 = GenerateHsv(highlightColor2);

	return ret;
}

UITheme::ThemeBaseColors UITheme::ThemeBaseColors::MakeThemeRgbCopy() const
{
	ThemeBaseColors ret;

	ret.textColor = GenerateRgb(textColor);
	ret.backgroundColor = GenerateRgb(backgroundColor);
	ret.highlightColor1 = GenerateRgb(highlightColor1);
	ret.highlightColor2 = GenerateRgb(highlightColor2);

	return ret;
}

ImVec4 UITheme::ThemeBaseColors::AddHsvModifier(const ImVec4& original, const ImVec4& mod)
{
	ImVec4 ret;

	ret.x = original.x + mod.x;
	ret.y = original.y + mod.y;
	ret.z = original.z + mod.z;
	ret.w = original.w + mod.w;

	return ret;
}

ImVec4 UITheme::ThemeBaseColors::SubHsvModifier(const ImVec4& original, const ImVec4& mod)
{
	ImVec4 ret;

	ret.x = original.x - mod.x;
	ret.y = original.y - mod.y;
	ret.z = original.z - mod.z;
	ret.w = original.w - mod.w;

	return ret;
}

ImVec4 UITheme::ThemeBaseColors::GenerateHsv(const ImVec4& original)
{
	ImVec4 originalHsv;

	ImGui::ColorConvertRGBtoHSV(original.x, original.y, original.z,
	                            originalHsv.x, originalHsv.y, originalHsv.z);

	originalHsv.w = original.w;

	return originalHsv;
}

ImVec4 UITheme::ThemeBaseColors::GenerateRgb(const ImVec4& original)
{
	ImVec4 originalHsv;

	ImGui::ColorConvertHSVtoRGB(original.x, original.y, original.z,
	                            originalHsv.x, originalHsv.y, originalHsv.z);

	originalHsv.w = original.w;

	return originalHsv;
}

ImVec4 UITheme::GetHsvOffset(const ImVec4& original, const ImVec4& offsetReference)
{
	return ThemeBaseColors::SubHsvModifier(ThemeBaseColors::GenerateHsv(original),
	                                       ThemeBaseColors::GenerateHsv(offsetReference));
}

ImVec4 UITheme::ApplyHsvOffset(const ImVec4& original, const ImVec4& hsvOffset)
{
	return ThemeBaseColors::GenerateRgb(
		ThemeBaseColors::AddHsvModifier(ThemeBaseColors::GenerateHsv(original), hsvOffset));
}

void UITheme::GeneratePresets()
{
	const ThemeBaseColors darkThemeHsvBrightnessModifiers{
		{0.f, 0.1f, 0.2f, 0.f},
		{0.f, -0.2f, -0.05f, 0.f},
		{0.f, -0.1f, -0.1f, 0.f},
		{0.f, -0.1f, -0.1f, 0.f}
	};

	const ThemeBaseColors lightThemeHsvBrightnessModifiers{
		{0.f, 0.f, -0.975f, 0.f},
		{0.f, -0.2f, 0.7f, 0.f},
		{0.f, 0.0f, 0.1f, 0.f},
		{0.f, 0.0f, 0.1f, 0.f}
	};

	presets[WATER][BRIGHTNESS_STANDARD] = currentBaseColors;
	presets[WATER][BRIGHTNESS_DARK] = currentBaseColors.MakeThemeCopy(darkThemeHsvBrightnessModifiers);
	presets[WATER][BRIGHTNESS_LIGHT] = currentBaseColors.MakeThemeCopy(lightThemeHsvBrightnessModifiers);

	ThemeBaseColors fireTheme{
		{1.f, 1.f, 1.f, 1.f},
		{0.306f, 0.091f, 0.091f, 0.941f},
		{0.882f, 0.310f, 0.104f, 0.714f},
		{0.663f, 0.224f, 0.471f, 0.902f}
	};

	presets[FIRE][BRIGHTNESS_STANDARD] = fireTheme;
	presets[FIRE][BRIGHTNESS_DARK] = fireTheme.MakeThemeCopy(darkThemeHsvBrightnessModifiers);
	presets[FIRE][BRIGHTNESS_LIGHT] = fireTheme.MakeThemeCopy(lightThemeHsvBrightnessModifiers);

	ThemeBaseColors earthTheme{
		{1.f, 1.f, 1.f, 1.f},
		{0.149f, 0.196f, 0.047f, 0.941f},
		{0.231f, 0.929f, 0.197f, 0.400f},
		{0.827f, 0.824f, 0.278f, 0.902f}
	};

	presets[EARTH][BRIGHTNESS_STANDARD] = earthTheme;
	presets[EARTH][BRIGHTNESS_DARK] = earthTheme.MakeThemeCopy(darkThemeHsvBrightnessModifiers);
	presets[EARTH][BRIGHTNESS_LIGHT] = earthTheme.MakeThemeCopy(lightThemeHsvBrightnessModifiers);
}
