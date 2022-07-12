#pragma once

#include "../Constant.h"
#include "MemoryMappedFile.h"
#include "core/reflection/StructureField.h"

namespace Core
{
	enum class EError;

	class  Config
	{
	public:
		Config() = delete;
		Config(const Config&) = delete;
		Config(Config&&) = delete;
		~Config() = delete;

		Config& operator=(const Config&) = delete;
		Config& operator=(Config&&) = delete;

		template<typename T>
		static void Parse(const char* filePath, T& out) { Parse(filePath, &out, T::MetaData); }
		static void Parse(const char* filePath, void* out, const class Structure* metaData);

	private:

		struct ParseMemory
		{
			ParseMemory(const char* filePath);

			char const* cmdptr = nullptr;
			unsigned int cmdLine = 1;

			char const* ptr = nullptr;

			char const* endPtr = nullptr;

			char buffer[CPP_MAX_NAME_LENGHT + 1] = {};

		private:
			MemoryMappedFile rawData;
		};

		static void LogError(ParseMemory* memory, EError error);

		static StructureField ParseField(ParseMemory* memory, const Structure* metaData);
		static const struct Type* ParseFieldType(ParseMemory* memory);
		static const Type* Config::ParseUnsignedType(ParseMemory* memory);
		static const Type* Config::ParseCustomType(ParseMemory* memory);
		static StructureField ParseFieldName(ParseMemory* memory, const Structure* metaData);

		static void ParseValue(ParseMemory* memory, StructureField field, void* out);
		static int ParseIntValue(ParseMemory* memory);
		static unsigned int ParseUIntValue(ParseMemory* memory);
		static float ParseFloatValue(ParseMemory* memory);
		static double ParseDoubleValue(ParseMemory* memory);
		static char ParseCharValue(ParseMemory* memory);
		static long long ParseLongValue(ParseMemory* memory);
		static unsigned long long ParseULongValue(ParseMemory* memory);
		static void ParseCharPtrValue(ParseMemory* memory, char* dest, int size);

		static char ParseEscapedChar(ParseMemory* memory);
		static char ParseEscapedChar(ParseMemory* memory, int radix);

		// util
		static bool GuessWord(ParseMemory* memory, const char* word);
		static void CopyWordToMemory(ParseMemory* memory);

		static bool IsValidFirstChar(char c);
		static bool IsValidNameChar(char c);
		static bool IsSpaceOrComment(const char* ptr);

		static void SkipToNextSignificant(ParseMemory* memory); // skip white space and comment
		static void SkipToNextLine(ParseMemory* memory);
		static void SkipComment(ParseMemory* memory);
		static void	SkipToCommandEnd(ParseMemory* memory);
		static void	SkipToNextCommand(ParseMemory* memory);
	};
}
