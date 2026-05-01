#include "MemberInfo.h"

#include "ReflectionFactory.h"

namespace Oyl::Reflection
{
	MemberInfo::MemberInfo(const Internal::MemberInfoParams& a_params)
		: m_owningType(a_params.ownerType),
		  m_accessSpecifier(a_params.accessSpecifier) {}
}
