#pragma once

#include "../Constant.h"
#include "MemoryMappedFile.h"

namespace Core
{
	enum class EError;

	/**
	* Config is a static class use to read or write custom cpp style config file.
	*
	* @author		Francis Blanchet
	* @version		1.0
	* @since		1.0
	*/
	class  Config
	{
	public:
		Config() = delete;
		Config(const Config&) = delete;
		Config(Config&&) = delete;
		~Config() = delete;

		Config& operator=(const Config&) = delete;
		Config& operator=(Config&&) = delete;

		/**
		* Update an object with data from a config file using reflection.
		*
		* @param filePath	Path of the config source file.
		* @param out		Reference to the object where the config will be store. This
		* 					object need to have reflection data.
		*/
		template<typename T>
		static void Parse(const char* filePath, T& out) { Parse(filePath, &out, T::MetaData); }
		/**
		* Update an object with data from a config file using reflection.
		*
		* @param filePath	Path of the config source file.
		* @param out		Object where the config will be store.
		* @param metaData	Reflection data of the object where the config will be store.
		*/
		static void Parse(const char* filePath, void* out, const class Structure* metaData);

	private:

		struct ParseMemory
		{
			ParseMemory(const char* filePath);

			/** pointer to the start of the current instruction (used for error logging) */
			char const* cmdptr = nullptr;
			/** line number where the current instruction start (used for error logging) */
			unsigned int cmdLine = 1;

			/** pointer to the current position of the parser in the file */
			char const* ptr = nullptr;

			/** pointer to the character after the last character of the file */
			char const* endPtr = nullptr;

			/** buffer use to transfer data from the file to the reflection system */
			char buffer[CPP_MAX_NAME_LENGHT + 1] = {};

		private:
			MemoryMappedFile rawData;
		};

		/**
		* Use logging to keep a trace of error happening while the parsing a config file.
		* This function will log the error and the instruction which produce the error
		* with said instruction line number.
		*
		* @param memory		Data structure keeping track of the parsing progress.
		* @param error		Error to be log.
		*/
		static void LogError(ParseMemory* memory, EError error);

		/**
		* Parse instruction up to the equal symbol (=) and return the corresponding field
		* in the reflection data provided. Return nullptr and log an error if no matching
		* field could be found. Also return nullptr and log an error if the type from the
		* corresponding field doesn't match the type parsed from the config file.
		*
		* @param memory		Data structure keeping track of the parsing progress.
		* @param metaData	Reflection data of the object where the config will be store.
		* @return			Reflection data of the current instruction field.
		*/
		static const struct Field* ParseField(ParseMemory* memory, const Structure* metaData);
		/**
		* Parse first word of the instruction and return the corresponding type from the
		* application reflection data. If the first word is "unsigned" of "long" further
		* parsing will be done to find one of the following type : { long long, unsigned
		* short, unsigned int, unsigned long long } Return nullptr and log an error if no
		* matching type could be found.
		*
		* @param memory		Data structure keeping track of the parsing progress.
		* @return			Reflection data of the current instruction type.
		*/
		static const struct Type* ParseFieldType(ParseMemory* memory);
		/**
		* Parse second word of the instruction for one of the following integer type :
		* { short, int, long long } and return the reflection data associated with the
		* unsigned version of the matching type. Return nullptr and log an error if no
		* matching type could be found.
		*
		* @param memory		Data structure keeping track of the parsing progress.
		* @return			Reflection data of the current instruction type.
		*/
		static const Type* Config::ParseUnsignedType(ParseMemory* memory);
		/**
		* Read first word of the instruction and return the corresponding type from the
		* application reflection data. Return nullptr and log an error if no matching type
		* could be found.
		*
		* @param memory		Data structure keeping track of the parsing progress.
		* @return			Reflection data of the current instruction type.
		*/
		static const Type* Config::ParseCustomType(ParseMemory* memory);
		/**
		* Read next word of the instruction and return the corresponding field in the
		* reflection data provided. Return nullptr and log an error if no matching field
		* could be found.
		*
		* @param memory		Data structure keeping track of the parsing progress.
		* @return			Reflection data of the current instruction field.
		*/
		static const Field* ParseFieldName(ParseMemory* memory, const Structure* metaData);

