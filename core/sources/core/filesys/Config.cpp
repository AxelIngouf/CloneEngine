#include "Config.h"

#include "core/CLog.h"
#include "ErrorManager.h"
#include "core/reflection/StructMeta.h"

#include <string>
#include <cstdlib>

#define CPP_MAX_NAME_LENGHT 255ul// todo: move to appropriate location
#define ERROR_BUFFER_SIZE 256ul// todo: move to appropriate location
#define INVALID_CHAR '\200' // first value beyond ascii table // todo: move to appropriate location
#define not !
#define is ==
#define isnot !=
#define and &&
#define or ||

namespace Core
{
    void Config::Parse(const char* filePath, void* out, const Structure* metaData)
    {
        ParseMemory memory(filePath);

        while (memory.ptr < memory.endPtr)
        {
            StructureField field = ParseField(&memory, metaData);

            if (field.IsValid())
            {
                if (*memory.ptr is '=')
                {
                    memory.ptr++; // skip '='
                    SkipToNextSignificant(&memory);

                    ParseValue(&memory, field, out);

                    if (*memory.ptr != ';')
                    {
                        LogError(&memory, EError::MISSING_SEMICOLON);
                    }
                }
                else
                {
                    LogError(&memory, EError::UNASSIGNED_VARIABLE);
                }
            }

            SkipToNextCommand(&memory);
        }
    }


    void Config::LogError(ParseMemory* memory, EError error)
    {
        char buf[ERROR_BUFFER_SIZE];
        constexpr size_t maxIndex = ERROR_BUFFER_SIZE - 1ul;

        // copy error
        const ErrorMsg& errorMsg = Error::Lookup(error);

        size_t messageSize = errorMsg.size < maxIndex ? errorMsg.size : maxIndex;
        size_t index = messageSize;
        memcpy(buf, errorMsg.message, messageSize);

        // add format
        std::string format = std::string(" : (line " + std::to_string(memory->cmdLine) + ")").c_str();
        messageSize = format.size();

        if (messageSize > maxIndex - index)
        {
            messageSize = maxIndex - index;
        }
        memcpy(&buf[index], format.c_str(), messageSize);
        index += messageSize;

        // copy command
        SkipToCommandEnd(memory);
        messageSize = memory->ptr + 1 - memory->cmdptr;
        if (messageSize > maxIndex - index)
        {
            messageSize = maxIndex - index;
        }
        memcpy(&buf[index], memory->cmdptr, messageSize);
        index += messageSize;

        // log message
        buf[index] = '\0';
        LOG(LOG_ERROR, buf); // tolog: use channel for in/out or config
    }


    StructureField Config::ParseField(ParseMemory* memory, const Structure* metaData)
    {
        const Type* type = ParseFieldType(memory);

        if (type is nullptr)
        {
            return StructureField();
        }

        StructureField field = ParseFieldName(memory, metaData);

        if (field.IsValid() == false)
        {
            return StructureField();
        }

        if (field.type isnot type)
        {
            LogError(memory, EError::TYPE_MISMATCH);
            return StructureField();
        }

        return field;
    }


    const Type* Config::ParseFieldType(ParseMemory* memory)
    {
        switch (*memory->ptr)
        {
        case 'u':
            if (GuessWord(memory, "unsigned"))
            {
                return ParseUnsignedType(memory);
            }
            break;
        case 'l':
            if (GuessWord(memory, "long")) // first long
            {
                if (GuessWord(memory, "long")) // second long
                {
                    SkipToNextSignificant(memory);

                    return Type::Find("long long");
                }
                else
                {
                    LogError(memory, EError::INVALID_TYPE);
                    return nullptr;
                }
            }
            break;
        default:
            break;
        }

        return ParseCustomType(memory);
    }


