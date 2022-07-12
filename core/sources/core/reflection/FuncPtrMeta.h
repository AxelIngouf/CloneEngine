#pragma once

#include "BlackMagic.h"
#include "ReflectionAPI.h"

#define FUNCPTR(returnType, name, ...)\
typedef returnType(*name)(__VA_ARGS__);\
namespace Wrapper\
{\
	inline static const Type name(#name, ETypeCategory::WRAPPER, sizeof(void*), sizeof(void*));\
}
