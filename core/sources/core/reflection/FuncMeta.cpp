#include "FuncMeta.h"

namespace Core
{
	Function::Function(const std::string & pName, const std::string & pType, bool pIsStatic) :
		name(pName),
		type((Type*)GetHashWithoutConst(pType)),
		isStatic(pIsStatic)
	{}

	Function::Function(const std::string& pName, const std::string& pType, std::initializer_list<Parameter> pParams, bool pIsStatic) :
		name(pName.c_str()),
		type((Type*)GetHashWithoutConst(pType)),
		m_parameters(std::move(pParams)),
		isStatic(pIsStatic)
	{}

	Result Function::Invoke(void* ref) const
	{
		InvokeImpl(ref);

		return Result(buffer, type);
	}

	const Function& Function::SetParam(const std::string& paramName, const void* value) const
	{
		for (const Parameter& param : m_parameters)
		{
			if (param.name.hash == ConstexprCustomHash(paramName.c_str()))
			{
				memcpy(buffer + param.offset, value, param.type->size);
				return *this;
			}
		}

		// todo: log error param not found
		return *this;
	}

	void* Function::GetParamValue(const std::string& paramName) const
	{
		for (const Parameter& param : m_parameters)
		{
			if (param.name.hash == ConstexprCustomHash(paramName.c_str()))
			{
				return buffer + param.offset;
			}
		}

		// todo: log error param not found
		return nullptr;
	}
}
