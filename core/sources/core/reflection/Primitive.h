#pragma once

#include <string>

#include "ReflectionAPI.h"

#define PRIMITIVE(name) inline static const Core::Type name ## _(# name, ETypeCategory::PRIMITIVE, sizeof(name), sizeof(name));
#define PRIMITIVE_SPACE(name, type) inline static const Core::Type name(# type, ETypeCategory::PRIMITIVE, sizeof(type), sizeof(type));

namespace Core
{
    namespace Primitive // todo: decide if we support commented type with our reflection, and change Config.cpp accordingly, if needed
    {
        //PRIMITIVE(void)
        inline static const Core::Type void_("void", ETypeCategory::PRIMITIVE, 0, 0);
        PRIMITIVE_SPACE(voidPtr, void*);

        PRIMITIVE(bool)

        PRIMITIVE(char)
        PRIMITIVE(short)
        PRIMITIVE(int)
        //PRIMITIVE(long)
        //PRIMITIVE_SPACE(shortInt, short int)
        //PRIMITIVE_SPACE(longInt, long int)
        PRIMITIVE_SPACE(longLong, long long)
        //PRIMITIVE_SPACE(longLongInt, long long int)

        //PRIMITIVE_SPACE(unsignedInt, signed)
        //PRIMITIVE_SPACE(unsignedChar, signed char)
        //PRIMITIVE_SPACE(unsignedShort, signed short)
        //PRIMITIVE_SPACE(unsignedInt, signed int)
        //PRIMITIVE_SPACE(unsignedLong, signed long)
        //PRIMITIVE_SPACE(unsignedShortInt, signed short int)
        //PRIMITIVE_SPACE(unsignedLongInt, signed long int)
        //PRIMITIVE_SPACE(unsignedLongLong, signed long long)
        //PRIMITIVE_SPACE(unsignedLongLongInt, signed long long int)

        //PRIMITIVE_SPACE(unsignedInt, unsigned)
        //PRIMITIVE_SPACE(unsignedChar, unsigned char)
        PRIMITIVE_SPACE(unsignedShort, unsigned short)
        PRIMITIVE_SPACE(unsignedInt, unsigned int)
        //PRIMITIVE_SPACE(unsignedLong, unsigned long)
        //PRIMITIVE_SPACE(unsignedShortInt, unsigned short int)
        //PRIMITIVE_SPACE(unsignedLongInt, unsigned long int)
        PRIMITIVE_SPACE(unsignedLongLong, unsigned long long)
        //PRIMITIVE_SPACE(unsignedLongLongInt, unsigned long long int)

        PRIMITIVE(float)
        PRIMITIVE(double)
            
        //PRIMITIVE(char_32)
        inline static const Core::Type char_32_("char_32", ETypeCategory::PRIMITIVE, sizeof(char_32), sizeof(char));
        //PRIMITIVE(char_256)
        inline static const Core::Type char_256_("char_256", ETypeCategory::PRIMITIVE, sizeof(char_256), sizeof(char));

        // todo:  refactor for std::string
        struct StringStepTest { char c; std::string s; };
        inline static const Core::Type string_("string", ETypeCategory::PRIMITIVE, sizeof(std::string), sizeof(StringStepTest) - sizeof(std::string));
        inline static const Core::Type stdString_("std::string", ETypeCategory::PRIMITIVE, sizeof(std::string), sizeof(StringStepTest) - sizeof(std::string));
    };
}
