#pragma once

// std
#include <vector>
#include <string>

#include "../Flag.h"

// primary source : http://donw.io/post/reflection-cpp-1/

typedef char char_32[32];
typedef char char_256[256];

namespace Core // todo : have reflection figure out at compile time. (consteval)
{
    long long CustomHash(const char* str);
    constexpr long long ConstexprCustomHash(const char* str) // todo: make ConstexprCustomHash and CustomHash equivalent (warning: pointer(*))
    {
        const int MAX_CHAR = 7;

        unsigned long long int hash = 0;
        unsigned long long int buffer = 0;

        int charCount = 0;
        int shiftValue = 2 + 64 - 8;

        while (*str != 0 && charCount < MAX_CHAR)
        {
            buffer = *str;
            buffer = buffer << shiftValue;
            hash ^= buffer;

            str++;
            charCount++;
            shiftValue -= 8;
        }

        shiftValue = 8;
        while (*str != 0)
        {
            buffer = *str;
            buffer = buffer << shiftValue;
            hash ^= buffer;

            str++;
            charCount++;
            shiftValue -= 8;
            if (shiftValue < 9)
            {
                shiftValue += 64 - 8 + 1;
            }
        }

        return hash;
    }

    long long GetHashWithoutConst(std::string type); // todo: find cleaner way to name/use this function



    void InitReflection(); // Finish setuping value into const variable. This cannot be avoid because the order of static instanciation cannot be controlled
    void InitReflectionOf(const struct Type* type);
    void InitStructureReflection(class Structure* type);
    void InitFunctionReflection();



    enum class ETypeCategory
    {
        WRAPPER, // (Libmaths::Vector3, Libmaths::Degree, void(*func)(), ...)
        POINTER, // currently unsupported
        PRIMITIVE, // (int , bool, char, ...)
        STRUCTURE,
        COLLECTION, // currently unsupported (std::vector, std::List, std::queue, ...)
        ENUMERATION,
    };



    struct  Hash // todo: replace [long long int Name::hash] by [Hash hash]
    {
        Hash(long long int pValue) :
            value(pValue)
        {}

        bool operator==(const Hash& other) const { return value == other.value; }

        long long int value = 0;
    };



    struct  Name
    {
        Name(const std::string& pName) :
            text(pName),
            hash(ConstexprCustomHash(pName.c_str()))
        {}

        bool operator==(const Name& other) const { return hash == other.hash; }

        const long long int hash;
        std::string const text;
    };



    struct Type
    {
        Type(const std::string& pName, ETypeCategory pCategory, int pSize, int step) :
            name(pName),
            category(pCategory),
            size(pSize),
            step(step)
        {
            TypeDataBase.emplace_back(this);
        }

        bool operator==(const Type& other) const { return name == other.name; }

        const Name name;
        const int size;
        const int step;
        const ETypeCategory category;


        static const std::vector<Type*>& GetAll() { return TypeDataBase; } // todo: return vector of CONST type*
        static const Type* Find(long long type);
        static const Type* Find(const Name& name) { return Find(name.hash); }
        static const Type* Find(const char* name) { return Find(ConstexprCustomHash(name)); }
        static const Type* Find(const std::string& name) { return Find(ConstexprCustomHash(name.c_str())); } // todo : switch to CustomHash when ConstexprCustomHash and CustomHash are equivalent

        static const Type NO_TYPE;

    private:
        friend void Core::InitReflection(); // so Core::InitReflection() can access Type::TypeDataBase

        inline static std::vector<Type*> TypeDataBase;

    };



    struct FieldFlag : public Flag<FieldFlag> // todo: use FieldFlag more
    {
        static inline const Flag CONSTANT = NewFlag();
        static inline const Flag POINTER = NewFlag();
        static inline const Flag TRANSIENT = NewFlag();
        static inline const Flag READONLY = NewFlag();
        static inline const Flag STATIC = NewFlag();
        static inline const Flag EDITOR_HIDDEN = NewFlag();
    };


    struct Field
    {
        Field(const std::string& pName, const std::string& pType, int pOffset, FieldFlag pFlags = FieldFlag::NONE) :
            name(pName),
            type((Type*)ConstexprCustomHash(pType.c_str())),
            offset(pOffset),
            flags(pFlags)
        {} // type will be change from hash to pointer in InitRefelction().

        const Name name;
        const Type* const type;
        const int offset;
        const FieldFlag flags;
    };



    struct Constant
    {
        Constant(const std::string& name, char value) :
            name(name),
            value(value)
        {}

        const Name name;
        const char value;
    };
}
