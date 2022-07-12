#include "StructMeta.h"

namespace Core
{
    StructBuilder::StructBuilder(const char* pName, int pSize) :
        m_struct((Structure*)Type::Find(pName)),
        m_alreadyExist(m_struct != nullptr)
    {
        if (m_alreadyExist == false)
        {
            m_struct = new Structure(pName, pSize);
        }
    }


    StructBuilder& StructBuilder::AddBase(const char* pType, int pOffset)
    {
        if (m_alreadyExist) return *this;

        m_struct->m_bases.emplace_back(pType, pOffset); // base will be change from hash to pointer in InitRefelction().

        return *this;
    }

    StructBuilder& StructBuilder::AddField(const char* pType, const char* pName, int pOffset, FieldFlag pFlags)
    {
        if (m_alreadyExist) return *this;

        size_t index = 0;
        char* name = nullptr;

        while (pName[index] != '\0') // remove default value // todo: better
        {
            if (pName[index] == '=')
            {
                name = new char[index + 1];
                name[index] = '\0';

                for (int i = 0; i < index; i++)
                {
                    if (isspace(pName[i]))
                    {
                        name[i] = '\0';
                        break;
                    }
                    else
                    {
                        name[i] = pName[i];
                    }
                }

                break;
            }

            index++;
        }

        if (name)
        {
            m_struct->m_fields.emplace_back(name, pType, pOffset, pFlags);
        }
        else
        {
            m_struct->m_fields.emplace_back(pName, pType, pOffset, pFlags);
        }

        return *this;
    }

    StructBuilder& StructBuilder::AddStaticField(const char* pType, const char* pName, void* pPtr)
    {
        if (m_alreadyExist) return *this;

        m_struct->m_staticFields.emplace_back(pName, pType, pPtr);

        return *this;
    }

    StructBuilder& StructBuilder::AddFunction(Function* pFunction)
    {
        if (m_alreadyExist) return *this;

        m_struct->m_functions.emplace_back(pFunction);

        return *this;
    }

    StructBuilder& StructBuilder::SetConstructor(Function* pFunction)
    {
        if (m_alreadyExist) return *this;

        m_struct->m_constructor = pFunction;

        return *this;
    }

    StructureField Structure::FindField(long long pHash) const
    {
        for(const Field& field : m_fields)
            if (field.name.hash == pHash)
                return StructureField(field, 0);

        for (StructureBase base : m_bases)
        {
            if (base.type->category == ETypeCategory::STRUCTURE)
            {
                StructureField result = ((Structure*)base.type)->FindField(pHash);

                if (result.IsValid())
                {
                    return StructureField(result, base.offset);
                }
            }
        }

        return StructureField();
    }

    const StaticField* Structure::FindStaticField(long long pHash) const
    {
        for (const StaticField& field : m_staticFields)
            if (field.name.hash == pHash)
                return &field;

        for (StructureBase base : m_bases)
        {
            if (base.type->category == ETypeCategory::STRUCTURE)
            {
                const StaticField* result = ((Structure*)base.type)->FindStaticField(pHash);

                if (result)
                {
                    return result;
                }
            }
        }

        return nullptr;
    }

    const Function* Structure::FindFunction(long long pHash) const
    {
        for (const Function* function : m_functions)
            if (function->name.hash == pHash)
                return function;

        for (StructureBase base : m_bases)
        {
            if (base.type->category == ETypeCategory::STRUCTURE)
            {
                const Function* result = ((Structure*)base.type)->FindFunction(pHash);

                if (result)
                {
                    return result;
                }
            }
        }

        return nullptr;
    }

    bool Structure::Inherit(long long pHash) const
    {
        if (name.hash == pHash)
        {
            return true;
        }

        for (StructureBase base : m_bases)
        {
            if (base.type->category == ETypeCategory::STRUCTURE)
            {
                if (((Structure*)base.type)->Inherit(pHash))
                {
                    return true;
                }
            }
        }

        return false;
    }
}