    const Type* Config::ParseUnsignedType(ParseMemory* memory)
    {
        if (GuessWord(memory, "int")) // second long
        {
            SkipToNextSignificant(memory);

            return Type::Find("unsigned int");
        }

        if (GuessWord(memory, "short"))
        {
            SkipToNextSignificant(memory);

            return Type::Find("unsigned short");
        }

        if (GuessWord(memory, "long") // first long
            and GuessWord(memory, "long")) // second long
        {
            SkipToNextSignificant(memory);

            return Type::Find("unsigned long long");
        }

        LogError(memory, EError::INVALID_TYPE);
        return nullptr;
    }


    const Type* Config::ParseCustomType(ParseMemory* memory)
    {
        CopyWordToMemory(memory);

        const Type* type = Type::Find(memory->buffer);

        if (type is nullptr)
        {
            LogError(memory, EError::INVALID_TYPE);
            return nullptr;
        }

        SkipToNextSignificant(memory);

        return type;
    }


    StructureField Config::ParseFieldName(ParseMemory* memory, const Structure* metaData)
    {
        CopyWordToMemory(memory);

        StructureField field = metaData->FindField(memory->buffer);

        if (field.IsValid() == false)
        {
            LogError(memory, EError::NAME_MISMATCH);
            return StructureField();
        }

        SkipToNextSignificant(memory);

        return field;
    }


    void Config::ParseValue(ParseMemory* memory, StructureField field, void* out)
    {
        void* variable = (char*)out + field.offset;

        switch (field.type->name.hash)
        {
        case ConstexprCustomHash("int"):
            *(int*)variable = ParseIntValue(memory);
            break;
        case ConstexprCustomHash("unsigned int"):
            *(unsigned int*)variable = ParseUIntValue(memory);
            break;
        case ConstexprCustomHash("float"):
            *(float*)variable = ParseFloatValue(memory);
            break;
        case ConstexprCustomHash("double"):
            *(double*)variable = ParseDoubleValue(memory);
            break;
        case ConstexprCustomHash("char"):
            *(char*)variable = ParseCharValue(memory);
            break;
        case ConstexprCustomHash("long long"): 
            *(long long*)variable = ParseLongValue(memory);
            break;
        case ConstexprCustomHash("unsigned long long"):
            *(unsigned long long*)variable = ParseULongValue(memory);
            break;
        case ConstexprCustomHash("char_32"):
            ParseCharPtrValue(memory, (char*)variable, 32);
            break;
        case ConstexprCustomHash("char_256"):
            ParseCharPtrValue(memory, (char*)variable, 256);
            break;
        default:
            break;
        }
    }

    int Config::ParseIntValue(ParseMemory* memory)
    {
        char* charPtr;

        long value = std::strtol(memory->ptr, &charPtr, 10);

        if (memory->ptr == charPtr)
        {
            LogError(memory, EError::INVALID_VALUE);
        }

        memory->ptr = charPtr;
        SkipToNextSignificant(memory);

        return value;
    }

    unsigned int Config::ParseUIntValue(ParseMemory* memory)
    {
        char* charPtr;

        unsigned long value = std::strtoul(memory->ptr, &charPtr, 10);

        if (memory->ptr == charPtr)
        {
            LogError(memory, EError::INVALID_VALUE);
        }

        memory->ptr = charPtr;
        SkipToNextSignificant(memory);

        return value;
    }

    float Config::ParseFloatValue(ParseMemory* memory)
    {
        char* charPtr;

        float value = std::strtof(memory->ptr, &charPtr);

        if (memory->ptr == charPtr)
        {
            LogError(memory, EError::INVALID_VALUE);
        }

        memory->ptr = charPtr;

        if (*memory->ptr is 'f')
        {
            memory->ptr++;
        }

        SkipToNextSignificant(memory);

        return value;
    }

    double Config::ParseDoubleValue(ParseMemory* memory)
    {
        char* charPtr;

        double value = std::strtod(memory->ptr, &charPtr);

        if (memory->ptr == charPtr)
        {
            LogError(memory, EError::INVALID_VALUE);
        }

        memory->ptr = charPtr;
        SkipToNextSignificant(memory);

        return value;
    }

