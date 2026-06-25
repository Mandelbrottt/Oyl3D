#include "Enum.h"

namespace Oyl::Reflection
{
	Enum::Enum(const Internal::EnumParams& a_params)
		: NamedDeclaration(a_params),
		  m_underlyingType(a_params.underlyingType)
	{
		m_enumerations.reserve(a_params.nValues);
	}

	Enum::~Enum() {}

	EnumValue*
	Enum::AddValue(const Internal::EnumValueParams& a_params)
	{
		EnumValue value;
		value.identifier = a_params.identifier;
		value.value = a_params.value;
		return &m_enumerations.emplace_back(value);
	}
}
