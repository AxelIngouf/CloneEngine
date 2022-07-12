#pragma once

#include <string>
#include <vector>

#include "DebugWatchValue.h"

namespace Core
{
	class DebugSection
	{
	public:

		explicit DebugSection(std::string title);

		void AddDebugWatchValue(const DebugWatchValue& value);

		[[nodiscard]] const std::vector<DebugWatchValue>& GetWatchValues() const { return debugWatchValues; }
		[[nodiscard]] const std::string& GetSectionTitle() const { return sectionTitle; }

		bool operator==(const DebugSection& other) const
		{
			return sectionTitle == other.sectionTitle;
		}

		bool operator==(const std::string& other) const
		{
			return sectionTitle == other;
		}

	protected:

		std::string sectionTitle;

		std::vector<DebugWatchValue> debugWatchValues;
	};
}
