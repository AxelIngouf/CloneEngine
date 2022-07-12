#pragma once

#include <string>
#include <vector>

#include "BlackMagic.h"
#include "../Array.h"
#include "ReflectionAPI.h"

namespace Core
{
	struct Result
	{
		Result(void* buffer, const Type* resultType) :
			Data(buffer), MetaData(resultType)
		{}

		void* Data;
		const Type* MetaData;
	};

	struct Parameter
	{
		Parameter(const std::string& pType, const std::string& pName) :
			name(pName.c_str()),
			type((Type*)GetHashWithoutConst(pType)),
			offset(-1)
		{}

		const Name name;
		const Type* const type;
		const int offset;
	};

	struct Function
	{
	public:
		Function() = delete;
		Function(const std::string& pName, const std::string& pType, bool pIsStatic = false);
		Function(const std::string& pName, const std::string& pType, std::initializer_list<Parameter> pParams, bool pIsStatic = false);

		Result Invoke(void* ref) const;
		const Function& SetParam(const std::string& paramName, const void* value) const;

		template<typename V>
		const Function& SetParamValue(const std::string& paramName, V value) const
		{
			return SetParam(paramName, &value);
		}

		const Name name;
		const Type* type;
		const bool isStatic; // todo: better (use flags ???)

	protected:
		virtual void InvokeImpl(void* ref) const = 0; // todo: use lambda ??
		void* GetParamValue(const std::string& paramName) const;

	private:
		friend void InitStructureReflection(class Structure* type);
		friend void InitFunctionReflection();
		friend void InitReflection();

		std::vector<Parameter> m_parameters;

		inline static char* bufferData;

	protected:

		inline static char* const& buffer = bufferData;

	};
}



#define UNFOLD_PARAMETER(__MACRO__, __parameter_type__, __parameter_name__, ...)\
    __MACRO__(__parameter_type__, __parameter_name__)\
    IF_ELSE(HAS_ARGS(__VA_ARGS__))\
        (DEFER1(UNFOLD_2ND_PARAMETER)(__MACRO__, __VA_ARGS__))\
    /*ELSE*/\
        (/* Do nothing, just terminate */)

#define UNFOLD_2ND_PARAMETER(__MACRO__, __parameter_type__, __parameter_name__, ...)\
    , __MACRO__(__parameter_type__, __parameter_name__)\
    IF_ELSE(HAS_ARGS(__VA_ARGS__))\
        (DEFER2(_UNFOLD_2ND_PARAMETER)()(__MACRO__, __VA_ARGS__))\
    /*ELSE*/\
        (/* Do nothing, just terminate */)

#define _UNFOLD_2ND_PARAMETER() UNFOLD_2ND_PARAMETER



#define _UNPACK(__parameter_type__, __parameter_name__) __parameter_type__ __parameter_name__
#define _UNPACK_TYPE(__parameter_type__, __parameter_name__) __parameter_type__
#define _PARAMETER(__parameter_type__, __parameter_name__) Core::Parameter(STRINGYFY(__parameter_type__), STRINGYFY(__parameter_name__))
#define _USE_PARAMETER(__parameter_type__, __parameter_name__) *(__parameter_type__*)GetParamValue(STRINGYFY(__parameter_name__))



// todo: static function
#define FUNCTION(__return_type__, __function_name__, ...) _FUNCTION(__return_type__, __function_name__, __VA_ARGS__)
#define _BASE_FUNCTION(...)
#define _DATA_FUNCTION(__return_type__, __function_name__, ...) public: __return_type__ __function_name__ (IF_ELSE(HAS_ARGS(__VA_ARGS__)) (DEFER2(UNFOLD_PARAMETER)(_UNPACK, __VA_ARGS__)) (/*inside else*/));
#define _FUNC_FUNCTION(...) __VA_ARGS__
#define _BASE_META_FUNCTION(...)
#define _FIELD_META_FUNCTION(...)
#define _STATIC_META_FUNCTION(...)
#define _FUNCTION_META_FUNCTION(__return_type__, __function_name__, ...) __function_name__
#define _GENERAL_META_FUNCTION(...)



