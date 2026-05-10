#include "MemberFunction.h"

#include "ReflectionFactory.h"

namespace Oyl::Reflection
{
	MemberFunction::MemberFunction(const Internal::MemberFunctionParams& a_params)
		: Function(a_params),
		  MemberInfo(a_params),
		  m_constructorType(a_params.constructorType),
		  m_isVirtual(a_params.isVirtual),
		  m_isPureVirtual(a_params.isPureVirtual),
		  m_isOverride(a_params.isOverride) {}
}
