#include "Variable.h"

#include "ReflectionFactory.h"

namespace Oyl::Reflection
{
	Variable::Variable(const Internal::VariableParams& a_params)
		: NamedDeclaration(a_params),
		  QualifierInfo(a_params),
		  m_type(a_params.type),
		  m_isReference(a_params.isReference),
		  m_isPointer(a_params.isPointer) {}
}
