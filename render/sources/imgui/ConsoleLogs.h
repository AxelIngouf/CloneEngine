#pragma once
#include "imgui/imgui.h"

namespace Core
{
    enum class ELogLevel;
    struct CLogMessage;
}

struct ConsoleLogs
{
	ConsoleLogs();

	void Clear();

	void AddLog(Core::CLogMessage* message);

	void Draw(const char* title, bool* p_open = nullptr);

	static ConsoleLogs consoleLogs;

private:
	void	AddLogLevelColor(const Core::ELogLevel& level);


	ImGuiTextBuffer buffer;
	ImGuiTextFilter filter;
	ImVector<int>	lineOffsets;
	ImVector<const char*>	lineFile;
	ImVector<int>	lineFileLine;
	ImVector<ImU32>	lineColors;
	bool			autoScroll;

	ImU32	warningColor = ImColor(255, 94, 19);
	ImU32	errorColor = ImColor{ 255, 0, 0 };
	ImU32	debugColor = ImColor{ 255, 204, 0 };
	ImU32	defaultColor = ImColor{ 255, 255, 255 };
};


void ReceiveNewLog(Core::CLogMessage* message);
