#pragma once

#include "BlackMagic.h"
#include "../Array.h"
#include "ReflectionAPI.h"

namespace Core
{
    class EnumBuilder
    {
    public:
         EnumBuilder(const char* pName);
         ~EnumBuilder() = default;

        EnumBuilder() = delete;
        EnumBuilder(const EnumBuilder& other) = delete;
        EnumBuilder(EnumBuilder&& other) = delete;
        EnumBuilder& operator=(const EnumBuilder& other) = delete;
        EnumBuilder& operator=(EnumBuilder&& other) = delete;

         EnumBuilder& AddConstant(const char* pName, char pValue);
         class Enumeration* Finish() { return m_enum; }

    private:
        Enumeration* m_enum;
        bool m_alreadyExist;

        //char c = 1;// todo : int enum, short enum and long long enum
        //short s = 2;
        //int i = 4;
        //long long l = 8;
    };

    class Enumeration : public Type
    {
    public:
         Enumeration(const char* pName) noexcept :
            Type(pName, ETypeCategory::ENUMERATION, 1, 1)
        {}
         ~Enumeration() = default;

        Enumeration() = delete;
        Enumeration(const Enumeration& other) = delete;
        Enumeration(Enumeration&& other) = delete;
        Enumeration& operator=(const Enumeration& other) = delete;
        Enumeration& operator=(Enumeration&& other) = delete;

         const std::string& GetName() const { return name.text; }
         const ArrayView<Constant> GetAllConstants() const { return ArrayView(&m_constants[0], m_constants.size()); }

         const Constant* FindConstant(const char* pName) const;
         const Constant* FindConstant(char pValue) const;

    private:
        friend EnumBuilder;

        std::vector<Constant> m_constants;
    };
}

#define ENUM(name, ...)\
class name/*\n*/\
{/*\n*/\
private:/*\n*/\
    struct EnumValue/*\n*/\
    {/*\n*/\
        constexpr EnumValue(char pValue) : value(pValue) {}/*\n*/\
        EnumValue& operator=(EnumValue other) { value = other.value; return *this; }/*\n*/\
        char value;/*\n*/\
    };/*\n*/\
/*\n*/\
    constexpr static EnumValue CreateValue(char value) { NextValue = value + 1; return EnumValue(value); }/*\n*/\
    constexpr static EnumValue CreateValue() { return EnumValue(NextValue++); }/*\n*/\
/*\n*/\
public:/*\n*/\
    constexpr explicit name() = default;/*\n*/\
    constexpr explicit name(char pValue) : m_value(pValue) {}/*\n*/\
    constexpr name(EnumValue pValue) : m_value(pValue) {}/*\n*/\
    name& operator=(name other) { m_value = other.m_value; return *this; }/*\n*/\
/*\n*/\
    bool operator==(name other) { return m_value.value == other.m_value.value; }/*\n*/\
    bool operator!=(name other) { return m_value.value != other.m_value.value; }/*\n*/\
/*\n*/\
    EVAL(CHOOSE_AND_UNFOLD(_DATA, __VA_ARGS__))\
/*\n*/\
    inline static const Core::Enumeration* MetaData = Core::EnumBuilder(# name)/*\n*/\
        EVAL(CHOOSE_AND_UNFOLD(_META, __VA_ARGS__))\
        .Finish();/*\n*/\
/*\n*/\
private:/*\n*/\
    EnumValue m_value = EnumValue(0);/*\n*/\
/*\n*/\
    inline static char NextValue = 0;/*\n*/\
};

#define CONSTANT(name, ...) _CONSTANT(name, __VA_ARGS__)

#define _DATA_CONSTANT(name, ...) inline static const EnumValue name = CreateValue(__VA_ARGS__);
#define _META_CONSTANT(name, ...) .AddConstant(# name, name ## .value)
