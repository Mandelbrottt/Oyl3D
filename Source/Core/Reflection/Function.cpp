#include "Function.h"

#include "ReflectionFactory.h"

namespace Oyl::Reflection
{
	Function::Function(const Internal::FunctionParams& a_params)
		: NamedDeclaration(a_params), Invokable(a_params) {}

	Function::~Function() {}
}
