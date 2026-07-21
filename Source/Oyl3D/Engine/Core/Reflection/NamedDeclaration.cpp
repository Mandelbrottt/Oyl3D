#include "NamedDeclaration.h"

#include "ReflectionFactory.h"

namespace Oyl::Reflection
{
	NamedDeclaration::NamedDeclaration(const Internal::NamedDeclarationParams& a_params)
		: m_qualifiedName(a_params.qualifiedName),
		  m_name(a_params.name) {}

	NamedDeclaration::~NamedDeclaration()
	{
		for (auto*& attribute : m_attributes)
		{
			delete attribute;
			attribute = nullptr;
		}
		m_attributes.clear();
	}
}
