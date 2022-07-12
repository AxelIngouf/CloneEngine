#pragma once

#include <stack>

#include "ReflectionAPI.h"

namespace Core
{
    struct StructureField
    {
        StructureField() = default;
        StructureField(const Field& field, int structureOffset) :
            name(&field.name),
            type(field.type),
            offset(field.offset + structureOffset),
            flags(field.flags)
        {}
        StructureField(const StructureField& field, int structureOffset) :
            name(field.name),
            type(field.type),
            offset(field.offset + structureOffset),
            flags(field.flags)
        {}

        StructureField& operator=(const StructureField& other);

        void* GetFrom(const void* instance) const
        {
            return (char*)instance + offset;
        }

        void SetFor(const void* instance, const void* value) const
        {
            memcpy((char*)instance + offset, value, type->size);
        }

        template<typename R, typename I>
        R& GetValue(const I& instance) const
        {
            if constexpr (std::is_same<I, void*>::value)
            {
                return *(R*)GetFrom(instance);
            }
            else
            {
                return *(R*)GetFrom(&instance);
            }
        }

        template<typename I, typename V>
        inline void SetValue(const I& instance, const V& value) const
        {
            if constexpr (std::is_same<I, void*>::value)
            {
                SetFor(instance, &value);
            }
            else
            {
                SetFor(&instance, &value);
            }
        }

        bool IsValid() { return type; }

        const Name* const name = nullptr;
        const Type* const type = nullptr;
        const int offset = 0;
        const FieldFlag flags = FieldFlag::NONE;
    };


    struct StaticField
    {
        StaticField() = delete;
        StaticField(const std::string& pName, const std::string& pType, void* pData) :
            name(pName),
            type((Type*)ConstexprCustomHash(pType.c_str())),
            data(pData)
        {} // type will be change from hash to pointer in InitRefelction().

        void* Get() const
        {
            return data;
        }

        void Set(const void* value) const
        {
            memcpy(data, value, type->size);
        }

        template<typename R>
        R& GetValue() const
        {
            return *(R*)Get();
        }

        template<typename V>
        inline void SetValue(const V& value) const
        {
            Set(&value);
        }

        bool IsValid() { return type; }

        const Name name = nullptr;
        const Type* const type = nullptr;
        void* const data = nullptr;
    };



    class FieldIterator;
    struct StructureBase
    {
        StructureBase(const std::string& pType, int pOffset) :
            type((Type*)ConstexprCustomHash(pType.c_str())),
            offset(pOffset)
        {} // type will be change from hash to pointer in InitRefelction().

        const Type* const type;
        const int offset;

    private:
        friend FieldIterator;
        StructureBase(const Type* pType, int pOffset) : // todo: remove (use in FieldIterator::ChangeType())
            type(pType),
            offset(pOffset)
        {}
    };



    class Structure;
    class FieldIterator
    {
    public:
        FieldIterator(const Structure* type);

        bool operator!=(const FieldIterator& rhs) const;
        StructureField operator*() const;
        void operator++();

    private:
        void ChangeType();

        const Structure* m_type;
        int m_index = 0;
        int m_offset = 0;

        std::stack<std::pair<StructureBase, int>> m_stack;
    };



    class FieldIterable
    {
    public:
        FieldIterable(const Structure* type) :
            m_type(type)
        {}

        FieldIterator begin() { return FieldIterator(m_type); }
        FieldIterator end() { return FieldIterator(nullptr); }

    private:
        const Structure* m_type;

    };
}
