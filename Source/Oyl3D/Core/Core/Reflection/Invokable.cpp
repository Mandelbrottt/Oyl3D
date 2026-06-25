#include "Invokable.h"

#include "ReflectionFactory.h"

namespace Oyl::Reflection
{
	Invokable::Invokable(const Internal::InvokableParams& a_params)
	{
		m_signature.returnType = a_params.returnType;
		m_functionPtr = a_params.functionPtr;
	}

	Invokable::~Invokable() {}

	Argument*
	Invokable::AddArgument(const Internal::ArgumentParams& a_argument)
	{
		Argument arg;
		arg.type = a_argument.type;
		arg.name = a_argument.name;
		return &m_signature.arguments.emplace_back(arg);
	}
}
