#pragma once

#include "Vector/Vector3.h"
#include "Matrix/Matrix4.h"
#include <string>


class DebugWatchValue
{
public:

	DebugWatchValue(std::string title, const int* value);
	DebugWatchValue(std::string title, const unsigned long long* value);
	DebugWatchValue(std::string title, const float* value);
	DebugWatchValue(std::string title, const LibMath::Vector3* value);
	DebugWatchValue(std::string title, const LibMath::Matrix4* value);

	[[nodiscard]] std::string GetFormattedText() const;

private:

	enum class DebugType
	{
		T_INT,
		T_ULONG,
		T_FLOAT,
		T_VECTOR3,
		T_MATRIX4,

		T_ERROR
	};

	std::string label;

	DebugType debugType = DebugType::T_ERROR;

	const void* watchValue;
};
