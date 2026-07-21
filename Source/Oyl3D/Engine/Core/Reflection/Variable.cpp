#include "Variable.h"

#include "ReflectionFactory.h"

namespace Oyl::Reflection
{
	Variable::Variable(const Internal::VariableParams& a_params)
		: NamedDeclaration(a_params),
		  m_type(a_params.type),
		  m_parentType(a_params.parentType),
		  m_accessSpecifier(a_params.accessSpecifier) {}
}
