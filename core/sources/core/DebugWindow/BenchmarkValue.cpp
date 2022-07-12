#include "BenchmarkValue.h"

namespace Core
{
	BenchmarkValue::BenchmarkValue(std::string title):
		sectionTitle(std::move(title))
	{
		benchmarkValues.resize(benchmarkValueNum, 0.f);
	}

	void BenchmarkValue::StartTimer()
	{
		startTimepoint = std::chrono::high_resolution_clock::now();
	}

	void BenchmarkValue::StopTimer()
	{
		using namespace std::chrono;

		const auto endTimepoint = high_resolution_clock::now();

		const auto start = time_point_cast<microseconds>(startTimepoint).time_since_epoch().count();
		const auto end = time_point_cast<microseconds>(endTimepoint).time_since_epoch().count();

		const auto duration = end - start;
		float ms = static_cast<float>(duration) * 0.001f;

		for (size_t i = 1; i < benchmarkValueNum; i++)
			benchmarkValues[i - 1] = benchmarkValues[i];

		benchmarkValues[benchmarkValueNum - 1] = ms;
	}
}
