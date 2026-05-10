#include "Function.h"

#include "ReflectionFactory.h"

namespace Oyl::Reflection
{
	Function::Function(const Internal::FunctionParams& a_params)
		: NamedDeclaration(a_params),
		  m_returnType(a_params.returnType),
		  m_isStatic(a_params.isStatic),
		  m_isReturnConst(a_params.isReturnConst),
		  m_isReturnVolatile(a_params.isReturnVolatile),
		  m_isReturnReference(a_params.isReturnReference),
		  m_isReturnPointer(a_params.isReturnPointer) {}

	void
	Function::AddParameter(Variable* a_variable)
	{
		m_parameters.emplace_back(a_variable);
	}
}
