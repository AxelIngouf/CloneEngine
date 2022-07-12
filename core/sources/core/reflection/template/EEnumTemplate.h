#pragma once

#include "../EnumMeta.h"

namespace Core
{
    class EEnumTemplate
    {
    private:
        struct EnumValue
        {
            constexpr EnumValue(char pValue) : value(pValue) {}
            EnumValue& operator=(EnumValue other) { value = other.value; return *this; }
            char value;
        };

        constexpr static EnumValue CreateValue(char value) { NextValue = value + 1; return EnumValue(value); }
        constexpr static EnumValue CreateValue() { return EnumValue(NextValue++); }

    public:
        constexpr explicit EEnumTemplate() = default;
        constexpr explicit EEnumTemplate(char pValue) : m_value(pValue) {}
        constexpr EEnumTemplate(EnumValue pValue) : m_value(pValue) {}
        EEnumTemplate& operator=(EEnumTemplate other) { m_value = other.m_value; return *this; }

        inline static const EnumValue VALUE_1 = CreateValue();
        inline static const EnumValue VALUE_2 = CreateValue();
        inline static const EnumValue VALUE_3 = CreateValue();

        bool operator==(EEnumTemplate other) { return m_value.value == other.m_value.value; }
        bool operator!=(EEnumTemplate other) { return m_value.value != other.m_value.value; }

        inline static const Core::Enumeration* MetaData = Core::EnumBuilder("EEnumTemplate")
            .AddConstant("VALUE_1", VALUE_1.value)
            .AddConstant("VALUE_2", VALUE_2.value)
            .AddConstant("VALUE_3", VALUE_3.value)
            .Finish();

    private:
        EnumValue m_value = EnumValue(0);

        inline static char NextValue = 0;
    };
}