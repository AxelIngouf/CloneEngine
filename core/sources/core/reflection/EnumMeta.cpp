#include "EnumMeta.h"

namespace Core
{
    EnumBuilder::EnumBuilder(const char* pName) :
        m_enum((Enumeration*)Type::Find(pName)),
        m_alreadyExist(m_enum)
    {
        if (!m_alreadyExist)
            m_enum = new Enumeration(pName);
    }


    EnumBuilder& EnumBuilder::AddConstant(const char* pName, char value)
    {
        if (m_alreadyExist)
            return *this;

        m_enum->m_constants.emplace_back(pName, value);

        return *this;
    }


    const Constant* Enumeration::FindConstant(const char* pName) const
    {
        long long hash = ConstexprCustomHash(pName); // todo : switch to CustomHash when ConstexprCustomHash and CustomHash are equivalent
        for(const Constant& constant : m_constants)
            if (constant.name.hash == hash)
                return &constant;

        return nullptr;
    }

    const Constant* Enumeration::FindConstant(char pValue) const
    {
        for (const Constant& constant : m_constants)
            if (constant.value == pValue)
                return &constant;

        return nullptr;
    }
}
