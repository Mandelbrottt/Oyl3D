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

	class Variable : public NamedDeclaration
	{
		friend Internal::ReflectionFactory;

	protected:
		explicit
		Variable(const Internal::VariableParams& a_params);

	public:
		const Type*
		GetType() const
		{
			return m_type;
		}

		const Type*
		GetParentType() const
		{
			return m_type;
		}

		AccessSpecifier
		GetAccessSpecifier() const
		{
			return m_accessSpecifier;
		}

	private:
		Type* m_type = nullptr;
		Type* m_parentType = nullptr;
		AccessSpecifier m_accessSpecifier = AccessSpecifier::Global;
	};
}