    char Config::ParseCharValue(ParseMemory* memory)
    {
        if (*memory->ptr++ is '\'')
        {
            char value;
            if (*memory->ptr++ is '\\')
            {
                value = ParseEscapedChar(memory);

                if (value is INVALID_CHAR)
                {
                    return INVALID_CHAR;
                }
            }
            else
            {
                value = *memory->ptr++;
            }

            if (*memory->ptr++ is '\'')
            {
                return value;
            }
        }
        else
        {
            char* charPtr;
            unsigned long intValue = std::strtoul(memory->ptr, &charPtr, 10);
            memory->ptr = charPtr;

            if (memory->ptr != charPtr
                and intValue <= (unsigned long)(char() - 1))
            {
                return (char)intValue;
            }
        }

        LogError(memory, EError::INVALID_VALUE);
        return INVALID_CHAR;
    }

    char Config::ParseEscapedChar(ParseMemory* memory)
    {
        char value;
        switch (*memory->ptr++)
        {
        case '\'':
            value = '\'';
            break;
        case '\"':
            value = '\"';
            break;
        case '\\':
            value = '\\';
            break;
        case 'a':
            value = '\a';
            break;
        case 'b':
            value = '\b';
            break;
        case 'f':
            value = '\f';
            break;
        case 'n':
            value = '\n';
            break;
        case 'r':
            value = '\r';
            break;
        case 't':
            value = '\t';
            break;
        case 'v':
            value = '\v';
            break;
        case 'x':
            value = ParseEscapedChar(memory, 16);
            break;
        default:
            memory->ptr--;
            value = ParseEscapedChar(memory, 8);
            break;
        }

        return value;
    }

    char Config::ParseEscapedChar(ParseMemory* memory, int radix)
    {
        char* charPtr;
        unsigned long intValue = std::strtoul(memory->ptr, &charPtr, radix);

        if (memory->ptr != charPtr
            and intValue <= (unsigned long)(char() - 1))
        {
            memory->ptr = charPtr;
            return (char)intValue;
        }

        LogError(memory, EError::UNKOWN_ESCAPE_SEQUENCE);
        return INVALID_CHAR;
    }

    long long Config::ParseLongValue(ParseMemory* memory)
    {
        char* charPtr;

        long long value = std::strtoll(memory->ptr, &charPtr, 10);

        if (memory->ptr == charPtr)
        {
            LogError(memory, EError::INVALID_VALUE);
        }

        memory->ptr = charPtr;
        SkipToNextSignificant(memory);

        return value;
    }

    unsigned long long Config::ParseULongValue(ParseMemory* memory)
    {
        char* charPtr;

        unsigned long long value = std::strtoull(memory->ptr, &charPtr, 10);

        if (memory->ptr == charPtr)
        {
            LogError(memory, EError::INVALID_VALUE);
        }

        memory->ptr = charPtr;
        SkipToNextSignificant(memory);

        return value;
    }

    void Config::ParseCharPtrValue(ParseMemory* memory, char* dest, int size)
    {
        if (*memory->ptr++ is '\"')
        {
            size_t index = 0;
            while (*memory->ptr isnot '\"'
                and index < size
                and memory->ptr < memory->endPtr)
            {
                if (*memory->ptr is '\\')
                {
                    memory->ptr++;

                    dest[index] = ParseEscapedChar(memory);

                    if (dest[index] is INVALID_CHAR)
                    {
                        return;
                    }
                }
                else
                {
                    dest[index] = *memory->ptr++;
                }

                index++;
            }

            if (index == size)
            {
                LogError(memory, EError::STRING_TOO_LONG);
                return;
            }

            dest[index] = '\0';

            if (*memory->ptr is '\"')
            {
                memory->ptr++;
                SkipToNextSignificant(memory);
                return;
            }
        }

        LogError(memory, EError::INVALID_VALUE);
    }

    // Util

    bool Config::GuessWord(ParseMemory* memory, const char* keyword)
    {
        size_t index = 0;

        while (keyword[index] isnot '\0')
        {
            if (memory->ptr[index] != keyword[index])
            {
                return false;
            }
            index++;
        }

        if (not IsSpaceOrComment(memory->ptr + index))
        {
            return false;
        }

        memory->ptr += index;

        SkipToNextSignificant(memory);

        return true;
    }


