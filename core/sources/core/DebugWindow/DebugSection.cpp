#include "DebugSection.h"

namespace Core
{
	DebugSection::DebugSection(std::string title):
		sectionTitle(std::move(title))
	{
	}

	void DebugSection::AddDebugWatchValue(const DebugWatchValue& value)
	{
		debugWatchValues.push_back(value);
	}
}
