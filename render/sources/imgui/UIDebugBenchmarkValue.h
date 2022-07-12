#pragma once

#include "core/DebugWindow/BenchmarkValue.h"


class UIDebugBenchmarkValue : Core::BenchmarkValue
{
public:

	static void DrawBenchmarkGraph(const BenchmarkValue& value);
};