		/**
		* Parse instruction after the equal symbol (=) and store the config value in the
		* given object using the given field reflection data.
		*
		* @param memory		Data structure keeping track of the parsing progress.
		* @param field		Reflection data of the current instruction associated field.
		* @param out		Object where the config will be store.
		*/
		static void ParseValue(ParseMemory* memory, const Field* field, void* out);
		/**
		* Parse instruction after the equal symbol (=) as an integer and return the value.
		* Log an error if the data after the equal symbol (=) cannot be parsed as an
		* integer.
		*
		* @param memory		Data structure keeping track of the parsing progress.
		* @return			Parsed integer.
		*/
		static int ParseIntValue(ParseMemory* memory);
		/**
		* Parse instruction after the equal symbol (=) as an unsigned integer and return
		* the value. Log an error if the data after the equal symbol (=) cannot be parsed
		* as an unsigned integer.
		*
		* @param memory		Data structure keeping track of the parsing progress.
		* @return			Parsed unsigned integer.
		*/
		static unsigned int ParseUIntValue(ParseMemory* memory);
		/**
		* Parse instruction after the equal symbol (=) as a float and return the value.
		* Log an error if the data after the equal symbol (=) cannot be parsed as a float.
		*
		* @param memory		Data structure keeping track of the parsing progress.
		* @return			Parsed float.
		*/
		static float ParseFloatValue(ParseMemory* memory);
		/**
		* Parse instruction after the equal symbol (=) as a double and return the value.
		* Log an error if the data after the equal symbol (=) cannot be parsed as a
		* double.
		*
		* @param memory		Data structure keeping track of the parsing progress.
		* @return			Parsed double.
		*/
		static double ParseDoubleValue(ParseMemory* memory);
		/**
		* Parse instruction after the equal symbol (=) as a character and return the
		* value. Log an error if the data after the equal symbol (=) cannot be parsed as a
		* character.
		*
		* @param memory		Data structure keeping track of the parsing progress.
		* @return			Parsed character.
		*/
		static char ParseCharValue(ParseMemory* memory);
		/**
		* Parse instruction after the equal symbol (=) as a long long and return the
		* value. Log an error if the data after the equal symbol (=) cannot be parsed as a
		* long long.
		*
		* @param memory		Data structure keeping track of the parsing progress.
		* @return			Parsed character.
		*/
		static long long ParseLongValue(ParseMemory* memory);
		/**
		* Parse instruction after the equal symbol (=) as an unsigned long long and return
		* the value. Log an error if the data after the equal symbol (=) cannot be parsed
		* as an unsigned long long.
		*
		* @param memory		Data structure keeping track of the parsing progress.
		* @return			Parsed character.
		*/
		static unsigned long long ParseULongValue(ParseMemory* memory);
		/**
		* Parse instruction after the equal symbol (=) as an array of character and copy
		* the string at provided destination. Log an error if the data after the equal
		* symbol (=) cannot be parsed as an array of character.
		*
		* @param memory		Data structure keeping track of the parsing progress.
		* @param dest		Memory address where the parsed character will be copied.
		* @param size		Maximum number of character to parse.
		* @return			Parsed character.
		*/
		static void ParseCharPtrValue(ParseMemory* memory, char* dest, int size);

		/**
		* Parse next character as an escaped character or next few character as an ascii
		* number and return the value. Log an error if the next character do not represent
		* an escaped character or a valid ascii number.
		*
		* @param memory		Data structure keeping track of the parsing progress.
		* @return			Parsed character.
		*/
		static char ParseEscapedChar(ParseMemory* memory);
		/**
		* Parse next few character as an ascii number with the given radix and return the
		* value. Log an error if the next few character do not represent a valid ascii
		* number.
		*
		* @param memory		Data structure keeping track of the parsing progress.
		* @param radix		Number format the character will be parse with.
		* @return			Parsed character.
		*/
		static char ParseEscapedChar(ParseMemory* memory, int radix);

		/**
		* Compare the guessed word with the string underlined by the file pointer.
		*
		* @param memory		Data structure keeping track of the parsing progress.
		* @param word		Character string the next significant is guessed to be.
		* @return			If the guessed word match the next significant.
		*/
		static bool GuessWord(ParseMemory* memory, const char* word);
		/**
		* Copy character from the file to a buffer in the ParseMemory data structure as
		* long as those character still represent a valid cpp name (Is made of only
		* letters, digits and underscore. Does not begin by a digit. Is shorter then the
		* maaximum lenght for a cpp name)
		*
		* @param memory		Data structure keeping track of the parsing progress.
		*/
		static void CopyWordToMemory(ParseMemory* memory);

		/**
		* Test if a character can start a cpp name. Return true if it is a letter or an
		* underscore. Return false otherwise.
		*
		* @param c			Character to test.
		* @return			If the character can start a cpp name.
		*/
		static bool IsValidFirstChar(char c);
		/**
		* Test if a character can be part of a cpp name. Return true if it is a letter,
		* a digit or an underscore. Return false otherwise.
		*
		* @param c			Character to test.
		* @return			If the character can be part of a cpp name.
		*/
		static bool IsValidNameChar(char c);
		/**
		* Test if the next character or series of character can be ignore. Return true if
		* the tested string start by a space ' ', an horizontal tab '\t', a vetical tab
		* '\v', a carriage return '\r', a line feed '\n', a form feed '\f', or a comment
		* "//" or "/*". Return false otherwise.
		*
		* @param ptr		Pointer to test.
		* @return			If the next character represent white space or a starting comment.
		*/
		static bool IsSpaceOrComment(const char* ptr);

		/**
		* Move the file pointer to the next significant character skipping all white space
		* and comment in the process.
		*
		* @param memory		Data structure keeping track of the parsing progress.
		*/
		static void SkipToNextSignificant(ParseMemory* memory);
		/**
		* Move the file pointer to the next character after a line feed '\n'
		*
		* @param memory		Data structure keeping track of the parsing progress.
		*/
		static void SkipToNextLine(ParseMemory* memory);
		/**
		* Move the file pointer to the next character after the comment. The file pointer
		* will be on the first character after a line feed '\n' if the comment started by
		* "//" or the first character after "*\/" if the comment started by "/*"
		*
		* @param memory		Data structure keeping track of the parsing progress.
		*/
		static void SkipComment(ParseMemory* memory);
		/**
		* Move the file pointer to the next semicolon ';' not part of a comment or the end of
		* the file. Which ever come first.
		*
		* @param memory		Data structure keeping track of the parsing progress.
		*/
		static void	SkipToCommandEnd(ParseMemory* memory);
		/**
		* Move the file pointer to the next significant after the next significant semicolon
		* ';' or the end of the file. Which ever come first. This also update the cmdptr and
		* the cmdLine.
		*
		* @param memory		Data structure keeping track of the parsing progress.
		*/
		static void	SkipToNextCommand(ParseMemory* memory);
	};
}
