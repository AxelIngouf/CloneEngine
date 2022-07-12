#pragma once

// primary source : http://jhnet.co.uk/articles/cpp_magic
//"EXPAND" macro need for visual studio to unpack "__VA_ARGS__" macro properly

#define FIRST(a, ...) a
#define SECOND(a, b, ...) b

#define EMPTY()
#define EXPAND(args) args
#define CAT(a,b) a ## b
#define CAT3(a,b,c) a ## b ## c // todo: CAT(a,b,...)

#define IS_PROBE(...) EXPAND(SECOND(__VA_ARGS__, 0))
#define PROBE() ~, 1

#define _NOT_0 PROBE()
#define NOT(x) IS_PROBE(CAT(_NOT_, x))

#define BOOL(x) NOT(NOT(x))

#define _IF_1_ELSE(...)
#define _IF_0_ELSE(...) __VA_ARGS__
#define _IF_1(...) __VA_ARGS__ _IF_1_ELSE
#define _IF_0(...)             _IF_0_ELSE
#define IF_ELSE(condition) _IF_ELSE(BOOL(condition))
#define _IF_ELSE(condition) CAT(_IF_, condition)

#define _END_OF_ARGUMENTS_() 0
#define HAS_ARGS(...) BOOL(FIRST(_END_OF_ARGUMENTS_ __VA_ARGS__)())

#define EVAL1(...) __VA_ARGS__
#define EVAL2(...) EVAL1(EVAL1(__VA_ARGS__))
#define EVAL4(...) EVAL2(EVAL2(__VA_ARGS__))
#define EVAL8(...) EVAL4(EVAL4(__VA_ARGS__))
#define EVAL16(...) EVAL8(EVAL8(__VA_ARGS__))
#define EVAL32(...) EVAL16(EVAL16(__VA_ARGS__))
#define EVAL64(...) EVAL32(EVAL32(__VA_ARGS__))
#define EVAL128(...) EVAL64(EVAL64(__VA_ARGS__))
#define EVAL256(...) EVAL128(EVAL128(__VA_ARGS__))
#define EVAL512(...) EVAL256(EVAL256(__VA_ARGS__))
#define EVAL1024(...) EVAL512(EVAL512(__VA_ARGS__))
#define EVAL2048(...) EVAL1024(EVAL1024(__VA_ARGS__))
#define EVAL4096(...) EVAL2048(EVAL2048(__VA_ARGS__))
#define EVAL(...) EVAL4096(EVAL4096(__VA_ARGS__))

#define DEFER1(m) m EMPTY()
#define DEFER2(m) m EMPTY EMPTY()()

#define STRINGYFY(...) #__VA_ARGS__








#if 0 // working example use for debugging as a fully fonctionning comparaison

#define MAP(m, first, ...)           \
  m(first)                           \
  IF_ELSE(HAS_ARGS(__VA_ARGS__))(    \
    DEFER2(_MAP)()(m, __VA_ARGS__)   \
  )(                                 \
    /* Do nothing, just terminate */ \
  )
#define _MAP() MAP

#define GREET(x) Hello, x!

//std::cout << EVAL1(DEFER1(STRINGYFY)(EVAL(MAP(GREET, Mum, Dad, Adam, Joe)))) << std::endl << std::endl;

#endif