#include "UIDebugBenchmarkValue.h"

#include "imgui/imgui.h"

void UIDebugBenchmarkValue::DrawBenchmarkGraph(const BenchmarkValue& value)
{
	ImGui::Text("%s: %.2f ms", value.GetBenchmarkTitle().c_str(),
	            value.GetBenchmarkValues()[value.GetBenchmarkValues().size() - 1]);

	float maxVal = 0;
	const float minVal = 0;

	for (float val : value.GetBenchmarkValues())
	{
		if (maxVal < val)
			maxVal = val;
	}

	ImGui::PlotLines("", &value.GetBenchmarkValues()[0], static_cast<int>(value.GetBenchmarkValues().size()), 0,
	                 nullptr, minVal, maxVal,
	                 ImVec2(ImGui::GetContentRegionAvailWidth(), 50));
}
