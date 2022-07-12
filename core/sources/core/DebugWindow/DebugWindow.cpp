#include "DebugWindow.h"

#include "core/CLog.h"
#include <algorithm>

namespace Core
{
	DebugWindow DebugWindow::defaultDebugWindow;

	void DebugWindow::AddSection(const std::string& sectionTitle)
	{
		defaultDebugWindow.debugSections.emplace_back(sectionTitle);
	}

	void DebugWindow::AddDebugValue(const DebugWatchValue& value, const std::string& sectionTitle)
	{
		const auto it = std::find(defaultDebugWindow.debugSections.begin(),
		                          defaultDebugWindow.debugSections.end(), sectionTitle);

		if (it == defaultDebugWindow.debugSections.end())
		{
			AddSection(sectionTitle);
			defaultDebugWindow.debugSections[defaultDebugWindow.debugSections.size() - 1].AddDebugWatchValue(value);
		}
		else
		{
			it->AddDebugWatchValue(value);
		}
	}

	void DebugWindow::StartBenchmark(const std::string& name)
	{
		const auto it = std::find(defaultDebugWindow.benchmarkValues.begin(),
		                          defaultDebugWindow.benchmarkValues.end(), name);

		if (it == defaultDebugWindow.benchmarkValues.end())
			defaultDebugWindow.benchmarkValues.emplace_back(name).StartTimer();
		else
			it->StartTimer();
	}

	void DebugWindow::StopBenchmark(const std::string& name)
	{
		const auto it = std::find(defaultDebugWindow.benchmarkValues.begin(),
		                          defaultDebugWindow.benchmarkValues.end(), name);

		if (it == defaultDebugWindow.benchmarkValues.end())
			defaultDebugWindow.benchmarkValues.emplace_back(name).StopTimer();
		else
			it->StopTimer();
	}

	const std::vector<DebugSection>& DebugWindow::GetSections()
	{
		return defaultDebugWindow.debugSections;
	}

	const std::vector<BenchmarkValue>& DebugWindow::GetBenchmarkValues()
	{
		return defaultDebugWindow.benchmarkValues;
	}
}
