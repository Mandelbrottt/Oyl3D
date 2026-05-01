#pragma once

#include "Enums.h"

namespace Spyll
{
	class AssemblyFactory;
}

namespace Oyl::Reflection
{
	class Type;

	namespace Internal
	{
		class AssemblyFactory;
	}

	class MemberInfo
	{
		friend Internal::AssemblyFactory;

	protected:
		MemberInfo() = default;

	public:
		const Type*
		GetOwningType() const
		{
			return m_owningType;
		}

		AccessSpecifier
		GetAccessSpecifier() const
		{
			return m_accessSpecifier;
		}

	private:
		Type* m_owningType = nullptr;

		AccessSpecifier m_accessSpecifier = AccessSpecifier::None;
	};
}
