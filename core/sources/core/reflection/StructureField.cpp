#include "StructureField.h"

#include "StructMeta.h"

namespace Core
{
    StructureField& StructureField::operator=(const StructureField& other)
    {
        return *(new StructureField(other, 0));
    }

    FieldIterator::FieldIterator(const Structure* type) :
        m_type(type)
    {
        if (m_type)
        {
            m_stack.emplace(StructureBase(m_type, 0), 0);

            if (m_type->m_fields.size() == 0)
            {
                ChangeType();
            }
        }
    }

    bool FieldIterator::operator!=(const FieldIterator& rhs) const
    {
        return m_type != rhs.m_type || m_index != rhs.m_index || m_offset != rhs.m_offset;
    }

    StructureField FieldIterator::operator*() const
    {
        return StructureField(m_type->m_fields[m_index], m_offset);
    }

    void FieldIterator::operator++()
    {
        ++m_index;
        if (m_index >= m_type->m_fields.size())
        {
            ChangeType();
        }
    }

    void FieldIterator::ChangeType()
    {
        m_index = 0;

        while (m_stack.size())
        {
            StructureBase& structure = m_stack.top().first;
            int index = m_stack.top().second;

            ArrayView<StructureBase> bases = ((Structure*)structure.type)->GetBases();
            if (index < bases.Size())
            {
                m_stack.top().second++;

                if (bases[index].type->category == ETypeCategory::STRUCTURE) // todo : ??? maybe use not a ETypeCategory::WRAPPER
                {
                    m_type = (Structure*)bases[index].type;
                    m_stack.emplace(bases[index], 0);
                    m_offset += bases[index].offset;

                    if (m_type->m_fields.size() == 0)
                    {
                        ChangeType();
                    }
                }
                else
                {
                    continue;
                }
                return;
            }
            else
            {
                m_offset -= m_stack.top().first.offset;
                m_stack.pop();
            }
        }

        m_type = nullptr;
        m_offset = 0;
    }
}
