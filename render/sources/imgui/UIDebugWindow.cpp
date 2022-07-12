#include "UIDebugWindow.h"


#include "UIDebugBenchmarkValue.h"
#include "UIDebugSection.h"


void UIDebugWindow::DrawBenchmarkValues()
{
	for (const auto& value : GetBenchmarkValues())
	{
		UIDebugBenchmarkValue::DrawBenchmarkGraph(value);
	}
}

void UIDebugWindow::DrawDebugWindow()
{
	for (const auto& section : GetSections())
	{
		UIDebugSection::DrawDebugSection(section);
	}
}