#define PRIVATE_FUNCTION(__return_type__, __function_name__, ...) _PRIVATE_FUNCTION(__return_type__, __function_name__, __VA_ARGS__)
#define _BASE_PRIVATE_FUNCTION(...)
#define _DATA_PRIVATE_FUNCTION(__return_type__, __function_name__, ...) private: __return_type__ __function_name__ (IF_ELSE(HAS_ARGS(__VA_ARGS__)) (DEFER2(UNFOLD_PARAMETER)(_UNPACK, __VA_ARGS__)) (/*inside else*/));
#define _FUNC_PRIVATE_FUNCTION(...) __VA_ARGS__
#define _BASE_META_PRIVATE_FUNCTION(...)
#define _FIELD_META_PRIVATE_FUNCTION(...)
#define _STATIC_META_PRIVATE_FUNCTION(...)
#define _FUNCTION_META_PRIVATE_FUNCTION(__return_type__, __function_name__, ...) __function_name__
#define _GENERAL_META_PRIVATE_FUNCTION(...)



#define _FUNCTION_META_BODY(__struct_name__, __function_name__) .AddFunction(new __struct_name__::CAT(__function_name__, Function)<int>())

#define _FUNC_BODY(__struct_name__, __return_type__, __function_name__, ...) \
template<class T> struct CAT(__function_name__, Function) : public Core::Function /*\n*/\
{ /*\n*/\
	CAT(__function_name__, Function)() : /*\n*/\
	Core::Function(STRINGYFY(__function_name__), STRINGYFY(__return_type__), { IF_ELSE(HAS_ARGS(__VA_ARGS__)) (DEFER2(UNFOLD_PARAMETER)(_PARAMETER, __VA_ARGS__)) () }) /*\n*/\
	{} /*\n*/\
 /*\n*/\
	void InvokeImpl(void* ref) const override /*\n*/\
	{ /*\n*/\
		if constexpr (std::is_same_v<std::invoke_result_t<decltype(&__struct_name__::__function_name__), __struct_name__, IF_ELSE(HAS_ARGS(__VA_ARGS__)) (DEFER2(UNFOLD_PARAMETER)(_UNPACK_TYPE, __VA_ARGS__)) ()>, void>) /*\n*/\
		{ /*\n*/\
			((__struct_name__*)ref)->__function_name__(IF_ELSE(HAS_ARGS(__VA_ARGS__)) (DEFER2(UNFOLD_PARAMETER)(_USE_PARAMETER, __VA_ARGS__)) ()); /*\n*/\
		} /*\n*/\
		else /*\n*/\
		{ /*\n*/\
			*(__return_type__*)buffer = ((__struct_name__*)ref)->__function_name__(IF_ELSE(HAS_ARGS(__VA_ARGS__)) (DEFER2(UNFOLD_PARAMETER)(_USE_PARAMETER, __VA_ARGS__)) ()); /*\n*/\
		} /*\n*/\
	} /*\n*/\
};



// any function that should not be reflected because the api doesn't provide support for them (yet), ?, probably never, anyway..., :), gl hf, ^^
#define OPERATOR(__return_type__, __function_name__, ...) _OPERATOR(__return_type__, __function_name__, __VA_ARGS__)
#define _BASE_OPERATOR(...)
#define _DATA_OPERATOR(__return_type__, __function_name__, ...) public: __return_type__ __function_name__ (IF_ELSE(HAS_ARGS(__VA_ARGS__)) (DEFER2(UNFOLD_PARAMETER)(_UNPACK, __VA_ARGS__)) (/*inside else*/));
#define _FUNC_OPERATOR(...)
#define _BASE_META_OPERATOR(...)
#define _FIELD_META_OPERATOR(...)
#define _STATIC_META_OPERATOR(...)
#define _FUNCTION_META_OPERATOR(...)
#define _GENERAL_META_OPERATOR(...)



//// todo: constructor
//#define CONSTRUCTOR(...) _CONSTRUCTOR(__VA_ARGS__)
//#define _BASE_CONSTRUCTOR(...)
//#define _DATA_CONSTRUCTOR(__struct_name__, ...) public: __struct_name__(IF_ELSE(HAS_ARGS(__VA_ARGS__)) (DEFER2(UNFOLD_PARAMETER)(_UNPACK, __VA_ARGS__)) (/*inside else*/));
//#define _FUNC_CONSTRUCTOR(...)
//#define _FIELD_META_CONSTRUCTOR(...)
//#define _FUNCTION_META_CONSTRUCTOR(...)
//#define _GENERAL_META_CONSTRUCTOR(...)
