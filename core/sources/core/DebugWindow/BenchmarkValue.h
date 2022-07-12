#pragma once

#include <chrono>
#include <string>
#include <vector>

constexpr int benchmarkValueNum = 120;

namespace Core
{
	class BenchmarkValue
	{
	public:

		explicit BenchmarkValue(std::string title);


		[[nodiscard]] const std::vector<float>& GetBenchmarkValues() const { return benchmarkValues; }
		[[nodiscard]] const std::string& GetBenchmarkTitle() const { return sectionTitle; }

		bool operator==(const BenchmarkValue& other) const
		{
			return sectionTitle == other.sectionTitle;
		}

		bool operator==(const std::string& other) const
		{
			return sectionTitle == other;
		}

		void StartTimer();
		void StopTimer();

	protected:

		std::chrono::time_point<std::chrono::high_resolution_clock> startTimepoint;

		std::string sectionTitle;

		std::vector<float> benchmarkValues;
	};
}
