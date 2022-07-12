#include "ReflectionAPI.h"

#include "Primitive.h"
#include "Wrapper.h"
#include "StructMeta.h"

#include <string> // memcpy
#include <algorithm> // remove_if

#include "../CLog.h"

namespace Core
{
    long long CustomHash(const char* str)
    {
        // only copying 7 char at a time to leave a 1 char (8 bits) of wiggling space for bitshift scrambling
        const int MAX_CHAR = 7;

        unsigned long long hash = 0;
        int index = 0;

        while (str[index] != 0 && index != MAX_CHAR)
        {
            index++;
        }

        memcpy(&hash, str, index); // "write" the 7 first char into the hash
        str += MAX_CHAR;
        index = 0;

        // since this hash is intended for c++ variable name, str is assumed to start with a letter or an underscore
        // since ascii are 0-127, last bit(8th) is always 0
        // since all letter and the underscore have an ascii value greater then 64, they have a 1 in the 6th bit
        // shifting the hash two bit left will align at least one 1 (from the first letter) with a 0 of the next pass
        // ensuring no collision between any str of 14 char or less
        hash = hash << 2;

        // since the hash has been shifted two bit left, both extreme right bit of the hash are 0s
        // fusing the next 7 char to the left of the hash leave those two 0s has 0s
        // ensuring no possible collision between any str of 21 to 15 char and any str of 14 char or less 3 time out of 4 (actually 48 / 63)
        int shiftValue = 8;
        unsigned long long int buffer = 0;
        while (str[index] != 0)
        {
            index++;
            if (index == MAX_CHAR)
            {
                memcpy(&buffer, str, index); // "write" the 7 next char into the buffer
                buffer = buffer << shiftValue; // bitshift buffer for scrambling effect
                hash ^= buffer; // fuse buffer to hash
                str += MAX_CHAR;
                index = 0;
                shiftValue++;
                if (shiftValue > 8)
                {
                    shiftValue = 0;
                }
            }
        }

        memcpy(&buffer, str, index); // add last char to hash
        buffer = buffer << shiftValue;
        hash ^= buffer;

        return hash;
    }


    //constexpr long long ConstexprCustomHash(const char* str) // todo: make ConstexprCustomHash and CustomHash equivalent
    //{
    //    const int MAX_CHAR = 7;

    //    unsigned long long int hash = 0;
    //    unsigned long long int buffer = 0;

    //    int charCount = 0;
    //    int shiftValue = 2 + 64 - 8;

    //    while (*str != 0 && charCount < MAX_CHAR)
    //    {
    //        buffer = *str;
    //        buffer = buffer << shiftValue;
    //        hash ^= buffer;

    //        str++;
    //        charCount++;
    //        shiftValue -= 8;
    //    }

    //    shiftValue = 8;
    //    while (*str != 0)
    //    {
    //        buffer = *str;
    //        buffer = buffer << shiftValue;
    //        hash ^= buffer;

    //        str++;
    //        charCount++;
    //        shiftValue -= 8;
    //        if (shiftValue < 9)
    //        {
    //            shiftValue += 64 - 8 + 1;
    //        }
    //    }

    //    return hash;
    //}


    long long GetHashWithoutConst(std::string type)
    {
        size_t pos = std::string::npos;
        while ((pos = type.find("const")) != std::string::npos)
        {
            type.erase(pos, 5);
        }
        type.erase(std::remove_if(type.begin(), type.end(), ::isspace), type.end());

        return ConstexprCustomHash(type.c_str());
    }

    std::vector<Function*>* g_functions; // todo : move global and function inside a class
    int g_bufferSizeNeeded = 0;
    void InitReflection()
    {
        static bool alreadyInit = false;
        if (alreadyInit)
        {
            return;
        }

        g_functions = new std::vector<Function*>;

        for (const Type* type : Type::TypeDataBase)
        {
            // todo: deal with duplicate
            InitReflectionOf(type);
        }

        InitFunctionReflection(); // function initialize after everything else to avoid circular init

        delete g_functions;

        Function::bufferData = new char[g_bufferSizeNeeded];

        alreadyInit = true;
    }


    void InitReflectionOf(const Type* type)
    {
        switch (type->category)
        {
        case ETypeCategory::STRUCTURE:
            InitStructureReflection((Structure*)type);
            break;
        case ETypeCategory::PRIMITIVE:
        case ETypeCategory::ENUMERATION:
            // do nothing;
            break;
        case ETypeCategory::COLLECTION:
        case ETypeCategory::POINTER:
        case ETypeCategory::WRAPPER:
        default:
            // not yet implemented;
            // tolog
            break;
        }
    }

    void InitStructureReflection(Structure* type)
    {
        if (type->step >= 0)
            return; // Structure already initialized

        for (StructureBase& base : type->m_bases)
        {
            const Type* typeFound = Type::Find((long long)base.type);
            *(const Type**)&base.type = typeFound;

            if (typeFound->step < 0) // todo: deal with Type::Find() return nullptr 
                InitReflectionOf(typeFound);

            if (type->step < typeFound->step)
                *(int*)&type->step = typeFound->step;
        }

        // fields
        for (Field& field : type->m_fields)
        {
            const Type* typeFound = Type::Find((long long)field.type);
            *(const Type**)&field.type = typeFound;

            if (typeFound->step < 0) // todo: deal with Type::Find() return nullptr 
                InitReflectionOf(typeFound);

            if (type->step < typeFound->step)
                *(int*)&type->step = typeFound->step;
        }

        // fields
        for (StaticField& field : type->m_staticFields)
        {
            const Type* typeFound = Type::Find((long long)field.type);
            *(const Type**)&field.type = typeFound;
        }

        // functions
        g_functions->insert(g_functions->end(), type->m_functions.begin(), type->m_functions.end());
    }

    void InitFunctionReflection()
    {
        for (Function* function : *g_functions)
        {
            int nextOffset = 0;

            const Type* typeFound = Type::Find((long long)function->type);
            *(const Type**)&function->type = typeFound;

            for (Parameter& parameter : function->m_parameters)
            {
                typeFound = Type::Find((long long)parameter.type);
                *(const Type**)&parameter.type = typeFound;

                while (nextOffset % typeFound->step != 0) // add padding
                    nextOffset++;

                *(int*)&parameter.offset = nextOffset;

                nextOffset += typeFound->size;
            }

            int bufferSizeNeeded = typeFound->size > nextOffset ? typeFound->size : nextOffset;
            if (g_bufferSizeNeeded < bufferSizeNeeded)
            {
                g_bufferSizeNeeded = bufferSizeNeeded;
            }
        }
    }

    const Type Type::NO_TYPE = Type("", ETypeCategory::PRIMITIVE, 0, 0);

    const Type* Type::Find(long long hash)
    {
        for (Type* type : TypeDataBase)
            if (type->name.hash == hash)
                return type;

        // todo: assert false
        // not found -> wrapper forgotten or class/struct without meta
        return nullptr;
    }
}
