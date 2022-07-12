#pragma once

#define IF_IMPLEMENT_FUNCTION_MACRO(FunctionName)\
template <typename T> class Has ## FunctionName\
{\
private:\
	typedef char YesType[1];\
	typedef char NoType[2];\
\
	template <typename C> static YesType& test(decltype(&C:: ## FunctionName ));\
	template <typename C> static NoType& test(...);\
\
public:\
	enum { value = sizeof(test<T>(0)) == sizeof(YesType) };\
};
