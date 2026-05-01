#include "QualifierInfo.h"

#include "ReflectionFactory.h"

namespace Oyl::Reflection
{
	QualifierInfo::QualifierInfo(const Internal::QualifierInfoParams& a_params)
		: m_isConst(a_params.isConst),
		  m_isVolatile(a_params.isVolatile) {}
}
