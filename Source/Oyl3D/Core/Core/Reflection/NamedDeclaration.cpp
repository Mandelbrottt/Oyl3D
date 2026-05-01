#include "NamedDeclaration.h"

#include "ReflectionFactory.h"

namespace Oyl::Reflection
{
	NamedDeclaration::NamedDeclaration(const Internal::NamedDeclarationParams& a_params)
		: m_qualifiedName(a_params.qualifiedName),
		  m_name(a_params.name) {}
}
