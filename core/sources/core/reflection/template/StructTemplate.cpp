#include "StructTemplate.h"

namespace Core
{
    int StructTemplate::GetValue()
    {
        return value;
    }

    void StructTemplate::SetValue(int newValue)
    {
        value = newValue;
    }

    int StructTemplate::AddOneTo(int number)
    {
        return number + 1;
    }
}