    void Config::CopyWordToMemory(ParseMemory* memory)
    {
        if (not IsValidFirstChar(*memory->ptr))
        {
            memory->buffer[0] = '\0';
            LogError(memory, EError::INVALID_NAME);
            return;
        }

        int validCount = 1;

        while (memory->ptr + validCount < memory->endPtr
            and IsValidNameChar(memory->ptr[validCount]))
        {
            validCount++;

            if (validCount > CPP_MAX_NAME_LENGHT)
            {
                memory->buffer[0] = '\0';
                LogError(memory, EError::NAME_TOO_LONG);
                return;
            }
        }

        memcpy(memory->buffer, memory->ptr, validCount);
        memory->buffer[validCount] = '\0';

        memory->ptr += validCount;
        SkipToNextSignificant(memory);
    }


    bool Config::IsValidFirstChar(char c)
    {
        return std::isalpha(c)
            or c is '_';
    }


    bool Config::IsValidNameChar(char c)
    {
        return IsValidFirstChar(c)
            or std::isdigit(c);
    }


    bool Config::IsSpaceOrComment(const char* ptr)
    {
        return std::isspace(*ptr)
            or (ptr[0] is '/'
                and (ptr[1] is '/'
                    or ptr[1] is '*'));
    }


    void Config::SkipToNextSignificant(ParseMemory* memory)
    {
        while (memory->ptr < memory->endPtr)
        {
            switch (*memory->ptr)
            {
            case '/':
                if (*(memory->ptr + 1) is '/')
                {
                    memory->ptr += 2;
                    SkipToNextLine(memory);
                }
                else if (*(memory->ptr + 1) is '*')
                {
                    memory->ptr += 2;
                    SkipComment(memory);
                }
                break;
            case '\n':
            case '\f':
            case '\r':
            case '\t':
            case '\v':
            case ' ':
                break;
            default:
                return;
            }

            memory->ptr++;
        }
    }


    void Config::SkipToNextLine(ParseMemory* memory)
    {
        while (memory->ptr < memory->endPtr
            and *memory->ptr isnot '\n')
        {
            memory->ptr++;
        }

        memory->ptr++;
    }


    void Config::SkipComment(ParseMemory* memory)
    {
        while (memory->ptr < memory->endPtr)
        {
            if (*memory->ptr is '*'
                and *(memory->ptr + 1) is '/')
            {
                memory->ptr += 1;
                return;
            }

            memory->ptr++;
        }
    }


    void Config::SkipToCommandEnd(ParseMemory* memory)
    {
        while (memory->ptr < memory->endPtr
            and *memory->ptr isnot ';')
        {
            if (*memory->ptr is '/')
            {
                if (*(memory->ptr + 1) is '/')
                {
                    memory->ptr += 2;
                    SkipToNextLine(memory);
                }
                else if (*(memory->ptr + 1) is '*')
                {
                    memory->ptr += 2;
                    SkipComment(memory);
                }
            }

            memory->ptr++;
        }
    }


    void Config::SkipToNextCommand(ParseMemory* memory)
    {
        SkipToCommandEnd(memory);
        memory->ptr++; // skip ';'
        SkipToNextSignificant(memory);

        while (memory->cmdptr < memory->ptr)
        {
            if (*memory->cmdptr is '\n')
            {
                memory->cmdLine++;
            }

            memory->cmdptr++;
        }
    }



    Config::ParseMemory::ParseMemory(const char* filePath) :
        rawData(filePath)
    {
        if (not rawData.IsValid())
        {
            //std::cout << "fail to open file : " << file << std::endl; //todo: tolog:
            return;
        }

        ptr = rawData.GetData();
        cmdptr = ptr;
        endPtr = rawData.GetData() + rawData.GetSize();

        SkipToNextSignificant(this);

        while (cmdptr < ptr)
        {
            if (*cmdptr is '\n')
            {
                cmdLine++;
            }

            cmdptr++;
        }
    }
}
