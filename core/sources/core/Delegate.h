#pragma once
#include "functional"

#define DELEGATE_INTERNAL(DEL, DEL_NAME, PARAMS, PARAM_NAMES, PARAMS_COUNT) \
class On##DEL_NAME{ \
	public:  \
		std::vector<DEL> delegates; \
		static constexpr int paramsCount = PARAMS_COUNT;\
	public: \
		On##DEL_NAME(){delegates.reserve(2);} \
		On##DEL_NAME & operator +=(DEL func) { delegates.push_back(func); return *this; } \
		void Add(DEL func) { delegates.push_back(func); }\
		void Broadcast PARAMS { for (int __DEL##INDEX = 0; \
		__DEL##INDEX < delegates.size() && delegates.size() < 50; __DEL##INDEX++) (delegates[__DEL##INDEX])PARAM_NAMES; } \
		void ClearDelegates()\
		{\
		    delegates.clear();\
		}\
		template <typename A, typename B>\
        void Add(A function, B object) {\
            if constexpr (paramsCount == 0) { Add(std::bind(function, object)); return; }\
            if constexpr (paramsCount == 1) { Add(std::bind(function, object, std::placeholders::_1)); return; }\
            if constexpr (paramsCount == 2) { Add(std::bind(function, object, std::placeholders::_1, std::placeholders::_2)); return; }\
            if constexpr (paramsCount == 3) { Add(std::bind(function, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)); return; }\
            if constexpr (paramsCount == 4) { Add(std::bind(function, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)); return; }\
            if constexpr (paramsCount == 5) { Add(std::bind(function, object, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5)); return; }\
        }\
		template <typename A, typename B, typename C>\
        void Add(A function, B object, C firstArgument) {\
		    Add(std::bind(function, object, firstArgument));\
		}\
		template <typename A, typename B, typename C, typename D>\
        void Add(A function, B object, C firstArgument, D secondArgument) {\
		    Add(std::bind(function, object, firstArgument, secondArgument));\
		}\
	};

#define DELEGATE(DELEGATE)\
	DELEGATE_INTERNAL(std::function<void(void)>, DELEGATE, (), (), 0)
#define DELEGATE_One_Param(DELEGATE, type, arg) \
    DELEGATE_INTERNAL(std::function<void(type)>, DELEGATE, (type arg), (arg), 1)
#define DELEGATE_Two_Params(DELEGATE, type, arg, type2, arg2) \
    DELEGATE_INTERNAL(std::function<void(type, type2)>, DELEGATE, (type arg, type2 arg2), (arg, arg2), 2)
#define DELEGATE_Three_Params(DELEGATE, type, arg, type2, arg2, type3, arg3) \
    DELEGATE_INTERNAL(std::function<void(type, type2, type3)>, DELEGATE, (type arg, type2 arg2, type3 arg3), (arg, arg2, arg3), 3)
#define DELEGATE_Four_Params(DELEGATE, type, arg, type2, arg2, type3, arg3, type4, arg4) \
    DELEGATE_INTERNAL(std::function<void(type, type2, type3, type4)>, DELEGATE, (type arg, type2 arg2, type3 arg3, type4 arg4), (arg, arg2, arg3, arg4), 4)
#define DELEGATE_Five_Params(DELEGATE, type, arg, type2, arg2, type3, arg3, type4, arg4, type5, arg5) \
    DELEGATE_INTERNAL(std::function<void(type, type2, type3, type4, type5)>, DELEGATE, (type arg, type2 arg2, type3 arg3, type4 arg4, type5 arg5), (arg, arg2, arg3, arg4, arg5), 5)