#pragma once

#include "Enums.h"

namespace Spyll
{
	class AssemblyFactory;
}

namespace Spyll::Reflection
{
	class Type;

	class MemberInfo
	{
		friend AssemblyFactory;

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
