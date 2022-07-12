#include "DebugWatchValue.h"


#include <iomanip>
#include <sstream>

DebugWatchValue::DebugWatchValue(std::string title, const int* value) :
	label(std::move(title)), debugType(DebugType::T_INT), watchValue(value)
{
}

DebugWatchValue::DebugWatchValue(std::string title, const unsigned long long* value) :
	label(std::move(title)), debugType(DebugType::T_ULONG), watchValue(value)
{
}

DebugWatchValue::DebugWatchValue(std::string title, const float* value) :
	label(std::move(title)), debugType(DebugType::T_FLOAT), watchValue(value)
{
}

DebugWatchValue::DebugWatchValue(std::string title, const LibMath::Vector3* value) :
	label(std::move(title)), debugType(DebugType::T_VECTOR3), watchValue(value)
{
}

DebugWatchValue::DebugWatchValue(std::string title, const LibMath::Matrix4* value) :
	label(std::move(title)), debugType(DebugType::T_MATRIX4), watchValue(value)
{
}

std::string DebugWatchValue::GetFormattedText() const
{
	const std::string prefix = label + ": ";

	std::ostringstream stream;


	if (watchValue)
	{
		switch (debugType)
		{
		case DebugType::T_INT:
			return prefix + std::to_string(*static_cast<const int*>(watchValue));
		case DebugType::T_ULONG:
			return prefix + std::to_string(*static_cast<const unsigned long long*>(watchValue));
		case DebugType::T_FLOAT:
			return prefix + std::to_string(*static_cast<const float*>(watchValue));
		case DebugType::T_VECTOR3:
			stream << std::fixed << std::setprecision(2) << "(" <<
				static_cast<const LibMath::Vector3*>(watchValue)->x << ", " <<
				static_cast<const LibMath::Vector3*>(watchValue)->y << ", " <<
				static_cast<const LibMath::Vector3*>(watchValue)->z << ")";
			return prefix + stream.str();
		case DebugType::T_MATRIX4:
			stream << std::fixed << std::setprecision(2);

			for (int i = 0; i < 4; i++)
			{
				stream << "\n\t";
				for (int j = 0; j < 4; j++)
				{
					stream << std::left << std::setw(7) <<
						static_cast<const LibMath::Matrix4*>(watchValue)->raw[i * 4 + j];
				}
			}

			return prefix + stream.str();


		case DebugType::T_ERROR:
			break;
		}
	}
	return prefix + "ERROR, NO TYPE OR NULLPTR";
}
