#pragma once

namespace Core
{
	enum class EError
	{
		INVALID_TYPE,
		INVALID_NAME,
		NAME_TOO_LONG,
		UNASSIGNED_VARIABLE,
		NAME_MISMATCH,
		TYPE_MISMATCH,
		INVALID_VALUE,
		MISSING_SEMICOLON,
		UNKOWN_ESCAPE_SEQUENCE,
		STRING_TOO_LONG,
		DOUBLE_SEMICOLON,

		NO_ERROR
	};

	struct ErrorMsg
	{
		constexpr ErrorMsg() :
			ErrorMsg("Unkonwn error message")
		{}

		constexpr ErrorMsg(const char* msg) :
			message(msg),
			size()
		{
			while (msg[size] != '\0') // strlen is not constexpr
			{
				size++;
			}
		}

		const char* message;
		size_t size;
	};

	class ErrorLookup
	{
	public:
		constexpr ErrorLookup() :
			errorList()
		{
			errorList[(int)EError::INVALID_TYPE] = ErrorMsg("Invalid variable type");
			errorList[(int)EError::INVALID_NAME] = ErrorMsg("Invalid variable name");
			errorList[(int)EError::NAME_TOO_LONG] = ErrorMsg("Name is too long");
			errorList[(int)EError::UNASSIGNED_VARIABLE] = ErrorMsg("Variable have no value");
			errorList[(int)EError::NAME_MISMATCH] = ErrorMsg("Variable with this name not found");
			errorList[(int)EError::TYPE_MISMATCH] = ErrorMsg("Variable with this name do not have this type");
			errorList[(int)EError::INVALID_VALUE] = ErrorMsg("Value cannot be convert to variable type");
			errorList[(int)EError::MISSING_SEMICOLON] = ErrorMsg("Missing line termination (;)");
			errorList[(int)EError::UNKOWN_ESCAPE_SEQUENCE] = ErrorMsg("Unkown character escape sequence");
			errorList[(int)EError::STRING_TOO_LONG] = ErrorMsg("Value is too long");
			errorList[(int)EError::DOUBLE_SEMICOLON] = ErrorMsg("Missing line");
			errorList[(int)EError::NO_ERROR] = ErrorMsg("No Error");
		}

		ErrorMsg errorList[(int)EError::NO_ERROR + 1];
	};

    class Error
    {
    public:
		static constexpr const ErrorMsg& Lookup(EError error)
		{
			return LookupTable.errorList[(int)error];
		}

    private:

		static constexpr ErrorLookup LookupTable = ErrorLookup();
    };
}