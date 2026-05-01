#pragma once

#include "Enums.h"

namespace Oyl::Reflection::Internal {
	struct MemberInfoParams;
}

namespace Oyl::Reflection
{
	class Type;

	namespace Internal
	{
		class ReflectionFactory;
	}

	class MemberInfo
	{
		friend Internal::ReflectionFactory;

	protected:
		explicit
		MemberInfo(const Internal::MemberInfoParams& a_params);

	public:
		Type*
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
