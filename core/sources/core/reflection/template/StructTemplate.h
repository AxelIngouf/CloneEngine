#pragma once

#include "../StructMeta.h"
#include "EEnumTemplate.h"
#include "Vector/Vector.h"

namespace Core
{
    namespace Reflection
    {
        Core::Structure* BaseStructMeta();
    }

    struct BaseStruct
    {
        int foo;
        float bar;

        inline static int val = 1;

        friend Core::Structure* Reflection::BaseStructMeta();
        inline static const Core::Structure* const MetaData = Reflection::BaseStructMeta();
    };

    inline Core::Structure* Reflection::BaseStructMeta()
    {
        return Core::StructBuilder("BaseStruct", sizeof(BaseStruct))
            .AddStaticField("int", "val", &BaseStruct::val)
            .AddField("int", "foo", (int)(long long)&((BaseStruct*)0)->foo)
            .AddField("float", "bar", (int)(long long)&((BaseStruct*)0)->bar)
            .Finish();
    };

    namespace Reflection
    {
        Core::Structure* CustomStructMeta();
    }

    struct CustomStruct
    {
        bool isYes;
        float number;
        float otherNumber;
        int numberOfNumber;

        friend Core::Structure* Reflection::CustomStructMeta();
        inline static const Core::Structure* const MetaData = Reflection::CustomStructMeta();
    };

    inline Core::Structure* Reflection::CustomStructMeta()
    {
        return Core::StructBuilder("CustomStruct", sizeof(CustomStruct))
            .AddField("bool", "isYes", (int)(long long)&((CustomStruct*)0)->isYes)
            .AddField("float", "number", (int)(long long)&((CustomStruct*)0)->number)
            .AddField("float", "otherNumber", (int)(long long)&((CustomStruct*)0)->otherNumber)
            .AddField("int", "numberOfNumber", (int)(long long)&((CustomStruct*)0)->numberOfNumber)
            .Finish();
    };

    namespace Reflection
    {
        Core::Structure* StructTemplateMeta();
    }

    struct StructTemplate : public BaseStruct
    {
        int value;
        LibMath::Vector3 vector;
        EEnumTemplate eValue;
        CustomStruct tryStruct;

    public:
        int GetValue();
    private:
        template<class T>
        struct GetValueFunctionObject : public Core::Function
        {
            GetValueFunctionObject() :
                Function("GetValue", "int")
            {}

            void InvokeImpl(void* ref) const override
            {
                if constexpr (std::is_same<std::invoke_result_t<decltype(&StructTemplate::GetValue), StructTemplate>, void>::value)
                {
                    ((StructTemplate*)ref)->GetValue();
                }
                else
                {
                    *(int*)buffer = ((StructTemplate*)ref)->GetValue();
                }
            }
        };


    public:
        void SetValue(int newValue);
    private:
        template<class T>
        struct SetValueFunctionObject : public Core::Function
        {
            SetValueFunctionObject() :
                Function("SetValue", "void", { Core::Parameter("int", "newValue") })
            {}

            void InvokeImpl(void* ref) const override
            {
                if constexpr (std::is_same_v<std::invoke_result_t<decltype(&StructTemplate::SetValue), StructTemplate, int>, void>)
                {
                    ((StructTemplate*)ref)->SetValue(*(int*)GetParamValue("newValue"));
                }
                else
                {
                    *(void*)buffer = ((StructTemplate*)ref)->SetValue(*(int*)GetParamValue("newValue"));
                }
            }
        };


    public:
        static int AddOneTo(int number);
    private:
        template<class T>
        struct AddOneToFunctionObject : public Function
        {
            AddOneToFunctionObject() :
                Function("AddOneTo", "int", { Parameter("int", "number") }, true)
            {}

            void InvokeImpl(void*) const override
            {
                if constexpr (std::is_same_v<std::invoke_result_t<decltype(&StructTemplate::SetValue), StructTemplate, int>, void>)
                {
                    AddOneTo(*(int*)GetParamValue("number"));
                }
                else
                {
                    *(int*)buffer = AddOneTo(*(int*)GetParamValue("number"));
                }
            }
        };


    public:
        friend Core::Structure* Reflection::StructTemplateMeta();
        inline static const Core::Structure* const MetaData = Reflection::StructTemplateMeta();
    };

    inline Core::Structure* Reflection::StructTemplateMeta()
    {
        return Core::StructBuilder("StructTemplate", sizeof(StructTemplate))
            .AddBase("BaseStruct", 0)
            .AddField("int", "value", (int)(long long)&((StructTemplate*)0)->value)
            .AddField("LibMath::Vector3", "vector", (int)(long long)&((StructTemplate*)0)->vector)
            .AddField("EEnumTemplate", "eValue", (int)(long long)&((StructTemplate*)0)->eValue)
            .AddField("CustomTemplate", "tryStruct", (int)(long long)&((StructTemplate*)0)->tryStruct)
            .AddFunction(new StructTemplate::GetValueFunctionObject<int>())
            .AddFunction(new StructTemplate::SetValueFunctionObject<int>())
            .AddFunction(new StructTemplate::AddOneToFunctionObject<int>())
            .Finish();
    };
}