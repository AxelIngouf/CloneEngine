#include "UIDebugSection.h"

#include "imgui/imgui.h"

void UIDebugSection::DrawDebugSection(const DebugSection& section)
{
	if (ImGui::TreeNode(section.GetSectionTitle().c_str()))
	{
		ImGui::Indent();
		for (const auto& value : section.GetWatchValues())
		{
			ImGui::Text(value.GetFormattedText().c_str());
		}
		ImGui::Unindent();
		ImGui::TreePop();
		ImGui::Separator();
	}
}
