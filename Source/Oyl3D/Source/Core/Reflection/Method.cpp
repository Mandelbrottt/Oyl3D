#include "Method.h"

#include "ReflectionFactory.h"

namespace Oyl::Reflection
{
	Method::Method(const Internal::MethodParams& a_params)
		: Function(a_params),
		  m_isVirtual(a_params.isVirtual),
		  m_isPureVirtual(a_params.isPureVirtual) {}
}
