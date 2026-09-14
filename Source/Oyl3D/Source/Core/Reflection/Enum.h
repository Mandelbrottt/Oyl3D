#pragma once

#include "NamedDeclaration.h"
#include "ReflectionParams.h"

namespace Oyl::Reflection
{
	class Type;

	namespace Internal
	{
		class ReflectionFactory;
	}

	struct EnumValue
	{
		// TODO: Add Attribute support
		std::string_view identifier;
		int32 value;
	};

	class Enum : public NamedDeclaration
	{
		friend Internal::ReflectionFactory;

	protected:
		explicit
		Enum(const Internal::EnumParams& a_params);

	public:
		virtual
		~Enum();

		const Type*
		GetUnderlyingType() const
		{
			return m_underlyingType;
		}

		const std::vector<EnumValue>&
		GetValues() const
		{
			return m_enumerations;
		}

	private:
		EnumValue*
		AddValue(const Internal::EnumValueParams& a_params);

	private:
		Type* m_underlyingType = nullptr;

		std::vector<EnumValue> m_enumerations;
	};
}
