#pragma once
#include <string>
#include <vector>


#include "BenchmarkValue.h"
#include "DebugSection.h"


#define START_BENCHMARK(name) Core::DebugWindow::StartBenchmark(name)
#define STOP_BENCHMARK(name) Core::DebugWindow::StopBenchmark(name)

namespace Core
{
	class DebugWindow
	{
	public:

		static void AddSection(const std::string& sectionTitle);
		static void AddDebugValue(const DebugWatchValue& value, const std::string& sectionTitle = "General");

		static void StartBenchmark(const std::string& name);
		static void StopBenchmark(const std::string& name);

		static const std::vector<DebugSection>& GetSections();
		static const std::vector<BenchmarkValue>& GetBenchmarkValues();

	protected:

		static DebugWindow defaultDebugWindow;
		std::vector<DebugSection> debugSections;
		std::vector<BenchmarkValue> benchmarkValues;
	};
}
